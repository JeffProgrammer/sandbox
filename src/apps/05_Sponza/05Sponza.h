#pragma once

#include "app.h"
#include "core/camera.h"
#include "resources/modelObj.h"

struct CameraUbo
{
   glm::mat4 projMatrix;
   glm::mat4 viewMatrix;
};

struct SunUbo
{
   glm::vec4 sunDir;
   glm::vec4 sunColor;
   glm::vec4 ambientColor;
};

struct SceneMesh
{
   uint32_t diffuseTexture;
   //GLuint bumpTexture;
   //GLuint specularTexture;

   int count;
   int vboStartOffset;
   int iboIndex;
};

struct SceneGraph
{
   uint32_t sponzaVao;
   uint32_t sponzaVbo;
   uint32_t sponzaShader;
   uint32_t sponzaCameraUboLocation;
   uint32_t sponzaSunUboLocation;
   uint32_t sponzaTextureLocation;
   std::vector<SceneMesh> sponzaMeshes;
};

class A06Application : public Application
{
public:
   DECLARE_APPLICATION(A06Application);

   virtual void onWindowSizeUpdate(int width, int height) override;

   virtual void onInit() override;
   virtual void onDestroy() override;
   virtual void onUpdate(double dt) override;
   virtual void onRenderImGUI(double dt) override;

   void updateCamera(double dt);
   void updatePerspectiveMatrix();
   void initGL();
   void initScene();
   void initUBOs();
   void initShader();
   void destroyGL();
   void render(double dt);

private:
   tinygltf::Model sponza;
   SceneGraph scene;

   Camera camera;

   CameraUbo cameraData;
   SunUbo sunData;

   int windowWidth;
   int windowHeight;

   uint32_t cameraUbo;
   uint32_t sunUbo;

   uint32_t cameraUboLocation;
   uint32_t sunUboLocation;
};
