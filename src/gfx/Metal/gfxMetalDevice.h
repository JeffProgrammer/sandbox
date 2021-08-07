#pragma once

#import <Metal/Metal.h>
#include "gfx/gfxDevice.h"

class GFXMetalDevice : public GFXDevice
{
   id<MTLDevice> mDevice;
   id<MTLCommandQueue> mCommandQueue;
   
   struct
   {
      bool hasUnifiedMemory;
   } mCaps;
public:
   GFXMetalDevice();
   
   virtual BufferHandle createBuffer(const GFXBufferDesc& desc) override;
   virtual void deleteBuffer(BufferHandle handle) override;

   virtual PipelineHandle createPipeline(const GFXPipelineDesc& desc) override;
   virtual void deletePipeline(PipelineHandle handle) override;

   virtual StateBlockHandle createRasterizerState(const GFXRasterizerStateDesc& desc) override;
   virtual StateBlockHandle createDepthStencilState(const GFXDepthStencilStateDesc& desc) override;
   virtual StateBlockHandle createBlendState(const GFXBlendStateDesc& desc) override;
   virtual void deleteStateBlock(StateBlockHandle handle) override;

   virtual void* mapBuffer(BufferHandle handle) override;
   virtual void unmapBuffer(BufferHandle handle) override;

   virtual void executeCmdBuffers(const GFXCmdBuffer** cmdBuffers, int count) override;
};
