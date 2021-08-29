#pragma once

#include "app.h"
#include "core/camera.h"
#include "gfx/gfxDevice.h"

struct CameraUbo
{
   glm::mat4 projMatrix;
   glm::mat4 viewMatrix;
};

#define CUBE_COUNT 512
#define LIGHT_COUNT 1024

struct CubeUbo
{
   glm::mat4 modelMatrix[CUBE_COUNT];
};

struct LightUbo
{
   struct Light
   {
      glm::vec4 position;
      glm::vec4 attenuation;
      glm::vec4 color;
   };
   
   Light lights[LIGHT_COUNT];
   int lightCount;
   int pad[3];
};

class ForwardRenderingApplication : public Application
{
public:
   DECLARE_APPLICATION(ForwardRenderingApplication);

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
   void createCubeData();
   void createLights(int count);

private:
   Camera camera;

   CameraUbo cameraData;
   CubeUbo cubeData;
   LightUbo lightData;

   int windowWidth;
   int windowHeight;

   GFXDevice* graphicsDevice;
   GFXCmdBuffer* cmdBuffer;

   StateBlockHandle depthStateHandle;
   StateBlockHandle rasterizerStateHandle;

   RenderPassHandle renderPassHandle;
   TextureHandle colorRenderPassAttachmentHandle;
   TextureHandle depthRenderPassAttachmentHandle;

   PipelineHandle pipelineHandle;

   BufferHandle cameraBufferHandle;
   BufferHandle lightBufferHandle;
   BufferHandle cubeBufferHandle;
   BufferHandle vertexBufferHandle;
   BufferHandle indexBufferHandle;
};