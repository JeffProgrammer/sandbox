#include "gfx/gfxCmdBuffer.h"

void GFXCmdBuffer::begin()
{
    memset(cmdBuffer, 0, COMMAND_BUFFER_SIZE);
    offset = 0;
    pushConstantOffset = 0;
}

void GFXCmdBuffer::end()
{
    int type = (int)CommandType::End;
    cmdBuffer[offset++] = type;
}

void GFXCmdBuffer::setViewport(int x, int y, int width, int height)
{
    int type = (int)CommandType::Viewport;
    cmdBuffer[offset++] = type;

    cmdBuffer[offset++] = x;
    cmdBuffer[offset++] = y;
    cmdBuffer[offset++] = width;
    cmdBuffer[offset++] = height;
}

void GFXCmdBuffer::setScissor(int x, int y, int width, int height)
{
    int type = (int)CommandType::Scissor;
    cmdBuffer[offset++] = type;

    cmdBuffer[offset++] = x;
    cmdBuffer[offset++] = y;
    cmdBuffer[offset++] = width;
    cmdBuffer[offset++] = height;
}

void GFXCmdBuffer::setRasterizerState(const StateBlockHandle handle)
{
    int type = (int)CommandType::RasterizerState;
    cmdBuffer[offset++] = type;

    cmdBuffer[offset++] = handle;
}

void GFXCmdBuffer::setDepthStencilState(const StateBlockHandle handle)
{
    int type = (int)CommandType::DepthStencilState;
    cmdBuffer[offset++] = type;

    cmdBuffer[offset++] = handle;
}

void GFXCmdBuffer::setBlendState(const StateBlockHandle handle)
{
    int type = (int)CommandType::BlendState;
    cmdBuffer[offset++] = type;

    cmdBuffer[offset++] = handle;
}

void GFXCmdBuffer::bindPipeline(PipelineHandle handle)
{
    int type = (int)CommandType::BindPipeline;
    cmdBuffer[offset++] = type;

    cmdBuffer[offset++] = handle;
}

void GFXCmdBuffer::bindPushConstants(uint32_t offset, uint32_t size, GFXShaderStageBit shaderStageBits, const void* data)
{
    int type = (int)CommandType::BindPushConstants;
    cmdBuffer[offset++] = type;

    cmdBuffer[offset++] = allocPushConstant(offset, size, shaderStageBits, data);
}

void GFXCmdBuffer::bindVertexBuffer(uint32_t bindingSlot, BufferHandle buffer, uint32_t stride, uint32_t offset)
{
    int type = (int)CommandType::BindVertexBuffer;
    cmdBuffer[offset++] = type;

    cmdBuffer[offset++] = bindingSlot;
    cmdBuffer[offset++] = buffer;
    cmdBuffer[offset++] = stride;
    cmdBuffer[offset++] = offset;
}

void GFXCmdBuffer::bindVertexBuffers(uint32_t startBindingSlot, uint32_t count, const BufferHandle *buffers, const uint32_t* strides, const uint32_t* offsets)
{
    int type = (int)CommandType::BindVertexBuffer;
    cmdBuffer[offset++] = type;

    cmdBuffer[offset++] = startBindingSlot;
    cmdBuffer[offset++] = count;
    for (uint32_t i = 0; i < count; i++)
    {
        cmdBuffer[offset++] = buffers[i];
        cmdBuffer[offset++] = strides[i];
        cmdBuffer[offset++] = offsets[i];
    }
}

void GFXCmdBuffer::bindIndexBuffer(BufferHandle buffer, GFXIndexBufferType indexType, uint32_t offset)
{
    int type = (int)CommandType::BindIndexBuffer;
    cmdBuffer[offset++] = type;

    cmdBuffer[offset++] = buffer;
    cmdBuffer[offset++] = (int)indexType;
    cmdBuffer[offset++] = offset;
}

void GFXCmdBuffer::bindConstantBuffer(uint32_t index, BufferHandle buffer, uint32_t offset, uint32_t size)
{
   int type = (int)CommandType::BindConstantBuffer;
   cmdBuffer[offset++] = type;

   cmdBuffer[offset++] = index;
   cmdBuffer[offset++] = buffer;
   cmdBuffer[offset++] = offset;
   cmdBuffer[offset++] = size;
}

void GFXCmdBuffer::bindTexture(uint32_t index, TextureHandle texture)
{
   int type = (int)CommandType::BindTexture;
   cmdBuffer[offset++] = type;

   cmdBuffer[offset++] = index;
   cmdBuffer[offset++] = texture;
}

void GFXCmdBuffer::bindTextures(uint32_t startIndex, uint32_t count, TextureHandle* textures)
{
   int type = (int)CommandType::BindTextures;
   cmdBuffer[offset++] = type;

   cmdBuffer[offset++] = startIndex;
   cmdBuffer[offset++] = count;
   for (uint32_t i = 0; i < count; ++i)
   {
      cmdBuffer[offset++] = textures[i];
   }
}

void GFXCmdBuffer::bindSampler(uint32_t index, SamplerHandle sampler)
{
   int type = (int)CommandType::BindSampler;
   cmdBuffer[offset++] = type;

   cmdBuffer[offset++] = index;
   cmdBuffer[offset++] = sampler;
}

void GFXCmdBuffer::bindSamplers(uint32_t startIndex, uint32_t count, SamplerHandle* samplers)
{
   int type = (int)CommandType::BindSamplers;
   cmdBuffer[offset++] = type;

   cmdBuffer[offset++] = startIndex;
   cmdBuffer[offset++] = count;
   for (uint32_t i = 0; i < count; ++i)
   {
      cmdBuffer[offset++] = samplers[i];
   }
}

void GFXCmdBuffer::drawPrimitives(int vertexStart, int vertexCount)
{
    int type = (int)CommandType::DrawPrimitives;
    cmdBuffer[offset++] = type;

    cmdBuffer[offset++] = vertexStart;
    cmdBuffer[offset++] = vertexCount;
}

void GFXCmdBuffer::drawPrimitivesInstanced(int vertexStart, int vertexCount, int instanceCount)
{
    int type = (int)CommandType::DrawPrimitivesInstanced;
    cmdBuffer[offset++] = type;

    cmdBuffer[offset++] = vertexStart;
    cmdBuffer[offset++] = vertexCount;
    cmdBuffer[offset++] = instanceCount;
}

void GFXCmdBuffer::drawIndexedPrimitives(int vertexCount, int indexBufferOffset)
{
    int type = (int)CommandType::DrawIndexedPrimitives;
    cmdBuffer[offset++] = type;

    cmdBuffer[offset++] = vertexCount;
    cmdBuffer[offset++] = indexBufferOffset;
}

void GFXCmdBuffer::drawIndexedPrimitivesInstanced(int vertexCount,  int indexBufferOffset, int instanceCount)
{
    int type = (int)CommandType::DrawIndexedPrimitivesInstanced;
    cmdBuffer[offset++] = type;

    cmdBuffer[offset++] = vertexCount;
    cmdBuffer[offset++] = indexBufferOffset;
    cmdBuffer[offset++] = instanceCount;
}
