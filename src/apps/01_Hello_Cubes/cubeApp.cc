#include <stdio.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include "apps/01_Hello_Cubes/cubeApp.h"
#include "core/cube.h"
#include "gl/shader.h"
#include "gfx/gfxCmdBuffer.h"
#include "gfx/OpenGL/gfxGLDevice.h"

IMPLEMENT_APPLICATION(CubeApplication);

const float VIEW_DISTANCE = 500.0f;

void CubeApplication::onInit()
{
   camera.setPosition(glm::vec3(1.8f, 2.0f, 1.85f));
   camera.setYawPitch(-2.34f, -0.58f);
   updatePerspectiveMatrix();

   getWindowSize(windowWidth, windowHeight);
   toggleCursorLock();
   setWindowTitle("Cube App");

   sunData.sunDir = glm::vec4(0.32f, 0.75f, 0.54f, 0.0f);
   sunData.sunColor = glm::vec4(1.4f, 1.2f, 0.4f, 0.0f);
   sunData.ambientColor = glm::vec4(0.3f, 0.3f, 0.4f, 0.0f);
   
   initGL();
}

void CubeApplication::onDestroy()
{
   destroyGL();
}

void CubeApplication::onUpdate(double dt)
{
   if (isKeyPressed(Key::ESCAPE))
      toggleCursorLock();
   
   updateCamera(dt);
   render(dt);
}

void CubeApplication::onWindowSizeUpdate(int width, int height)
{
   windowWidth = width;
   windowHeight = height;
}

void CubeApplication::updateCamera(double dt)
{
   Move move;
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

void CubeApplication::updatePerspectiveMatrix()
{
   camera.setProjectionMatrix(glm::perspective((float)glm::radians(110.0), getAspectRatio(), 0.01f, VIEW_DISTANCE));

   camera.getMatrices(cameraData.projMatrix, cameraData.viewMatrix);
}

void CubeApplication::initGL()
{
   graphicsDevice = new GFXGLDevice();
   cmdBuffer = new GFXCmdBuffer();

   {
      GFXRasterizerStateDesc rasterState;
      rasterState.cullMode = GFXCullMode::CULL_FRONT;
      rasterState.windingMode = GFXWindingMode::CLOCKWISE;
      rasterState.fillMode = GFXFillMode::SOLID;

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

      cubeVertexBufferHandle = graphicsDevice->createBuffer(cubeBuffer);
   }

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

      char* vertShader = readShaderFile("apps/01_Hello_Cubes/shaders/cube.vert");
      char* fragShader = readShaderFile("apps/01_Hello_Cubes/shaders/cube.frag");

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

      cubePipelineHandle = graphicsDevice->createPipeline(pipelineDesc);
   }

   {
      GFXBufferDesc cubeIndexBuffer;
      cubeIndexBuffer.type = GFXBufferType::INDEX_BUFFER;
      cubeIndexBuffer.usage = GFXBufferUsageEnum::STATIC_GPU_ONLY;
      cubeIndexBuffer.sizeInBytes = sizeof(cubeIndices);
      cubeIndexBuffer.data = (void*)cubeIndices;

      cubeIndexBufferHandle = graphicsDevice->createBuffer(cubeIndexBuffer);
   }

   initUBOs();

   cameraUboLocation = 0;
   sunUboLocation = 1;
}

void CubeApplication::initUBOs()
{
   {
      GFXBufferDesc cameraBufferDesc;
      cameraBufferDesc.type = GFXBufferType::CONSTANT_BUFFER;
      cameraBufferDesc.usage = GFXBufferUsageEnum::DYNAMIC_CPU_TO_GPU;
      cameraBufferDesc.sizeInBytes = sizeof(CameraUbo);
      cameraBufferDesc.data = (void*)NULL;

      cameraBufferHandle = graphicsDevice->createBuffer(cameraBufferDesc);
   }

   {
      GFXBufferDesc sunBufferDesc;
      sunBufferDesc.type = GFXBufferType::CONSTANT_BUFFER;
      sunBufferDesc.usage = GFXBufferUsageEnum::STATIC_GPU_ONLY;
      sunBufferDesc.sizeInBytes = sizeof(SunUbo);
      sunBufferDesc.data = (void*)&sunData;

      sunBufferHandle = graphicsDevice->createBuffer(sunBufferDesc);
   }
}

void CubeApplication::destroyGL()
{
   graphicsDevice->deleteBuffer(cubeVertexBufferHandle);
   graphicsDevice->deleteBuffer(cubeIndexBufferHandle);
   graphicsDevice->deleteBuffer(cameraBufferHandle);
   graphicsDevice->deleteBuffer(sunBufferHandle);
   graphicsDevice->deletePipeline(cubePipelineHandle);

   delete cmdBuffer;
   delete graphicsDevice;
}

void CubeApplication::render(double dt)
{
   char* pData = (char*)graphicsDevice->mapBuffer(cameraBufferHandle, 0, sizeof(CameraUbo));
   memcpy(pData, &cameraData, sizeof(CameraUbo));
   graphicsDevice->unmapBuffer(cameraBufferHandle);

   cmdBuffer->begin();

   cmdBuffer->setViewport(0, 0, windowWidth, windowHeight);
   cmdBuffer->setScissor(0, 0, windowWidth, windowHeight);

   // TODO, add framebuffer binding and clearing...concept of a 'render pass'...
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glClearColor(0.0, 0.0, 0.0, 1.0);

   cmdBuffer->setRasterizerState(rasterizerStateHandle);
   cmdBuffer->setDepthStencilState(depthStateHandle);

   cmdBuffer->bindPipeline(cubePipelineHandle);
   cmdBuffer->bindConstantBuffer(0, cameraBufferHandle, 0, sizeof(CameraUbo));
   cmdBuffer->bindConstantBuffer(1, sunBufferHandle, 0, sizeof(SunUbo));
   cmdBuffer->bindVertexBuffer(0, cubeVertexBufferHandle, sizeof(float) * 6, 0);
   cmdBuffer->bindIndexBuffer(cubeIndexBufferHandle, GFXIndexBufferType::BITS_16, 0);

   cmdBuffer->drawIndexedPrimitives(36, 0);

   cmdBuffer->end();

   const GFXCmdBuffer* buffer[1];
   buffer[0] = cmdBuffer;
   graphicsDevice->executeCmdBuffers(buffer, 1);
}

void CubeApplication::onRenderImGUI(double dt)
{
   ImGui::NewFrame();
   ImGui::Begin("Debug Information");
   ImGui::SetWindowSize(ImVec2(400, 120));
   ImGui::Text("Frame Rate: %.1f FPS", ImGui::GetIO().Framerate);
   ImGui::Separator();
   ImGui::Text("GL Renderer: %s", glGetString(GL_RENDERER));
   ImGui::Text("GL Vendor: %s", glGetString(GL_VENDOR));
   ImGui::Text("GL Version: %s", glGetString(GL_VERSION));
   ImGui::End();
   ImGui::Render();
}
