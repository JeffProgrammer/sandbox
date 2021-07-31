#define Deserialize(CMD, name) \
    CMD name; \
    memcpy(buffer + offset, &name, sizeof(CMD)); \
    off += sizeof(CMD)

void GFXGLCmdBuffer::submit()
{
    size_t offset = 0;
    for (;;)
    {
        switch (cmdBuffer[offset++])
        {
            case CommandType::Viewport:
            {
                int x = cmdBuffer[offset++];
                int y = cmdBuffer[offset++];
                int w = cmdBuffer[offset++];
                int h = cmdBuffer[offset++];
                glViewport(x, y, w, h);
                break;
            }

            case CommandType::Scissor:
            {
                int x = cmdBuffer[offset++];
                int y = cmdBuffer[offset++];
                int w = cmdBuffer[offset++];
                int h = cmdBuffer[offset++];
                glScissor(x, y, w, h);
                break;
            }

            case CommandType::RasterizerState
            {
                break;
            }

            case CommandType::DepthState:
            {
                break;
            }

            case CommandType::StencilState:
            {
                break;
            }

            case CommandType::BlendState:
            {
                break;
            }

            case CommandType::BindPipeline
            {
                const BufferHandle handle = static_cast<BufferHandle>(cmdBuffer[offset++]);
                const GLPipeline& pipeline = mPipelines[handle];

                glBindVertexArray(pipeline.vaoHandle);
                glUseProgram(pipeline.shader);

                mState.currentProgram = pipeline.shader;
                mState.primitiveType = pipeline.primitiveType;
                break;
            }

            case CommandType::UpdatePushConstants
            {

                break;
            }

            case CommandType::BindDescriptorSets
            {
                //https://developer.nvidia.com/vulkan-shader-resource-binding
                break;
            }

            case CommandType::BindVertexBuffer:
            {
                GLuint bindingSlot = cmdBuffer[offset++];
                GLuint buffer = mBuffers[cmdBuffer[offset++]].buffer;
                GLsizei stride = static_cast<GLsizei>(cmdBuffer[offset++]);
                GLintptr offset = static_cast<GLintptr>(cmdBuffer[offset++]);

                glBindVertexBuffer(bindingSlot, buffer, offset, stride);
                break;
            }

            case CommandType::BindVertexBuffers:
            {
                GLuint startBindingSlot = cmdBuffer[offset++];
                GLsizei count = static_cast<GLsizei>(cmdBuffer[offset++]);

                GLuint* buffers = _alloca(sizeof(GLuint) * count);
                GLintptr* strides = _alloca(sizeof(GLintptr) * count);
                GLsizei* offsets = _alloca(sizeof(GLsizei) * count);

                for (GLsizei i = 0; i < count; i++)
                {
                    buffers[i] = mBuffers[cmdBuffer[offset++]].buffer;
                    strides[i] = static_cast<GLsizei>(cmdBuffer[offset++]);
                    offsets[i] = static_cast<GLintptr>(cmdBuffer[offset++]);
                }

                if (mCaps.hasBindVertexBuffers)
                {
                    glBindVertexBuffers(startBindingSlot, count, buffers, offsets, strides);
                }
                else
                {
                    for (GLsizei i = 0; i < count; i++)
                    {
                        glBindVertexBuffer(startBindingSlot + i, buffers[i], offsets[i], strides[i]);
                    }
                }

                break;
            }
            
            case CommandType::BindIndexBuffer:
            {
                const BufferHandle handle = static_cast<BufferHandle>(cmdBuffer[offset++]);
                const GFXIndexBufferType type = (GFXIndexBufferType)cmdBuffer[offset++];
                const GLuint buffer = mBuffers[handle].buffer;

                mState.indexBufferType = type == GFXIndexBufferType::BITS_16 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
                break;
            }

            case CommandType::DrawPrimitives:
            {
                int vertexStart = cmdBuffer[offset++];
                int vertexCount = cmdBuffer[offset++];

                glDrawArrays(mState.primitiveType, vertexStart, vertexCount);
                break;
            }

            case CommandType::DrawPrimitivesInstanced:
            {
                int vertexStart = cmdBuffer[offset++];
                int vertexCount = cmdBuffer[offset++];
                int instanceCount = cmdBuffer[offset++];

                glDrawArraysInstanced(mState.primitiveType, vertexStart, vertexCount, instanceCount);
                break;
            }
            
            case CommandType::DrawIndexedPrimitives:
            {
                int vertexCount = cmdBuffer[offset++];
                GLuint indexBuffsetOffset = cmdBuffer[offset++];

                glDrawElements(mState.primitiveType, vertexCount, mState.indexBufferType, (void*)indexBufferOffset);
                break;
            }

            case CommandType::DrawIndexedPrimitivesInstanced:
            {
                int vertexCount = cmdBuffer[offset++];
                GLuint indexBuffsetOffset = cmdBuffer[offset++];
                int instanceCount = cmdBuffer[offset++];

                glDrawElementsInstanced(mState.primitiveType, vertexCount, mState.indexBufferType, (void*)indexBufferOffset, instanceCount);
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