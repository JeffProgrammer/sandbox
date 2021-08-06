#pragma once

#include "gfx/gfxTypes.h"

class GFXCmdBuffer;

class GFXDevice
{
public:
   virtual BufferHandle createBuffer(const GFXBufferDesc& desc) = 0;
   virtual void deleteBuffer(BufferHandle handle) = 0;

   virtual PipelineHandle createPipeline(const GFXPipelineDesc& desc) = 0;
   virtual void deletePipeline(PipelineHandle handle) = 0;

   virtual StateBlockHandle createRasterizerState(const GFXRasterizerStateDesc& desc) = 0;
   virtual StateBlockHandle createDepthStencilState(const GFXDepthStencilStateDesc& desc) = 0;
   virtual StateBlockHandle createBlendState(const GFXBlendStateDesc& desc) = 0;
   virtual void deleteStateBlock(StateBlockHandle handle) = 0;

   virtual void* mapBuffer(BufferHandle handle, uint32_t offset, uint32_t size) = 0;
   virtual void unmapBuffer(BufferHandle handle) = 0;

   virtual void executeCmdBuffers(const GFXCmdBuffer** cmdBuffers, int count) = 0;
};