#include <stdio.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include "apps/03_Draw_Performance/03DrawPerformance.h"
#include "core/cube.h"

IMPLEMENT_APPLICATION(DrawPerformanceApplication);

const float VIEW_DISTANCE = 500.0f;

void DrawPerformanceApplication::onInit()
{
   camera.setPosition(glm::vec3(18.0f, 50.0f, 18.5f));
   camera.setYawPitch(-2.34f, -1.20f);
   updatePerspectiveMatrix();

   getWindowSize(windowWidth, windowHeight);
   setWindowTitle("Draw Performance Application");

   sunData.sunDir = glm::vec4(0.32f, 0.75f, 0.54f, 0.0f);
   sunData.sunColor = glm::vec4(1.4f, 1.2f, 0.4f, 0.0f);
   sunData.ambientColor = glm::vec4(0.3f, 0.3f, 0.4f, 0.0f);

   gridSize = 100;

   createCubeData();

   initGL();
}

void DrawPerformanceApplication::onDestroy()
{
   destroyGL();
}

void DrawPerformanceApplication::onUpdate(double dt)
{
   updateCamera(dt);
   render(dt);
}

void DrawPerformanceApplication::onWindowSizeUpdate(int width, int height)
{
   windowWidth = width;
   windowHeight = height;
}

void DrawPerformanceApplication::updateCamera(double dt)
{
   Move move = {};
   camera.update(dt, move);
   updatePerspectiveMatrix();
}

void DrawPerformanceApplication::updatePerspectiveMatrix()
{
   camera.setProjectionMatrix(glm::perspective((float)glm::radians(110.0), getAspectRatio(), 0.01f, VIEW_DISTANCE));

   camera.getMatrices(cameraData.projMatrix, cameraData.viewMatrix);
}

void DrawPerformanceApplication::initGL()
{
   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);

   glGenBuffers(1, &vbo);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertsBuffer), cubeVertsBuffer, GL_STATIC_DRAW);

   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, NULL);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)12);

   glGenBuffers(1, &ibo);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

   initShader();
   initUBOs();
}

void DrawPerformanceApplication::initUBOs()
{
   glGenBuffers(1, &cameraUbo);
   glBindBuffer(GL_UNIFORM_BUFFER, cameraUbo);
   glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraUbo), NULL, GL_DYNAMIC_DRAW);

   glGenBuffers(1, &sunUbo);
   glBindBuffer(GL_UNIFORM_BUFFER, sunUbo);
   glBufferData(GL_UNIFORM_BUFFER, sizeof(SunUbo), &sunData, GL_STATIC_DRAW);
}

void DrawPerformanceApplication::initShader()
{
   char* vertShader = readShaderFile("apps/03_Draw_Performance/shaders/cube.vert");
   char* fragShader = readShaderFile("apps/03_Draw_Performance/shaders/cube.frag");

   GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vShader, 1, &vertShader, NULL);
   glCompileShader(vShader);
   validateShaderCompilation(vShader);

   GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fShader, 1, &fragShader, NULL);
   glCompileShader(fShader);
   validateShaderCompilation(fShader);

   shaderProgram = glCreateProgram();
   glAttachShader(shaderProgram, vShader);
   glAttachShader(shaderProgram, fShader);
   glLinkProgram(shaderProgram);
   glDetachShader(shaderProgram, vShader);
   glDetachShader(shaderProgram, fShader);
   glDeleteShader(vShader);
   glDeleteShader(fShader);
   validateShaderLinkCompilation(shaderProgram);

   free(vertShader);
   free(fragShader);

   uniformModelMatLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
   uniformSunLocationBlock = glGetUniformBlockIndex(shaderProgram, "SunBuffer");
   uniformCameraLocationBlock = glGetUniformBlockIndex(shaderProgram, "CameraBuffer");

   cameraUboLocation = 0;
   sunUboLocation = 1;

   glUniformBlockBinding(shaderProgram, uniformCameraLocationBlock, cameraUboLocation);
   glUniformBlockBinding(shaderProgram, uniformSunLocationBlock, sunUboLocation);
}

void DrawPerformanceApplication::destroyGL()
{
   glUseProgram(0);
   glDeleteProgram(shaderProgram);

   GLuint deleteBuffers[4] = { vbo, ibo, cameraUbo, sunUbo };
   glDeleteBuffers(4, deleteBuffers);

   glBindVertexArray(0);
   glDeleteVertexArrays(1, &vao);
}

void DrawPerformanceApplication::render(double dt)
{
   glViewport(0, 0, windowWidth, windowHeight);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glClearColor(0.0, 0.0, 0.0, 1.0);

   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glCullFace(GL_FRONT);
   glFrontFace(GL_CW);

   glUseProgram(shaderProgram);

   // Update camera data
   glBindBuffer(GL_UNIFORM_BUFFER, cameraUbo);
   glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraUbo), &cameraData);

   // Draw Cube
   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

   glBindBufferBase(GL_UNIFORM_BUFFER, cameraUboLocation, cameraUbo);
   glBindBufferBase(GL_UNIFORM_BUFFER, sunUboLocation, sunUbo);

   for (int i = 0; i < gridSize * gridSize; ++i)
   {
      glUniformMatrix4fv(uniformModelMatLocation, 1, GL_FALSE, (const GLfloat*)&cubeData[i]);
      glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, NULL);
   }
}

void DrawPerformanceApplication::onRenderImGUI(double dt)
{
   ImGui::NewFrame();
   ImGui::Begin("Debug Information & Options");
   ImGui::SetWindowSize(ImVec2(700, 180));
   ImGui::Text("Frame Rate: %.1f FPS", ImGui::GetIO().Framerate);
   ImGui::Text("# of Cubes Rendering (# Drawcalls): %d", gridSize * gridSize);
   ImGui::Separator();

   ImGui::Text("OpenGL Driver Information:");
   ImGui::Text("   Renderer: %s", glGetString(GL_RENDERER));
   ImGui::Text("   Vendor: %s", glGetString(GL_VENDOR));
   ImGui::Text("   Version: %s", glGetString(GL_VERSION));

   ImGui::Separator();

   if (ImGui::InputInt("Grid Size", &gridSize))
   {
      createCubeData();
   }

   ImGui::End();
   ImGui::Render();
}

void DrawPerformanceApplication::createCubeData()
{
   if (cubeData)
      delete[] cubeData;

   cubeData = new CubeData[gridSize * gridSize];

   int idx = 0;
   for (int x = -gridSize / 2; x < gridSize/2; x++)
   {
      for (int z = -gridSize / 2; z < gridSize/2; z++)
      {
         glm::mat4 mat = glm::translate(glm::mat4(1.0), glm::vec3(x, 0, z));
         mat = glm::scale(mat, glm::vec3(0.8, 0.8, 0.8));
         cubeData[idx++].matrix = mat;
      }
   }
}