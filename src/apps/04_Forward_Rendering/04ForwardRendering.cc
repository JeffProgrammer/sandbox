#include <stdio.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/color_space.hpp>
#include <imgui.h>
#include "apps/04_Forward_Rendering/04ForwardRendering.h"
#include "core/cube.h"
#include "gfx/gfxCmdBuffer.h"
#include "gfx/OpenGL/gfxGLDevice.h"

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
   graphicsDevice = new GFXGLDevice();
   cmdBuffer = new GFXCmdBuffer();

   {
      GFXTextureStateDesc colorTexDesc = {};
      colorTexDesc.height = windowHeight;
      colorTexDesc.width = windowWidth;
      colorTexDesc.type = GFXTextureType::TEXTURE_2D;
      colorTexDesc.levels = 1;
      colorTexDesc.internalFormat = GFXTextureInternalFormat::RGBA8;

      GFXTextureStateDesc depthTexDesc = {};
      depthTexDesc.height = windowHeight;
      depthTexDesc.width = windowWidth;
      depthTexDesc.type = GFXTextureType::TEXTURE_2D;
      depthTexDesc.levels = 1;
      depthTexDesc.internalFormat = GFXTextureInternalFormat::DEPTH_16;

      colorRenderPassAttachmentHandle = graphicsDevice->createTexture(colorTexDesc);
      depthRenderPassAttachmentHandle = graphicsDevice->createTexture(depthTexDesc);

      GFXColorRenderPassAttachment colorAttach = {};
      colorAttach.clearColor[0] = 0.0f;
      colorAttach.clearColor[1] = 0.0f;
      colorAttach.clearColor[2] = 0.0f;
      colorAttach.clearColor[3] = 1.0f;
      colorAttach.loadAction = GFXLoadAttachmentAction::CLEAR;
      colorAttach.texture = colorRenderPassAttachmentHandle;

      GFXDepthRenderPassAttachment depthAttach = {};
      depthAttach.clearDepth = 1.0;
      depthAttach.loadAction = GFXLoadAttachmentAction::CLEAR;
      depthAttach.texture = depthRenderPassAttachmentHandle;

      GFXRenderPassDesc renderPassState;
      renderPassState.colorAttachmentCount = 1;
      renderPassState.colorAttachments[0] = std::move(colorAttach);
      renderPassState.depthAttachmentEnabled = true;
      renderPassState.depthAttachment = std::move(depthAttach);

      renderPassHandle = graphicsDevice->createRenderPass(renderPassState);
   }

   {
      GFXRasterizerStateDesc rasterState;
      rasterState.cullMode = GFXCullMode::CULL_FRONT;
      rasterState.windingMode = GFXWindingMode::CLOCKWISE;
      rasterState.fillMode = GFXFillMode::SOLID;
      rasterState.enableDynamicPointSize = false;

      rasterizerStateHandle = graphicsDevice->createRasterizerState(rasterState);
   }

   {
      GFXDepthStencilStateDesc depthState;
      depthState.enableDepthTest = true;
      depthState.enableDepthWrite = true;
      depthState.depthCompareFunc = GFXCompareFunc::LESS;

      depthStateHandle = graphicsDevice->createDepthStencilState(depthState);
   }

   {
      GFXBufferDesc cubeBuffer;
      cubeBuffer.type = GFXBufferType::VERTEX_BUFFER;
      cubeBuffer.usage = GFXBufferUsageEnum::STATIC_GPU_ONLY;
      cubeBuffer.sizeInBytes = sizeof(cubeVertsBuffer);
      cubeBuffer.data = (void*)cubeVertsBuffer;

      vertexBufferHandle = graphicsDevice->createBuffer(cubeBuffer);
   }

   {
      GFXBufferDesc cubeBuffer;
      cubeBuffer.type = GFXBufferType::INDEX_BUFFER;
      cubeBuffer.usage = GFXBufferUsageEnum::STATIC_GPU_ONLY;
      cubeBuffer.sizeInBytes = sizeof(cubeIndices);
      cubeBuffer.data = (void*)cubeIndices;

      indexBufferHandle = graphicsDevice->createBuffer(cubeBuffer);
   }

   initShader();
   initUBOs();
   createLights(LIGHT_COUNT);
}

