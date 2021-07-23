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
   GLuint diffuseTexture;
   //GLuint bumpTexture;
   //GLuint specularTexture;

   int count;
   int vboStartOffset;
};

struct SceneGraph
{
   GLuint sponzaVao;
   GLuint sponzaVbo;
   GLuint sponzaShader;
   GLuint sponzaCameraUboLocation;
   GLuint sponzaSunUboLocation;
   GLuint sponzaTextureLocation;
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
   Model sponza;
   SceneGraph scene;

   Camera camera;

   CameraUbo cameraData;
   SunUbo sunData;

   int windowWidth;
   int windowHeight;

   GLuint cameraUbo;
   GLuint sunUbo;

   GLuint cameraUboLocation;
   GLuint sunUboLocation;
};