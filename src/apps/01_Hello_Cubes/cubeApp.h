#pragma once

#include "app.h"
#include "core/camera.h"
#include "gfx/gfxDevice.h"

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

class CubeApplication : public Application
{
public:
   DECLARE_APPLICATION(CubeApplication);

   virtual void onWindowSizeUpdate(int width, int height) override;

   virtual void onInit() override;
   virtual void onDestroy() override;
   virtual void onUpdate(double dt) override;
   virtual void onRenderImGUI(double dt) override;

   void updateCamera(double dt);
   void updatePerspectiveMatrix();
   void initGL();
   void initUBOs();
   void destroyGL();
   void render(double dt);

private:
   Camera camera;
   CameraUbo cameraData;
   SunUbo sunData;

   GFXDevice* graphicsDevice;
   GFXCmdBuffer* cmdBuffer;

   int windowWidth;
   int windowHeight;

   StateBlockHandle depthStateHandle;
   StateBlockHandle rasterizerStateHandle;

   BufferHandle cubeVertexBufferHandle;
   BufferHandle cubeIndexBufferHandle;
   PipelineHandle cubePipelineHandle;
   
   BufferHandle cameraBufferHandle;
   BufferHandle sunBufferHandle;

   RenderPassHandle renderPassHandle;
   TextureHandle colorRenderPassAttachmentHandle;
   TextureHandle depthRenderPassAttachmentHandle;

   int cameraUboLocation;
   int sunUboLocation;
};