void ForwardRenderingApplication::initUBOs()
{
   {
      GFXBufferDesc cameraBufferDesc;
      cameraBufferDesc.type = GFXBufferType::CONSTANT_BUFFER;
      cameraBufferDesc.usage = GFXBufferUsageEnum::DYNAMIC_CPU_TO_GPU;
      cameraBufferDesc.sizeInBytes = sizeof(CameraUbo);
      cameraBufferDesc.data = nullptr;

      cameraBufferHandle = graphicsDevice->createBuffer(cameraBufferDesc);
   }

   {
      GFXBufferDesc lightBufferDesc;
      lightBufferDesc.type = GFXBufferType::CONSTANT_BUFFER;
      lightBufferDesc.usage = GFXBufferUsageEnum::DYNAMIC_CPU_TO_GPU;
      lightBufferDesc.sizeInBytes = sizeof(LightUbo);
      lightBufferDesc.data = nullptr;

      lightBufferHandle = graphicsDevice->createBuffer(lightBufferDesc);
   }

   {
      GFXBufferDesc cubeBufferDesc;
      cubeBufferDesc.type = GFXBufferType::CONSTANT_BUFFER;
      cubeBufferDesc.usage = GFXBufferUsageEnum::STATIC_GPU_ONLY;
      cubeBufferDesc.sizeInBytes = sizeof(CubeUbo);
      cubeBufferDesc.data = &cubeData;

      cubeBufferHandle = graphicsDevice->createBuffer(cubeBufferDesc);
   }
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
      const glm::mat4 &modelMatrix = cubeData.modelMatrix[i];
      glm::vec4 pos = modelMatrix[3];
      pos.y = 10.0f;
      pos.w = 16.0f;
      lightData.lights[i].position = pos;
      lightData.lights[i].color = glm::vec4(glm::rgbColor(glm::vec3(((float)rand()/(float)RAND_MAX)*360.0f, 1.0f, 0.2f)), 1.0);
      lightData.lights[i].attenuation = glm::vec4(1.0f, 0.7f, 5.8f, 0.0f);
   }

   char* pData = (char*)graphicsDevice->mapBuffer(lightBufferHandle, 0, sizeof(LightUbo));
   memcpy(pData, &lightData, sizeof(LightUbo));
   graphicsDevice->unmapBuffer(lightBufferHandle);
}

void ForwardRenderingApplication::initShader()
{
   GFXInputLayoutElementDesc inputLayoutDescs[2];
   inputLayoutDescs[0].slot = 0;
   inputLayoutDescs[0].count = 3;
   inputLayoutDescs[0].type = GFXInputLayoutFormat::FLOAT;
   inputLayoutDescs[0].divisor = GFXInputLayoutDivisor::PER_VERTEX;
   inputLayoutDescs[0].offset = 0;
   inputLayoutDescs[0].bufferBinding = 0;

   inputLayoutDescs[1].slot = 1;
   inputLayoutDescs[1].count = 3;
   inputLayoutDescs[1].type = GFXInputLayoutFormat::FLOAT;
   inputLayoutDescs[1].divisor = GFXInputLayoutDivisor::PER_VERTEX;
   inputLayoutDescs[1].offset = 12;
   inputLayoutDescs[1].bufferBinding = 0;

   GFXInputLayoutDesc inputLayout;
   inputLayout.count = 2;
   inputLayout.descs = inputLayoutDescs;

   char* vertShader = readShaderFile("apps/04_Forward_Rendering/shaders/cube.vert");
   char* fragShader = readShaderFile("apps/04_Forward_Rendering/shaders/cube.frag");

   GFXShaderDesc shaders[2];
   shaders[0].type = GFXShaderType::VERTEX;
   shaders[0].code = vertShader;
   shaders[0].codeLength = strlen(vertShader);

   shaders[1].type = GFXShaderType::FRAGMENT;
   shaders[1].code = fragShader;
   shaders[1].codeLength = strlen(fragShader);

   GFXPipelineDesc pipelineDesc;
   pipelineDesc.primitiveType = GFXPrimitiveType::TRIANGLE_LIST;
   pipelineDesc.inputLayout = std::move(inputLayout);
   pipelineDesc.shadersStages = shaders;
   pipelineDesc.shaderStageCount = 2;

   pipelineHandle = graphicsDevice->createPipeline(pipelineDesc);
}

