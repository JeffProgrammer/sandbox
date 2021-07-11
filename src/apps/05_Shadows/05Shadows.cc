#include <stdio.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/color_space.hpp>
#include <imgui.h>
#include "apps/05_Shadows/05Shadows.h"
#include "core/groundplane.h"
#include "core/cube.h"
#include "gl/shader.h"

IMPLEMENT_APPLICATION(ShadowsApplication);

const float VIEW_DISTANCE = 100.0f;

void ShadowsApplication::onInit()
{
   camera.setPosition(glm::vec3(4.5f, 10.0f, 4.5f));
   camera.setYawPitch(-2.34f, -1.20f);
   updatePerspectiveMatrix();

   getWindowSize(windowWidth, windowHeight);
   setWindowTitle("Shadows Application");

   sunData.sunDir = glm::vec4(0.32f, 0.75f, 0.54f, 0.0f);
   sunData.sunColor = glm::vec4(1.4f, 1.2f, 0.4f, 0.0f);
   sunData.ambientColor = glm::vec4(0.3f, 0.3f, 0.4f, 1.0f);
   
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

   if (isKeyPressed(Key::FORWARD))   move.y += 1.0f;
   if (isKeyPressed(Key::BACKWARDS)) move.y -= 1.0f;
   if (isKeyPressed(Key::RIGHT))     move.x += 1.0f;
   if (isKeyPressed(Key::LEFT))      move.x -= 1.0f;

   glm::vec2 mouse = getMouseDelta();
   move.pitch = mouse.y;
   move.yaw = mouse.x;

   camera.update(dt, move);
   updatePerspectiveMatrix();
}

void ShadowsApplication::updatePerspectiveMatrix()
{
   camera.setProjectionMatrix(glm::perspective((float)glm::radians(90.0), getAspectRatio(), 0.01f, VIEW_DISTANCE));

   camera.getMatrices(cameraData.projMatrix, cameraData.viewMatrix);

   glm::mat4 ortho = glm::ortho<float>(-20, 20, -20, 20, 0, 20);

   glm::vec3 lightDir = -glm::normalize(glm::vec3(sunData.sunDir));
   glm::mat4 lightView = glm::lookAt(lightDir, glm::vec3(0.0f), glm::vec3(0, 1.0f, 0));

   shadows.shadowCamera.projMatrix = ortho;
   shadows.shadowCamera.viewMatrix = lightView;
   shadows.shadowCamera.shadowMatrix = glm::mat4(1.0f);
}

void ShadowsApplication::initGL()
{
   glClearColor(0.0, 0.0, 0.0, 1.0);

   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);
   
   // We store all geometry shapes into 1 vbo. We allocate a vbo that's large enough
   glGenBuffers(1, &vbo);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   {
      glBufferData(GL_ARRAY_BUFFER, 4096, NULL, GL_STATIC_DRAW);

      groundPlaneVboOffset = 0;
      cubeVboOffset = sizeof(groundPlaneVertexBuffer);

      glBufferSubData(GL_ARRAY_BUFFER, groundPlaneVboOffset, sizeof(groundPlaneVertexBuffer), groundPlaneVertexBuffer);
      glBufferSubData(GL_ARRAY_BUFFER, cubeVboOffset, sizeof(cubeVertsBuffer), cubeVertsBuffer);
   }

   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, NULL);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)12);

   glBindVertexArray(0);

   // shadowmap vao
   glGenVertexArrays(1, &shadows.vao);
   glBindVertexArray(shadows.vao);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, NULL);

   // same as vbo...pack the index buffer
   glGenBuffers(1, &ibo);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
   {
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4096, NULL, GL_STATIC_DRAW);

      groundPlaneIboOffset = 0;
      cubeIboOffset = sizeof(groundPlaneIndexBuffer);

      // I wanted to give an example of glMapBuffer here outside of glBufferSubData.
      // Note that we use a char* type for the buffer, as we're working with bytes when we copy.
      char* buffer = (char*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
      {
         memcpy(buffer + 0, groundPlaneIndexBuffer, sizeof(groundPlaneIndexBuffer));
         memcpy(buffer + sizeof(groundPlaneIndexBuffer), cubeIndices, sizeof(cubeIndices));
      }
      glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
   }

   initShader();
   initUBOs();

   glGenFramebuffers(1, &shadows.fbo);
   glBindFramebuffer(GL_FRAMEBUFFER, shadows.fbo);

   glGenTextures(1, &shadows.shadowTexture2DMap);
   glBindTexture(GL_TEXTURE_2D, shadows.shadowTexture2DMap);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, shadows.width, shadows.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

   glDrawBuffer(GL_NONE);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadows.shadowTexture2DMap, 0);

   if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      abort();

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   glBindTexture(GL_TEXTURE_2D, 0);
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

   GLuint uniformCameraLocationBlock = glGetUniformBlockIndex(geometryProgram, "CameraBuffer");
   GLuint uniformSunLocationBlock = glGetUniformBlockIndex(geometryProgram, "SunBuffer");

   objectColorUboLocation = glGetUniformLocation(geometryProgram, "objectColor");
   modelMatrixUboLocation = glGetUniformLocation(geometryProgram, "modelMatrix");
   shadowTextureLocation  = glGetUniformLocation(geometryProgram, "shadowMap");
   
   cameraUboLocation = 0;
   sunUboLocation = 1;

   glUniformBlockBinding(geometryProgram, uniformCameraLocationBlock, cameraUboLocation);
   glUniformBlockBinding(geometryProgram, uniformSunLocationBlock, sunUboLocation);

   // shadows

   char* shadowVertShader = readShaderFile("apps/05_Shadows/shaders/shadow.vert");
   char* shadowFragShader = readShaderFile("apps/05_Shadows/shaders/shadow.frag");

   shadows.shaderProgram = createVertexAndFragmentShaderProgram(shadowVertShader, shadowFragShader);

   free(shadowVertShader);
   free(shadowFragShader);

   GLuint uniformCameraLocationBlockShadows = glGetUniformBlockIndex(shadows.shaderProgram, "CameraBuffer");
   shadows.modelMatrixLocation = glGetUniformLocation(shadows.shaderProgram, "modelMatrix");
   glUniformBlockBinding(shadows.shaderProgram, uniformCameraLocationBlockShadows, cameraUboLocation);
}

