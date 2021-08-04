#include "gfx/Metal/gfxMetalDevice.h"

BufferHandle GFXMetalDevice::createBuffer(const GFXBufferDesc& desc)
{
   return 0;
}

void GFXMetalDevice::deleteBuffer(BufferHandle handle)
{
   
}

PipelineHandle GFXMetalDevice::createPipeline(const GFXPipelineDesc& desc)
{
   return 0;
}

void GFXMetalDevice::deletePipeline(PipelineHandle handle)
{
   
}

StateBlockHandle GFXMetalDevice::createRasterizerState(const GFXRasterizerStateDesc& desc)
{
   return 0;
}

StateBlockHandle GFXMetalDevice::createDepthStencilState(const GFXDepthStencilStateDesc& desc)
{
   return 0;
}

StateBlockHandle GFXMetalDevice::createBlendState(const GFXBlendStateDesc& desc)
{
   return 0;
}

void GFXMetalDevice::deleteStateBlock(StateBlockHandle handle)
{
   
}

void* GFXMetalDevice::mapBuffer(BufferHandle handle)
{
   return nullptr;
}

void GFXMetalDevice::unmapBuffer(BufferHandle handle)
{
   
}

void GFXMetalDevice::executeCmdBuffers(const GFXCmdBuffer** cmdBuffers, int count)
{
   
}
