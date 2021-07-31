#pragma once

#include "gfx/gfxTypes.h"

class GFXDevice
{
public:
   virtual BufferHandle createBuffer(const GFXBufferDesc& desc) = 0;
   virtual void deleteBuffer(BufferHandle handle) = 0;

   virtual PipelineHandle createPipeline(const GFXPipelineDesc& desc) = 0;
   virtual void deletePipeline(PipelineHandle handle) = 0;

   virtual void* mapBuffer(BufferHandle handle) = 0;
   virtual void unmapBuffer(BufferHandle handle) = 0;
};