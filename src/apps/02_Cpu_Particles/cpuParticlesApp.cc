#include <stdio.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <imgui.h>
#include "apps/02_Cpu_Particles/cpuParticlesApp.h"
#include "gfx/gfxCmdBuffer.h"
#include "gfx/OpenGL/gfxGLDevice.h"

IMPLEMENT_APPLICATION(CpuParticlesApp);

const float VIEW_DISTANCE = 500.0f;

void CpuParticlesApp::onInit()
{
   camera.setPosition(glm::vec3(1.8f, 2.0f, 1.85f));
   camera.setYawPitch(-2.34f, -0.58f);
   updatePerspectiveMatrix();

   getWindowSize(windowWidth, windowHeight);
   setWindowTitle("Cpu Particle App");

   vsync = false;
   freeze = false;

   initParticles();

   initGL();
}

void CpuParticlesApp::onDestroy()
{
   destroyGL();
}

void CpuParticlesApp::onUpdate(double dt)
{
   updateCamera(dt);

   simulateParticles(dt);
   copyParticlesToGLBuffer();

   render(dt);
}

void CpuParticlesApp::onWindowSizeUpdate(int width, int height)
{
   windowWidth = width;
   windowHeight = height;
}

void CpuParticlesApp::initParticles()
{
   for (int i = 0; i < PARTICLE_COUNT; ++i)
   {
      resetParticle(particles[i]);
   }
}

void CpuParticlesApp::resetParticle(Particle& p)
{
   p.pos = glm::vec3(0);
   p.velocity = glm::ballRand(3.0);
   p.lifeTime = 0.0;
   p.color = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
   p.lifeTimeMax = (PARTICLE_TIME_MAX_MS - 1000) - ((rand() % 10 + 1) * 100);
}

void CpuParticlesApp::simulateParticles(double dt)
{
   if (freeze)
      return;

   float deltaSeconds = (float)(dt / 1000.0f);

   for (int i = 0; i < PARTICLE_COUNT; ++i)
   {
      Particle& p = particles[i];
      p.pos += p.velocity * deltaSeconds;
      p.lifeTime += (float)dt;

      if (p.lifeTime >  p.lifeTimeMax)
         resetParticle(p);
   }
}

void CpuParticlesApp::copyParticlesToGLBuffer()
{
   for (int i = 0; i < PARTICLE_COUNT; ++i)
   {
      glParticles[i].pos = particles[i].pos;
      glParticles[i].color = particles[i].color;
   }
}

void CpuParticlesApp::updateCamera(double dt)
{
   Move move = {};
   camera.update(dt, move);
   updatePerspectiveMatrix();
}

void CpuParticlesApp::updatePerspectiveMatrix()
{
   camera.setProjectionMatrix(glm::perspective((float)glm::radians(110.0), getAspectRatio(), 0.01f, VIEW_DISTANCE));

   camera.getMatrices(cameraData.projMatrix, cameraData.viewMatrix);
}

void CpuParticlesApp::initGL()
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
      rasterState.enableDynamicPointSize = true;

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
      cubeBuffer.usage = GFXBufferUsageEnum::DYNAMIC_CPU_TO_GPU;
      cubeBuffer.sizeInBytes = sizeof(glParticles);
      cubeBuffer.data = nullptr;

      particleBufferHandle = graphicsDevice->createBuffer(cubeBuffer);
   }

   initShader();
   initUBOs();
}

void CpuParticlesApp::initUBOs()
{
   GFXBufferDesc cameraBufferDesc;
   cameraBufferDesc.type = GFXBufferType::CONSTANT_BUFFER;
   cameraBufferDesc.usage = GFXBufferUsageEnum::DYNAMIC_CPU_TO_GPU;
   cameraBufferDesc.sizeInBytes = sizeof(CameraUbo);
   cameraBufferDesc.data = nullptr;

   cameraBufferHandle = graphicsDevice->createBuffer(cameraBufferDesc);
}

