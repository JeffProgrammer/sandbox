#pragma once

#include <stdint.h>
#include <vector>

#include "gfx/gfxTypes.h"

enum class CommandType
{
   Viewport,
   Scissor,

   RasterizerState,
   DepthStencilState,
   BlendState,
   BindPipeline,

   BindPushConstants,
   BindDescriptorSets,
   BindVertexBuffer,
   BindVertexBuffers,
   BindIndexBuffer,
   BindConstantBuffer,
   BindTexture,
   BindTextures,
   BindSampler,
   BindSamplers,

   DrawPrimitives,
   DrawPrimitivesInstanced,
   DrawIndexedPrimitives,
   DrawIndexedPrimitivesInstanced,

   End
};

class GFXCmdBuffer
{
   friend class GFXDevice;
   friend class GFXGLDevice;
   friend class GFXMetalDevice;
private:
    enum
    {
        COMMAND_BUFFER_SIZE = 16384,
       
        // Note: The min spec for Vulkan is 128 bytes so we should
        // assume no more than this is supported. If we have a REALLY
        // good usecase for allowing more, we have to take this into consideration.
        MAX_PUSH_CONSTANT_SIZE_IN_BYTES = 128,
       
        PUSH_BUFFER_CONSTANT_STRIDE = 16, // same as PUSH_CONSTANT_STRIDE
    };

    struct PushConstant
    {
        int offset;
        int size;
        GFXShaderStageBit shaderStageBits;
        char data[MAX_PUSH_CONSTANT_SIZE_IN_BYTES];
    };

    std::vector<PushConstant> pushConstantPool;
    int pushConstantOffset;

    int allocPushConstant(uint32_t offset, uint32_t size, GFXShaderStageBit shaderStageBits, const void* data)
    {
        if (size > MAX_PUSH_CONSTANT_SIZE_IN_BYTES) 
        {
            size = MAX_PUSH_CONSTANT_SIZE_IN_BYTES;

            // if validation...warn?
        }

        PushConstant constant;
        constant.offset = offset;
        constant.size = size;
        constant.shaderStageBits = shaderStageBits;
        memcpy(constant.data, (char*)data, size);

        if (size % PUSH_BUFFER_CONSTANT_STRIDE) {
           // if validation...warn?
           abort();
        }

        if (pushConstantOffset < pushConstantPool.size())
        {
            pushConstantPool[pushConstantOffset] = std::move(constant);
        }
        else
        {
            pushConstantPool.push_back(constant);
        }

        return pushConstantOffset++;
    }

    uint32_t cmdBuffer[COMMAND_BUFFER_SIZE];
    size_t offset;
    
public:
    virtual void begin();
    virtual void end();

    void setViewport(int x, int y, int width, int height);
    void setScissor(int x, int y, int width, int height);
    void setRasterizerState(const StateBlockHandle handle);
    void setDepthStencilState(const StateBlockHandle handle);
    void setBlendState(const StateBlockHandle handle);

    void bindPipeline(PipelineHandle handle);

    void bindPushConstants(uint32_t offset, uint32_t size, GFXShaderStageBit shaderStageBits, const void* data);
    void bindVertexBuffer(uint32_t bindingSlot, BufferHandle buffer, uint32_t stride, uint32_t offset);
    void bindVertexBuffers(uint32_t startBindingSlot, uint32_t count, const BufferHandle *buffers, const uint32_t* strides, const uint32_t* offsets);
    void bindIndexBuffer(BufferHandle buffer, GFXIndexBufferType indexType, uint32_t offset);
    void bindConstantBuffer(uint32_t index, BufferHandle buffer, uint32_t offset, uint32_t size);
    void bindTexture(uint32_t index, TextureHandle texture);
    void bindTextures(uint32_t startIndex, uint32_t count, TextureHandle* textures);
    void bindSampler(uint32_t index, SamplerHandle sampler);
    void bindSamplers(uint32_t startIndex, uint32_t count, SamplerHandle* samplers);

    void drawPrimitives(int vertexStart, int vertexCount);
    void drawPrimitivesInstanced(int vertexStart, int vertexCount, int instanceCount);
    void drawIndexedPrimitives(int vertexCount, int indexBufferOffset);
    void drawIndexedPrimitivesInstanced( int vertexCount, int indexBufferOffset, int instanceCount);
};
