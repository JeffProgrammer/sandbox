#pragma once

#include <stdint.h>
#include <vector>

#include "gfx/gfxTypes.h"

enum class CommandType
{
   Viewport,
   Scissor,

   RasterizerState,
   DepthState,
   StencilState,
   BlendState,
   BindPipeline,

   UpdatePushConstants,
   BindDescriptorSets,
   BindVertexBuffer,
   BindVertexBuffers,
   BindIndexBuffer,

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
private:
    enum
    {
        COMMAND_BUFFER_SIZE = 4096*4,
        MAX_PUSH_CONSTANT_SIZE_IN_BYTES = 256
    };

    struct PushConstant
    {
        int offset;
        int size;
        char data[MAX_PUSH_CONSTANT_SIZE_IN_BYTES];
    };

    std::vector<PushConstant> pushConstantPool;
    int pushConstantOffset;

    int allocPushConstant(uint32_t offset, uint32_t size, const void* data)
    {
        if (size > MAX_PUSH_CONSTANT_SIZE_IN_BYTES) 
        {
            size = MAX_PUSH_CONSTANT_SIZE_IN_BYTES;

            // if validation...warn?
        }

        PushConstant constant;
        constant.offset = offset;
        constant.size = size;
        memcpy(constant.data, data, size);

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
    void setDepthState(const StateBlockHandle handle);
    void setStencilState(const StateBlockHandle handle);
    void setBlendState(const StateBlockHandle handle);

    void bindPipeline(PipelineHandle handle);

    void updatePushConstants(uint32_t offset, uint32_t size, const void* data);
    //void bindDescriptorSets(/* TODO: parameters */);
    void bindVertexBuffer(uint32_t bindingSlot, BufferHandle buffer, uint32_t stride, uint32_t offset);
    void bindVertexBuffers(uint32_t startBindingSlot, uint32_t count, const BufferHandle *buffers, const uint32_t* strides, const uint32_t* offsets);
    void bindIndexBuffer(BufferHandle buffer, GFXIndexBufferType indexType, uint32_t offset);

    void drawPrimitives(int vertexStart, int vertexCount);
    void drawPrimitivesInstanced(int vertexStart, int vertexCount, int instanceCount);
    void drawIndexedPrimitives(int vertexCount, int indexBufferOffset);
    void drawIndexedPrimitivesInstanced( int vertexCount, int indexBufferOffset, int instanceCount);

    virtual void submit() = 0;
};