#include <stdio.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/color_space.hpp>
#include <imgui.h>
#include "apps/04_Forward_Rendering/04ForwardRendering.h"
#include "core/cube.h"

IMPLEMENT_APPLICATION(ForwardRenderingApplication);

const float VIEW_DISTANCE = 500.0f;

void ForwardRenderingApplication::onInit()
{
   camera.setPosition(glm::vec3(9.0f, 25.0f, 9.0f));
   camera.setYawPitch(-2.34f, -1.20f);
   updatePerspectiveMatrix();

   getWindowSize(windowWidth, windowHeight);
   setWindowTitle("Forward Rendering Application");

   createCubeData();

   initGL();
}

void ForwardRenderingApplication::onDestroy()
{
   destroyGL();
}

void ForwardRenderingApplication::onUpdate(double dt)
{
   updateCamera(dt);
   render(dt);
}

void ForwardRenderingApplication::onWindowSizeUpdate(int width, int height)
{
   windowWidth = width;
   windowHeight = height;
}

void ForwardRenderingApplication::updateCamera(double dt)
{
   Move move = {};
   camera.update(dt, move);
   updatePerspectiveMatrix();
}

void ForwardRenderingApplication::updatePerspectiveMatrix()
{
   camera.setProjectionMatrix(glm::perspective((float)glm::radians(110.0), getAspectRatio(), 0.01f, VIEW_DISTANCE));

   camera.getMatrices(cameraData.projMatrix, cameraData.viewMatrix);
}

void ForwardRenderingApplication::initGL()
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
   createLights(8);
}

void ForwardRenderingApplication::initUBOs()
{
   glGenBuffers(1, &cameraUbo);
   glBindBuffer(GL_UNIFORM_BUFFER, cameraUbo);
   glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraUbo), NULL, GL_DYNAMIC_DRAW);

   glGenBuffers(1, &lightUbo);
   glBindBuffer(GL_UNIFORM_BUFFER, lightUbo);
   glBufferData(GL_UNIFORM_BUFFER, sizeof(LightUbo), NULL, GL_STATIC_DRAW);

   glGenBuffers(1, &cubeUbo);
   glBindBuffer(GL_UNIFORM_BUFFER, cubeUbo);
   glBufferData(GL_UNIFORM_BUFFER, sizeof(CubeUbo), &cubeData, GL_STATIC_DRAW);
}

void ForwardRenderingApplication::createLights(int count)
{
   if (count > LIGHT_COUNT)
      count = LIGHT_COUNT;
   if (count < 0)
      count = 0;

   memset(&lightData, 0, sizeof(LightUbo));
   lightData.lightCount = count;

   for (int i = 0; i < count; i++)
   {
      glm::vec4 pos = cubeData.modelMatrix[i][3];
      pos.y += 5.0f;
      lightData.lights[i].position = glm::vec3(pos);
      lightData.lights[i].radius = 3.0f;
      lightData.lights[i].color = glm::vec4(glm::rgbColor(glm::vec3(rand()/(float)RAND_MAX*360.0f, 1.0f, 0.2f)), 1.0);
   }

   glBindBuffer(GL_UNIFORM_BUFFER, lightUbo);
   glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightUbo), &lightData);
}

void ForwardRenderingApplication::initShader()
{
   char* vertShader = readShaderFile("apps/04_Forward_Rendering/shaders/cube.vert");
   char* fragShader = readShaderFile("apps/04_Forward_Rendering/shaders/cube.frag");

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

   uniformCameraLocationBlock = glGetUniformBlockIndex(shaderProgram, "CameraBuffer");
   uniformLightLocationBlock = glGetUniformBlockIndex(shaderProgram, "LightBuffer");
   uniformCubeLocationBlock = glGetUniformBlockIndex(shaderProgram, "CubeInstanceBuffer");

   cameraUboLocation = 0;
   lightUboLocation = 1;
   cubeUboLocation = 2;

   glUniformBlockBinding(shaderProgram, uniformCameraLocationBlock, cameraUboLocation);
   glUniformBlockBinding(shaderProgram, uniformLightLocationBlock, lightUboLocation);
   glUniformBlockBinding(shaderProgram, uniformCubeLocationBlock, cubeUboLocation);
}

void ForwardRenderingApplication::destroyGL()
{
   glUseProgram(0);
   glDeleteProgram(shaderProgram);

   GLuint deleteBuffers[5] = { vbo, ibo, cameraUbo, lightUbo, cubeUbo };
   glDeleteBuffers(5, deleteBuffers);

   glBindVertexArray(0);
   glDeleteVertexArrays(1, &vao);
}

void ForwardRenderingApplication::render(double dt)
{
   glViewport(0, 0, windowWidth, windowHeight);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glClearColor(0.0, 0.0, 0.0, 1.0);

   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glCullFace(GL_FRONT);
   glFrontFace(GL_CW);

   glUseProgram(shaderProgram);

   glBindBuffer(GL_UNIFORM_BUFFER, cameraUbo);
   glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraUbo), &cameraData);

   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

   glBindBufferBase(GL_UNIFORM_BUFFER, cameraUboLocation, cameraUbo);
   glBindBufferBase(GL_UNIFORM_BUFFER, lightUboLocation, lightUbo);
   glBindBufferBase(GL_UNIFORM_BUFFER, cubeUboLocation, cubeUbo);

   glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, NULL, CUBE_COUNT);
}

void ForwardRenderingApplication::onRenderImGUI(double dt)
{
   ImGui::NewFrame();
   ImGui::Begin("Debug Information & Options");
   //ImGui::SetWindowSize(ImVec2(700, 180));
   ImGui::Text("Frame Rate: %.1f FPS", ImGui::GetIO().Framerate);
   ImGui::Separator();

   ImGui::Text("OpenGL Driver Information:");
   ImGui::Text("   Renderer: %s", glGetString(GL_RENDERER));
   ImGui::Text("   Vendor: %s", glGetString(GL_VENDOR));
   ImGui::Text("   Version: %s", glGetString(GL_VERSION));

   ImGui::Separator();
   if (ImGui::InputInt("Light Count", &lightData.lightCount))
   {
      createLights(lightData.lightCount);
   }

   for (int i = 0; i < lightData.lightCount; i++)
   {
      glm::vec4 color = lightData.lights[i].color;
      glm::vec3 pos = lightData.lights[i].position;
      ImGui::Text("Light[%d] Position(%f, %f, %f) Color (%f, %f, %f)", i, pos.x, pos.y, pos.z, color.x, color.y, color.z);
   }

   ImGui::End();
   ImGui::Render();
}

void ForwardRenderingApplication::createCubeData()
{
   int gridSize = (int)sqrt(CUBE_COUNT);

   int idx = 0;
   for (int x = -gridSize / 2; x < gridSize / 2; x++)
   {
      for (int z = -gridSize / 2; z < gridSize / 2; z++)
      {
         glm::mat4 mat = glm::translate(glm::mat4(1.0), glm::vec3(x * 4, 0, z * 4));
         mat = glm::scale(mat, glm::vec3(2));
         cubeData.modelMatrix[idx++] = mat;
      }
   }
}