#include <stdio.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/color_space.hpp>
#include <imgui.h>
#include "apps/05_Shadows/05Shadows.h"
#include "core/groundplane.h"
#include "gl/shader.h"

IMPLEMENT_APPLICATION(ShadowsApplication);

const float VIEW_DISTANCE = 500.0f;

void ShadowsApplication::onInit()
{
   camera.setPosition(glm::vec3(4.5f, 10.0f, 4.5f));
   camera.setYawPitch(-2.34f, -1.20f);
   updatePerspectiveMatrix();

   getWindowSize(windowWidth, windowHeight);
   setWindowTitle("Shadows Application");

   sunData.sunDir = glm::vec4(0.32f, 0.75f, 0.54f, 0.0f);
   sunData.sunColor = glm::vec4(1.4f, 1.2f, 0.4f, 0.0f);
   sunData.ambientColor = glm::vec4(0.3f, 0.3f, 0.4f, 0.0f);
   
   initGL();
}

void ShadowsApplication::onDestroy()
{
   destroyGL();
}

void ShadowsApplication::onUpdate(double dt)
{
   updateCamera(dt);
   render(dt);
}

void ShadowsApplication::onWindowSizeUpdate(int width, int height)
{
   windowWidth = width;
   windowHeight = height;
}

void ShadowsApplication::updateCamera(double dt)
{
   Move move = {};
   camera.update(dt, move);
   updatePerspectiveMatrix();
}

void ShadowsApplication::updatePerspectiveMatrix()
{
   camera.setProjectionMatrix(glm::perspective((float)glm::radians(110.0), getAspectRatio(), 0.01f, VIEW_DISTANCE));

   camera.getMatrices(cameraData.projMatrix, cameraData.viewMatrix);
}

void ShadowsApplication::initGL()
{
   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);

   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, NULL);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)12);

   glGenBuffers(1, &groundVbo);
   glBindBuffer(GL_ARRAY_BUFFER, groundVbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(groundPlaneVertexBuffer), groundPlaneVertexBuffer, GL_STATIC_DRAW);
   
   initShader();
   initUBOs();
}

void ShadowsApplication::initUBOs()
{
   glGenBuffers(1, &cameraUbo);
   glBindBuffer(GL_UNIFORM_BUFFER, cameraUbo);
   glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraUbo), NULL, GL_DYNAMIC_DRAW);

   glGenBuffers(1, &sunUbo);
   glBindBuffer(GL_UNIFORM_BUFFER, sunUbo);
   glBufferData(GL_UNIFORM_BUFFER, sizeof(SunUbo), &sunData, GL_STATIC_DRAW);
}

void ShadowsApplication::initShader()
{
   char* vertShader = readShaderFile("apps/05_Shadows/shaders/models.vert");
   char* fragShader = readShaderFile("apps/05_Shadows/shaders/models.frag");

   geometryProgram = createVertexAndFragmentShaderProgram(vertShader, fragShader);

   free(vertShader);
   free(fragShader);

   uniformCameraLocationBlock = glGetUniformBlockIndex(geometryProgram, "CameraBuffer");
   uniformSunLocationBlock = glGetUniformBlockIndex(geometryProgram, "LightBuffer");
   objectColorUboLocation = glGetUniformLocation(geometryProgram, "objectColor");
   modelMatrixUboLocation = glGetUniformLocation(geometryProgram, "modelMatrix");
   
   cameraUboLocation = 0;
   sunUboLocation = 1;

   glUniformBlockBinding(geometryProgram, uniformCameraLocationBlock, cameraUboLocation);
   glUniformBlockBinding(geometryProgram, uniformSunLocationBlock, sunUboLocation);
}

void ShadowsApplication::destroyGL()
{
   glUseProgram(0);
   glDeleteProgram(geometryProgram);

   GLuint deleteBuffers[3] = { cameraUbo, sunUbo, groundVbo };
   glDeleteBuffers(3, deleteBuffers);

   glBindVertexArray(0);
   glDeleteVertexArrays(1, &vao);
}

void ShadowsApplication::render(double dt)
{
   glViewport(0, 0, windowWidth, windowHeight);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glClearColor(0.0, 0.0, 0.0, 1.0);

   glEnable(GL_DEPTH_TEST);
   //glEnable(GL_CULL_FACE);
   //glCullFace(GL_BACK);
   //glFrontFace(GL_CCW);

   glUseProgram(geometryProgram);

   glBindBuffer(GL_UNIFORM_BUFFER, cameraUbo);
   glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraUbo), &cameraData);

   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, groundVbo);

   glBindBufferBase(GL_UNIFORM_BUFFER, cameraUboLocation, cameraUbo);
   glBindBufferBase(GL_UNIFORM_BUFFER, sunUboLocation, sunUbo);
      
   glm::mat4 model(1.0);
   model = glm::scale(model, glm::vec3(10.0, 1.0, 10.0));
   glUniformMatrix4fv(modelMatrixUboLocation, 1, GL_FALSE, &(model[0][0]));
   glUniform4f(objectColorUboLocation, 1.0f, 0.0f, 0.0f, 1.0f);

   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void ShadowsApplication::onRenderImGUI(double dt)
{
   ImGui::NewFrame();
   ImGui::Begin("Debug Information & Options");
   ImGui::Text("Frame Rate: %.1f FPS", ImGui::GetIO().Framerate);
   ImGui::Separator();

   ImGui::Text("OpenGL Driver Information:");
   ImGui::Text("   Renderer: %s", glGetString(GL_RENDERER));
   ImGui::Text("   Vendor: %s", glGetString(GL_VENDOR));
   ImGui::Text("   Version: %s", glGetString(GL_VERSION));


   ImGui::End();
   ImGui::Render();
}
