#include "gfx/Metal/gfxMetalDevice.h"
#include "gfx/gfxCmdBuffer.h"

static MTLCompareFunction getMetalCmpFunc(GFXCompareFunc func)
{
   switch (func)
   {
   case GFXCompareFunc::NEVER:
      return MTLCompareFunctionNever;
   case GFXCompareFunc::LESS:
      return MTLCompareFunctionLess;
   case GFXCompareFunc::EQUAL:
      return MTLCompareFunctionEqual;
   case GFXCompareFunc::LEQUAL:
      return MTLCompareFunctionLessEqual;
   case GFXCompareFunc::GREATER:
      return MTLCompareFunctionGreater;
   case GFXCompareFunc::NEQUAL:
      return MTLCompareFunctionNever;
   case GFXCompareFunc::GEQUAL:
      return MTLCompareFunctionGreaterEqual;
   case GFXCompareFunc::ALWAYS:
      return MTLCompareFunctionAlways;
   }
}

static MTLStencilOperation getMetalStencilOp(GFXStencilFunc func)
{
   switch (func)
   {
   case GFXStencilFunc::ZERO:
      return MTLStencilOperationZero;
   case GFXStencilFunc::KEEP:
      return MTLStencilOperationKeep;
   case GFXStencilFunc::DECR:
      return MTLStencilOperationDecrementClamp;
   case GFXStencilFunc::INCR:
      return MTLStencilOperationIncrementClamp;
   case GFXStencilFunc::DECR_WRAP:
      return MTLStencilOperationDecrementClamp;
   case GFXStencilFunc::INCR_WRAP:
      return MTLStencilOperationIncrementWrap;
   case GFXStencilFunc::REPLACE:
      return MTLStencilOperationReplace;
   case GFXStencilFunc::INVERT:
      return MTLStencilOperationInvert;
   }
}

static MTLStencilDescriptor* createMetalStencilDescriptor(const GFXDepthStencilStateDesc::GFXStencilDescriptor& desc)
{
   MTLStencilDescriptor *state = [MTLStencilDescriptor alloc];
   state.readMask = desc.stencilReadMask;
   state.writeMask = desc.stencilWriteMask;
   state.stencilCompareFunction = getMetalCmpFunc(desc.stencilCompareOp);
   state.stencilFailureOperation = getMetalStencilOp(desc.stencilFailFunc);
   state.depthStencilPassOperation = getMetalStencilOp(desc.depthPassFunc);
   state.depthFailureOperation = getMetalStencilOp(desc.depthFailFunc);
   
   return state;
}

GFXMetalDevice::GFXMetalDevice()
{
   // TODO: Have to hook this up to the window...pass window handle into the GFX layer...
   mDevice = MTLCreateSystemDefaultDevice();
   mCommandQueue = [mDevice newCommandQueue];
   
   mCaps.hasUnifiedMemory = [mDevice hasUnifiedMemory];
}

BufferHandle GFXMetalDevice::createBuffer(const GFXBufferDesc& desc)
{
   id<MTLBuffer> mtlBuffer;
   NSUInteger bytes = desc.sizeInBytes;
   NSUInteger options = 0;
   

   if (!mCaps.hasUnifiedMemory && desc.usage == BufferUsageEnum::STATIC_GPU_ONLY)
      options |= MTLResourceStorageModePrivate;
   else
      options |= MTLResourceStorageModeShared;

   if (desc.data == NULL)
      mtlBuffer = [mDevice newBufferWithLength:bytes options:options];
   else
      mtlBuffer = [mDevice newBufferWithBytes:desc.data length:bytes options:options];
   
   // TODO: Return
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
   MTLCompareFunction depthFunc = getMetalCmpFunc(desc.depthCompareFunc);
   MTLStencilDescriptor *backStencil = createMetalStencilDescriptor(desc.backFaceStencil);
   MTLStencilDescriptor *frontStencil = createMetalStencilDescriptor(desc.frontFaceStencil);
   
   MTLDepthStencilDescriptor* descriptor = [MTLDepthStencilDescriptor alloc];
   [descriptor setDepthWriteEnabled:desc.enableDepthTest];
   [descriptor setDepthCompareFunction:depthFunc];
   [descriptor setBackFaceStencil:backStencil];
   [descriptor setFrontFaceStencil:frontStencil];
   
   id<MTLDepthStencilState> mtlState = [mDevice newDepthStencilStateWithDescriptor:descriptor];
   
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
   // TODO: Move command buffer recording okutside of executeCmdBuffers and let it record
   // in the actual thread its recording on. At the momemnt, we're mimicing GL and doing
   // translation on the fly every cmd buffer during the submit, but this could be potentially
   // quite a performance improvement to not do it here.
   
   id<MTLCommandBuffer> commandBuffer = [mCommandQueue commandBuffer];
   
   for (int i = 0; i < count; i++)
   {
      const GFXCmdBuffer* cmd = cmdBuffers[i];
      const uint32_t* cmdBuffer = cmd->cmdBuffer;
      size_t offset = 0;
      
      MTLRenderPassDescriptor *renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
      id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
      
      for (;;)
      {
         switch ((CommandType)cmdBuffer[offset++])
         {
         case CommandType::Viewport:
         {
            MTLViewport viewport;
            viewport.originX = cmdBuffer[offset++];
            viewport.originY = cmdBuffer[offset++];
            viewport.width = cmdBuffer[offset++];
            viewport.height = cmdBuffer[offset++];
            viewport.znear = 0.0f;
            viewport.zfar = 1.0f;
            [renderEncoder setViewport:viewport];
            break;
         }
            
         case CommandType::Scissor:
         {
            MTLScissorRect scissor;
            scissor.x = cmdBuffer[offset++];
            scissor.y = cmdBuffer[offset++];
            scissor.width = cmdBuffer[offset++];
            scissor.height = cmdBuffer[offset++];
            
            [renderEncoder setScissorRect:scissor];
            break;
         }
            
         case CommandType::RasterizerState:
         {
            break;
         }
            
         case CommandType::DepthStencilState:
         {
            break;
         }
            
         case CommandType::BlendState:
         {
            break;
         }
            
         case CommandType::BindPipeline:
         {
            break;
         }

         case CommandType::UpdatePushConstants:
         {
            const int pushConstantLookupId = cmdBuffer[offset++];
            const GFXCmdBuffer::PushConstant& pushConstant = cmd->pushConstantPool[pushConstantLookupId];
            
            if (pushConstant.shaderStageBits & GFXShaderStageBit::VERTEX_BIT)
            {
               [renderEncoder setVertexBytes:pushConstant.data
                  length:pushConstant.size
                  atIndex:0
               ];
            }
            
            if (pushConstant.shaderStageBits & GFXShaderStageBit::FRAGMENT_BIT)
            {
               [renderEncoder setFragmentBytes:pushConstant.data
                  length:pushConstant.size
                  atIndex:0
               ];
            }
            break;
         }
            
         case CommandType::BindDescriptorSets:
         {
            break;
         }
            
         case CommandType::BindVertexBuffer:
         {
            break;
         }
            
         case CommandType::BindVertexBuffers:
         {
            break;
         }
            
            
         case CommandType::BindIndexBuffer:
         {
            break;
         }

         case CommandType::DrawPrimitives:
         {
            break;
         }
            
         case CommandType::DrawPrimitivesInstanced:
         {
            break;
         }
            
         case CommandType::DrawIndexedPrimitives:
         {
            break;
         }
            
         case CommandType::DrawIndexedPrimitivesInstanced:
         {
            break;
         }

         case CommandType::End:
         {
            goto done;
         }
         }
      }
   done:
      ;
   }
}
