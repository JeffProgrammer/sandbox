
#pragma once

#include "app.h"
#include "core/camera.h"

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

class ShadowsApplication : public Application
{
public:
   DECLARE_APPLICATION(ShadowsApplication);

   virtual void onWindowSizeUpdate(int width, int height) override;

   virtual void onInit() override;
   virtual void onDestroy() override;
   virtual void onUpdate(double dt) override;
   virtual void onRenderImGUI(double dt) override;

   void updateCamera(double dt);
   void updatePerspectiveMatrix();
   void initGL();
   void initUBOs();
   void initShader();
   void destroyGL();
   void render(double dt);

private:
   Camera camera;

   CameraUbo cameraData;
   SunUbo sunData;

   int windowWidth;
   int windowHeight;

   GLuint geometryProgram;

   GLuint groundVbo;
   GLuint groundIbo;
   GLuint vao;
   GLuint cameraUbo;
   GLuint sunUbo;

   GLuint uniformCameraLocationBlock;
   GLuint uniformSunLocationBlock;

   GLuint cameraUboLocation;
   GLuint sunUboLocation;
   GLuint objectColorUboLocation;
   GLuint modelMatrixUboLocation;
};