void ForwardRenderingApplication::destroyGL()
{
   graphicsDevice->deleteStateBlock(depthStateHandle);
   graphicsDevice->deleteStateBlock(rasterizerStateHandle);

   graphicsDevice->deleteBuffer(cameraBufferHandle);
   graphicsDevice->deleteBuffer(lightBufferHandle);
   graphicsDevice->deleteBuffer(cubeBufferHandle);
   graphicsDevice->deleteBuffer(vertexBufferHandle);
   graphicsDevice->deleteBuffer(indexBufferHandle);
   graphicsDevice->deletePipeline(pipelineHandle);

   graphicsDevice->deleteTexture(colorRenderPassAttachmentHandle);
   graphicsDevice->deleteTexture(depthRenderPassAttachmentHandle);
   graphicsDevice->deleteRenderPass(renderPassHandle);

   delete cmdBuffer;
   delete graphicsDevice;
}

void ForwardRenderingApplication::render(double dt)
{
   char* pData = (char*)graphicsDevice->mapBuffer(cameraBufferHandle, 0, sizeof(CameraUbo));
   memcpy(pData, &cameraData, sizeof(CameraUbo));
   graphicsDevice->unmapBuffer(cameraBufferHandle);

   cmdBuffer->begin();

   cmdBuffer->bindRenderPass(renderPassHandle);
   cmdBuffer->setViewport(0, 0, windowWidth, windowHeight);
   cmdBuffer->setScissor(0, 0, windowWidth, windowHeight);

   cmdBuffer->setRasterizerState(rasterizerStateHandle);
   cmdBuffer->setDepthStencilState(depthStateHandle);

   cmdBuffer->bindPipeline(pipelineHandle);
   cmdBuffer->bindConstantBuffer(0, cameraBufferHandle, 0, sizeof(CameraUbo));
   cmdBuffer->bindConstantBuffer(1, lightBufferHandle, 0, sizeof(LightUbo));
   cmdBuffer->bindConstantBuffer(2, cubeBufferHandle, 0, sizeof(CubeUbo));

   cmdBuffer->bindVertexBuffer(0, vertexBufferHandle, sizeof(float) * 6, 0);
   cmdBuffer->bindIndexBuffer(indexBufferHandle, GFXIndexBufferType::BITS_16, 0);
   
   cmdBuffer->drawIndexedPrimitivesInstanced(36, 0, CUBE_COUNT);

   cmdBuffer->end();

   const GFXCmdBuffer* buffer[1];
   buffer[0] = cmdBuffer;
   graphicsDevice->executeCmdBuffers(buffer, 1);

   // and now we present our render pass
   graphicsDevice->present(renderPassHandle, windowWidth, windowHeight);
}

void ForwardRenderingApplication::onRenderImGUI(double dt)
{
   ImGui::NewFrame();
   ImGui::Begin("Debug Information & Options");
   ImGui::Text("Frame Rate: %.1f FPS", ImGui::GetIO().Framerate);

   ImGui::Separator();
   ImGui::Text("%s Driver Information:", graphicsDevice->getApiString());
   ImGui::Text("   Renderer: %s", graphicsDevice->getGFXDeviceRendererDesc());
   ImGui::Text("   Vendor: %s", graphicsDevice->getGFXDeviceVendorDesc());
   ImGui::Text("   Version: %s", graphicsDevice->getApiVersionString());

   ImGui::Separator();
   if (ImGui::InputInt("Light Count", &lightData.lightCount))
   {
      createLights(lightData.lightCount);
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
         glm::vec3 pos = glm::vec3((float)x * 4, 0, (float)z * 4);

         glm::mat4 mat = glm::mat4(1);
         mat = glm::translate(mat, pos);
         mat = glm::scale(mat, glm::vec3(2));
         cubeData.modelMatrix[idx++] = mat;
      }
   }
}