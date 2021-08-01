#include <stdio.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/color_space.hpp>
#include <imgui.h>
#include <tiny_gltf.h>
#include "apps/05_Sponza/05Sponza.h"
#include "core/cube.h"
#include "gl/shader.h"
#include "resources/modelObj.h"

IMPLEMENT_APPLICATION(A06Application);

const float VIEW_DISTANCE = 500.0f;

void A06Application::onInit()
{
   camera.setPosition(glm::vec3(9.0f, 25.0f, 9.0f));
   camera.setYawPitch(-2.34f, -1.20f);
   updatePerspectiveMatrix();

   getWindowSize(windowWidth, windowHeight);
   setWindowTitle("06 Application");

   sunData.sunDir = glm::vec4(0.32f, 0.75f, 0.54f, 0.0f);
   sunData.sunColor = glm::vec4(1.4f, 1.2f, 0.4f, 0.0f);
   sunData.ambientColor = glm::vec4(0.3f, 0.3f, 0.4f, 0.0f);

   if (!loadModel("../thirdparty/models/Sponza/glTF/Sponza.gltf", &sponza))
      abort();

   initScene();
   initGL();
}

void A06Application::onDestroy()
{
   destroyGL();
}

void A06Application::onUpdate(double dt)
{
   updateCamera(dt);
   render(dt);
}

void A06Application::onWindowSizeUpdate(int width, int height)
{
   windowWidth = width;
   windowHeight = height;
}

void A06Application::updateCamera(double dt)
{
   Move move = {};
   camera.update(dt, move);
   updatePerspectiveMatrix();
}

void A06Application::updatePerspectiveMatrix()
{
   camera.setProjectionMatrix(glm::perspective((float)glm::radians(110.0), getAspectRatio(), 0.01f, VIEW_DISTANCE));

   camera.getMatrices(cameraData.projMatrix, cameraData.viewMatrix);
}

void A06Application::initGL()
{
   initShader();
   initUBOs();
}

void A06Application::initScene()
{
   // Buffer holds the vertex and index data
   const tinygltf::Buffer& buffer = sponza.buffers[0];
   
   glGenVertexArrays(1, &scene.sponzaVao);
   glBindVertexArray(scene.sponzaVao);

   glGenBuffers(1, &scene.sponzaVbo);
   glBindBuffer(GL_ARRAY_BUFFER, scene.sponzaVbo);
   //glBufferData(GL_ARRAY_BUFFER, buffer.data.size(), buffer.data.data(), GL_STATIC_DRAW);

   
   /*glGenVertexArrays(1, &scene.sponzaVao);
   glBindVertexArray(scene.sponzaVao);

   glGenBuffers(1, &scene.sponzaVbo);
   glBindBuffer(GL_ARRAY_BUFFER, scene.sponzaVbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(ModelVertex) * sponza.totalModelVertCount, NULL, GL_STATIC_DRAW);

   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glEnableVertexAttribArray(2);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)offsetof(ModelVertex, pos));
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)offsetof(ModelVertex, normal));
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)offsetof(ModelVertex, uv));

   int startOffset = 0;
   for (const auto& pair : sponza.materialShapes)
   {
      int materialId = pair.first;
      const std::vector<ModelVertex>& verts = pair.second.verts;
      const Material& material = sponza.materials[materialId];

      SceneMesh mesh;

      glBufferSubData(GL_ARRAY_BUFFER, startOffset * sizeof(ModelVertex), sizeof(ModelVertex) * verts.size(), verts.data());

      mesh.count = verts.size();
      mesh.vboStartOffset = startOffset;
      startOffset += mesh.count;

      // Generate texture
      glGenTextures(1, &mesh.diffuseTexture);
      glBindTexture(GL_TEXTURE_2D, mesh.diffuseTexture);
      glTexImage2D(
         GL_TEXTURE_2D, 
         0, 
         material.diffuseTexture.components == 4 ? GL_RGBA : GL_RGB, 
         material.diffuseTexture.width, 
         material.diffuseTexture.height,
         0,
         material.diffuseTexture.components == 4 ? GL_RGBA : GL_RGB, 
         GL_UNSIGNED_BYTE, 
         material.diffuseTexture.pixels
      );
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glGenerateMipmap(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, 0);

      scene.sponzaMeshes.push_back(mesh);
   }
    */
}

void A06Application::initUBOs()
{
   glGenBuffers(1, &cameraUbo);
   glBindBuffer(GL_UNIFORM_BUFFER, cameraUbo);
   glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraUbo), NULL, GL_DYNAMIC_DRAW);

   glGenBuffers(1, &sunUbo);
   glBindBuffer(GL_UNIFORM_BUFFER, sunUbo);
   glBufferData(GL_UNIFORM_BUFFER, sizeof(SunUbo), &sunData, GL_STATIC_DRAW);
}

void A06Application::initShader()
{
   char* vertShader = readShaderFile("apps/06/shaders/model.vert");
   char* fragShader = readShaderFile("apps/06/shaders/model.frag");

   scene.sponzaShader = createVertexAndFragmentShaderProgram(vertShader, fragShader);

   free(vertShader);
   free(fragShader);

   scene.sponzaCameraUboLocation = glGetUniformBlockIndex(scene.sponzaShader, "CameraBuffer");
   scene.sponzaSunUboLocation = glGetUniformBlockIndex(scene.sponzaShader, "SunBuffer");
   scene.sponzaTextureLocation = glGetUniformLocation(scene.sponzaShader, "diffuseTexture");

   cameraUboLocation = 0;
   sunUboLocation = 1;

   glUniformBlockBinding(scene.sponzaShader, scene.sponzaCameraUboLocation, cameraUboLocation);
   glUniformBlockBinding(scene.sponzaShader, scene.sponzaSunUboLocation, sunUboLocation);
}

void A06Application::destroyGL()
{
   glUseProgram(0);
   glDeleteProgram(scene.sponzaShader);

   for (const auto& mesh : scene.sponzaMeshes) 
   {
      glDeleteTextures(1, &mesh.diffuseTexture);
   }

   GLuint deleteBuffers[2] = { scene.sponzaVbo, cameraUbo };
   glDeleteBuffers(2, deleteBuffers);

   glBindVertexArray(0);
   glDeleteVertexArrays(1, &scene.sponzaVao);
}

void A06Application::render(double dt)
{
   glViewport(0, 0, windowWidth, windowHeight);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glClearColor(0.0, 0.0, 0.0, 1.0);

   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);
   glFrontFace(GL_CCW);

   glUseProgram(scene.sponzaShader);

   glActiveTexture(GL_TEXTURE0);

   glBindBuffer(GL_UNIFORM_BUFFER, cameraUbo);
   glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraUbo), &cameraData);

   glBindVertexArray(scene.sponzaVao);
   glBindBuffer(GL_ARRAY_BUFFER, scene.sponzaVbo);

   glBindBufferBase(GL_UNIFORM_BUFFER, cameraUboLocation, cameraUbo);
   glBindBufferBase(GL_UNIFORM_BUFFER, sunUboLocation, sunUbo);
   glUniform1i(scene.sponzaTextureLocation, 0);

   for (const SceneMesh& mesh : scene.sponzaMeshes)
   {
      glBindTexture(GL_TEXTURE_2D, mesh.diffuseTexture);
      glDrawArrays(GL_TRIANGLES, mesh.vboStartOffset, mesh.count);
   }
}

void A06Application::onRenderImGUI(double dt)
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