void ShadowsApplication::destroyGL()
{
   glUseProgram(0);
   glDeleteProgram(geometryProgram);

   GLuint deleteBuffers[4] = { cameraUbo, sunUbo, vbo, ibo };
   glDeleteBuffers(4, deleteBuffers);

   glDeleteTextures(1, &shadows.shadowTexture2DMap);
   glDeleteFramebuffers(1, &shadows.fbo);

   glBindVertexArray(0);
   GLuint vaos[2] = { vao, shadows.vao };
   glDeleteVertexArrays(2, vaos);
}

void ShadowsApplication::render(double dt)
{
   glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);
   //glFrontFace(GL_CCW);

   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);
   glClearDepth(1.0);
   glDepthMask(GL_TRUE);

https://arm-software.github.io/opengl-es-sdk-for-android/projected_lights.html
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(1.0, 1.0);

   // Render shadowmap
   glBindFramebuffer(GL_FRAMEBUFFER, shadows.fbo);
   glViewport(0, 0, shadows.width, shadows.height);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glBindVertexArray(shadows.vao);

   glBindBuffer(GL_UNIFORM_BUFFER, cameraUbo);
   glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraUbo), &shadows.shadowCamera);

   glUseProgram(shadows.shaderProgram);

   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

   glBindBufferBase(GL_UNIFORM_BUFFER, cameraUboLocation, cameraUbo);

   drawScene(true);

   // Render Regular World
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   glViewport(0, 0, windowWidth, windowHeight);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glBindVertexArray(vao);

   glm::mat4 biasMatrix(
      0.5, 0.0, 0.0, 0.0,
      0.0, 0.5, 0.0, 0.0,
      0.0, 0.0, 0.5, 0.0,
      0.5, 0.5, 0.5, 1.0
   );

   glm::mat4 shadowMvp = shadows.shadowCamera.projMatrix * shadows.shadowCamera.viewMatrix;
   cameraData.shadowMatrix = biasMatrix * shadowMvp;
   glBindBuffer(GL_UNIFORM_BUFFER, cameraUbo);
   glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraUbo), &cameraData);

   glUseProgram(geometryProgram);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, shadows.shadowTexture2DMap);
   glUniform1i(shadowTextureLocation, 0);

   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

   glBindBufferBase(GL_UNIFORM_BUFFER, cameraUboLocation, cameraUbo);
   glBindBufferBase(GL_UNIFORM_BUFFER, sunUboLocation, sunUbo);

   drawScene(false);
}

void ShadowsApplication::drawScene(bool shadowPass)
{
   drawGround(shadowPass);

   drawCube(shadowPass, glm::vec3(0.0, 0.0, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
   drawCube(shadowPass, glm::vec3(5.0, 0.0, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
   drawCube(shadowPass, glm::vec3(0.0, 1.0, 5.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
   drawCube(shadowPass, glm::vec3(5.0, 1.0, 5.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
}

void ShadowsApplication::drawGround(bool shadowPass)
{
   glm::mat4 model(1.0);
   model = glm::scale(model, glm::vec3(20.0f, 0.01f, 20.0f));
   if (shadowPass)
   {
      glUniformMatrix4fv(shadows.modelMatrixLocation, 1, GL_FALSE, &(model[0][0]));
   }
   else
   {
      glUniformMatrix4fv(modelMatrixUboLocation, 1, GL_FALSE, &(model[0][0]));
      glUniform4f(objectColorUboLocation, 1.0f, 0.0f, 0.0f, 1.0f);
   }
   glDrawElementsBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)groundPlaneIboOffset, 0);
}

void ShadowsApplication::drawCube(bool shadowPass, const glm::vec3& position, const glm::vec4& color)
{
   glm::mat4 cubeModel(1.0);
   cubeModel = glm::translate(cubeModel, position);
   if (shadowPass)
   {
      glUniformMatrix4fv(shadows.modelMatrixLocation, 1, GL_FALSE, &(cubeModel[0][0]));
   }
   else
   {
      glUniformMatrix4fv(modelMatrixUboLocation, 1, GL_FALSE, &(cubeModel[0][0]));
      glUniform4fv(objectColorUboLocation, 1, &(color[0]));
   }
   glDrawElementsBaseVertex(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)cubeIboOffset, groundPlaneNumberOfVerts);
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