void CpuParticlesApp::initShader()
{
   GFXInputLayoutElementDesc inputLayoutDescs[2];
   inputLayoutDescs[0].slot = 0;
   inputLayoutDescs[0].count = 3;
   inputLayoutDescs[0].type = GFXInputLayoutFormat::FLOAT;
   inputLayoutDescs[0].divisor = GFXInputLayoutDivisor::PER_VERTEX;
   inputLayoutDescs[0].offset = 0;
   inputLayoutDescs[0].bufferBinding = 0;

   inputLayoutDescs[1].slot = 1;
   inputLayoutDescs[1].count = 4;
   inputLayoutDescs[1].type = GFXInputLayoutFormat::FLOAT;
   inputLayoutDescs[1].divisor = GFXInputLayoutDivisor::PER_VERTEX;
   inputLayoutDescs[1].offset = 12;
   inputLayoutDescs[1].bufferBinding = 0;

   GFXInputLayoutDesc inputLayout;
   inputLayout.count = 2;
   inputLayout.descs = inputLayoutDescs;

   char* vertShader = readShaderFile("apps/02_Cpu_Particles/shaders/particles.vert");
   char* fragShader = readShaderFile("apps/02_Cpu_Particles/shaders/particles.frag");

   GFXShaderDesc shaders[2];
   shaders[0].type = GFXShaderType::VERTEX;
   shaders[0].code = vertShader;
   shaders[0].codeLength = strlen(vertShader);

   shaders[1].type = GFXShaderType::FRAGMENT;
   shaders[1].code = fragShader;
   shaders[1].codeLength = strlen(fragShader);

   GFXPipelineDesc pipelineDesc;
   pipelineDesc.primitiveType = GFXPrimitiveType::POINT_LIST;
   pipelineDesc.inputLayout = std::move(inputLayout);
   pipelineDesc.shadersStages = shaders;
   pipelineDesc.shaderStageCount = 2;

   pipelineHandle = graphicsDevice->createPipeline(pipelineDesc);
}

void CpuParticlesApp::destroyGL()
{
   graphicsDevice->deleteStateBlock(depthStateHandle);
   graphicsDevice->deleteStateBlock(rasterizerStateHandle);

   graphicsDevice->deleteBuffer(particleBufferHandle);
   graphicsDevice->deleteBuffer(cameraBufferHandle);
   graphicsDevice->deletePipeline(pipelineHandle);

   graphicsDevice->deleteTexture(colorRenderPassAttachmentHandle);
   graphicsDevice->deleteTexture(depthRenderPassAttachmentHandle);
   graphicsDevice->deleteRenderPass(renderPassHandle);

   delete cmdBuffer;
   delete graphicsDevice;
}

void CpuParticlesApp::render(double dt)
{
   char* pData = (char*)graphicsDevice->mapBuffer(cameraBufferHandle, 0, sizeof(CameraUbo));
   memcpy(pData, &cameraData, sizeof(CameraUbo));
   graphicsDevice->unmapBuffer(cameraBufferHandle);

   pData = (char*)graphicsDevice->mapBuffer(particleBufferHandle, 0, sizeof(glParticles));
   memcpy(pData, &glParticles[0], sizeof(glParticles));
   graphicsDevice->unmapBuffer(particleBufferHandle);

   cmdBuffer->begin();

   cmdBuffer->bindRenderPass(renderPassHandle);
   cmdBuffer->setViewport(0, 0, windowWidth, windowHeight);
   cmdBuffer->setScissor(0, 0, windowWidth, windowHeight);

   cmdBuffer->setRasterizerState(rasterizerStateHandle);
   cmdBuffer->setDepthStencilState(depthStateHandle);

   cmdBuffer->bindPipeline(pipelineHandle);
   cmdBuffer->bindConstantBuffer(0, cameraBufferHandle, 0, sizeof(CameraUbo));
   cmdBuffer->bindVertexBuffer(0, particleBufferHandle, sizeof(GLParticle), 0);

   cmdBuffer->drawPrimitives(0, PARTICLE_COUNT);

   cmdBuffer->end();

   const GFXCmdBuffer* buffer[1];
   buffer[0] = cmdBuffer;
   graphicsDevice->executeCmdBuffers(buffer, 1);

   // and now we present our render pass
   graphicsDevice->present(renderPassHandle, windowWidth, windowHeight);
}

void CpuParticlesApp::onRenderImGUI(double dt)
{
   ImGui::NewFrame();
   
   ImGui::Begin("Debug Information & Options");
   ImGui::SetWindowSize(ImVec2(700, 180));
   ImGui::Text("Frame Rate: %.1f FPS", ImGui::GetIO().Framerate);

#ifdef GFX_OPENGL
   ImGui::Separator();
   ImGui::Text("OpenGL Driver Information:");
   ImGui::Text("   Renderer: %s", glGetString(GL_RENDERER));
   ImGui::Text("   Vendor: %s", glGetString(GL_VENDOR));
   ImGui::Text("   Version: %s", glGetString(GL_VERSION));
#endif

   ImGui::Separator();

   if (ImGui::Checkbox("Enable Vsync", &vsync))
   {
      setVerticalSync(vsync);
   }

   ImGui::Checkbox("Freeze Simulation", &freeze);

   ImGui::End();

   ImGui::Render();
}
