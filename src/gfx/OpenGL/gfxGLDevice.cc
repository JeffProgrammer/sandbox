#include <assert.h>
#include "gfx/OpenGL/gfxGLDevice.h"

static inline void validateShaderCompilation(GLuint shader)
{
   GLint status;
   glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
   if (!status)
   {
      GLint len;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

      GLchar* log = new GLchar[len];
      glGetShaderInfoLog(shader, len, NULL, log);

      printf("OpenGL Shader Compiler Error: %s\n", log);
      delete[] log;
      abort();
   }
}

static inline void validateShaderLinkCompilation(GLuint program)
{
   GLint status;
   glGetProgramiv(program, GL_LINK_STATUS, &status);
   if (!status)
   {
      GLint len;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

      GLchar* log = new GLchar[len];
      glGetProgramInfoLog(program, len, NULL, log);

      printf("OpenGL Shader Linking Error: %s\n", log);
      delete[] log;
      abort();
   }
}

GFXGLDevice::GFXGLDevice()
{
   glGenVertexArrays(1, &mState.globalVAO);
   glBindVertexArray(mState.globalVAO);

   // enable scissor test by default
   glEnable(GL_SCISSOR_TEST);
}

GFXGLDevice::~GFXGLDevice()
{
   glBindVertexArray(0);
   glDeleteVertexArrays(1, &mState.globalVAO);
}

BufferHandle GFXGLDevice::createBuffer(const GFXBufferDesc& desc)
{
   GLenum usage = _getBufferUsage(desc.usage);
   GLenum type = _getBufferType(desc.type);

   GLuint buffer;
   glGenBuffers(1, &buffer);
   glBindBuffer(type, buffer);
   glBufferData(type, desc.sizeInBytes, desc.data, usage);

   BufferHandle returnHandle = mBufferHandleCounter++;
   mBuffers[returnHandle] = { buffer, desc.usage, type };

   return returnHandle;
}

void GFXGLDevice::deleteBuffer(BufferHandle handle)
{
   const auto& found = mBuffers.find(handle);
   if (found != mBuffers.end())
   {
      glDeleteBuffers(1, &found->second.buffer);
      mBuffers.erase(found);
   }
#ifdef GFX_DEBUG
   else
   {
      assert(false);
   }
#endif
}

PipelineHandle GFXGLDevice::createPipeline(const GFXPipelineDesc& desc)
{
   GLPipeline pipelineState;

   glGenVertexArrays(1, &pipelineState.vaoHandle);
   glBindVertexArray(pipelineState.vaoHandle);

   // see examples here: https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_vertex_attrib_binding.txt
   for (int i = 0; i < desc.inputLayout.count; i++)
   {
      // Note: semantic name is our slot (for DX compatible purposes?)
      const GFXInputLayoutElementDesc& attribute = desc.inputLayout.descs[i];
      GLuint slot = (GLuint)attribute.slot;

      glEnableVertexAttribArray(slot);
      glVertexAttribFormat(slot, attribute.count, _getInputLayoutType(attribute.type), GL_FALSE, attribute.offset);
      glVertexAttribBinding(slot, attribute.bufferBinding);
      glVertexBindingDivisor(slot, attribute.divisor == GFXInputLayoutDivisor::PER_VERTEX ? 0 : 1);
   }

   glBindVertexArray(mState.globalVAO);

   pipelineState.primitiveType = _getPrimitiveType(desc.primitiveType);
   pipelineState.shader = _createShaderProgram(desc.shadersStages, desc.shaderStageCount); 

   PipelineHandle returnHandle = mPipelineHandleCounter++;
   mPipelines[returnHandle] = pipelineState;
   return returnHandle;
}

void GFXGLDevice::deletePipeline(PipelineHandle handle)
{
   const auto& found = mPipelines.find(handle);
   if (found != mPipelines.end())
   {
      glDeleteVertexArrays(1, &found->second.vaoHandle);


      mPipelines.erase(found);
   }
#ifdef GFX_DEBUG
   else
   {
      assert(false);
   }
#endif
}

StateBlockHandle GFXGLDevice::createRasterizerState(const GFXRasterizerStateDesc& desc)
{
   GLRasterizerState state;

   switch (desc.cullMode)
   {
   case GFXCullMode::CULL_NONE:
      state.enableFaceCulling = false;
      state.cullMode = GL_NONE;
      break;
   case GFXCullMode::CULL_BACK:
      state.enableFaceCulling = true;
      state.cullMode = GL_BACK;
      break;
   default:
      state.enableFaceCulling = true;
      state.cullMode = GL_FRONT;
   }

   state.windingOrder = desc.windingMode == GFXWindingMode::CLOCKWISE ? GL_CW : GL_CCW;
   state.polygonFillMode = desc.fillMode == GFXFillMode::SOLID ? GL_FILL : GL_LINE;

   StateBlockHandle handle = mRasterizerHandleCounter++;
   mRasterizerStates[handle] = std::move(state);
   return handle;
}

StateBlockHandle GFXGLDevice::createDepthStencilState(const GFXDepthStencilStateDesc& desc)
{
   GLDepthStencilState state;
   state.enableDepthTest = desc.enableDepthTest;
   state.depthCompareFunc = _getCompareFunc(desc.depthCompareFunc);

   state.frontFaceStencil.depthFailFunc = _getStencilFunc(desc.frontFaceStencil.depthFailFunc);
   state.frontFaceStencil.depthPassFunc = _getStencilFunc(desc.frontFaceStencil.depthPassFunc);
   state.frontFaceStencil.stencilFailFunc = _getStencilFunc(desc.frontFaceStencil.stencilFailFunc);
   state.frontFaceStencil.stencilCompareOp = _getCompareFunc(desc.frontFaceStencil.stencilCompareOp);
   state.frontFaceStencil.stencilReadMask = desc.frontFaceStencil.stencilReadMask;
   state.frontFaceStencil.stencilWriteMask = desc.frontFaceStencil.stencilWriteMask;
   state.frontFaceStencil.referenceValue = desc.frontFaceStencil.referenceValue;

   state.backFaceStencil.depthFailFunc = _getStencilFunc(desc.backFaceStencil.depthFailFunc);
   state.backFaceStencil.depthPassFunc = _getStencilFunc(desc.backFaceStencil.depthPassFunc);
   state.backFaceStencil.stencilFailFunc = _getStencilFunc(desc.backFaceStencil.stencilFailFunc);
   state.backFaceStencil.stencilCompareOp = _getCompareFunc(desc.backFaceStencil.stencilCompareOp);
   state.backFaceStencil.stencilReadMask = desc.backFaceStencil.stencilReadMask;
   state.backFaceStencil.stencilWriteMask = desc.backFaceStencil.stencilWriteMask;
   state.backFaceStencil.referenceValue = desc.backFaceStencil.referenceValue;

   StateBlockHandle handle = mDepthStencilStateHandleCounter++;
   mDepthStencilStates[handle] = state;
   return handle;
}

StateBlockHandle GFXGLDevice::createBlendState(const GFXBlendStateDesc& desc)
{
   assert(false && "GFXGLDevice::createBlendState() is not implemented");
   return 0;
}

void GFXGLDevice::deleteStateBlock(StateBlockHandle handle)
{
   assert(false && "GFXGLDevice::deleteStateBlock() is not implemented");
}

SamplerHandle GFXGLDevice::createSampler(const GFXSamplerStateDesc& desc)
{
   GLuint sampler;
   glGenSamplers(1, &sampler);
   glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, _getSamplerMinFilteRMode(desc.minFilterMode));
   glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, _getSamplerMagFilterMode(desc.magFilterMode));
   glSamplerParameteri(sampler, GL_TEXTURE_MIN_LOD, desc.minLOD);
   glSamplerParameteri(sampler, GL_TEXTURE_MAX_LOD, desc.maxLOD);
   glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, _getSamplerWrapMode(desc.wrapS));
   glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, _getSamplerWrapMode(desc.wrapT));
   glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, _getSamplerWrapMode(desc.wrapR));

   float colors[4] = { desc.borderColorR, desc.borderColorG, desc.borderColorB, desc.borderColorA };
   glSamplerParameterfv(sampler, GL_TEXTURE_BORDER_COLOR, colors);

   if (desc.compareMode != GFXSamplerCompareMode::NONE)
   {
      glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_MODE, _getSamplerCompareMode(desc.compareMode));
      glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_FUNC, _getCompareFunc(desc.compareFunc));
   }

   GLSampler state;
   state.handle = sampler;

   SamplerHandle samplerHandle = mSamplerHandleCounter++;
   mSamplers[samplerHandle] = std::move(state);
   return samplerHandle;
}

void GFXGLDevice::deleteSampler(SamplerHandle handle)
{
   const auto& found = mSamplers.find(handle);
   if (found != mSamplers.end())
   {
      glDeleteSamplers(1, &found->second.handle);

      mSamplers.erase(found);
   }
#ifdef GFX_DEBUG
   else
   {
      assert(false);
   }
#endif
}

TextureHandle GFXGLDevice::createTexture(const GFXTextureStateDesc& desc)
{
   assert(false && "GFXGLDevice::createTexture() is not implemented");
   return 0;
}

void GFXGLDevice::deleteTexture(TextureHandle handle)
{
   assert(false && "GFXGLDevice::deleteTexture() is not implemented");
}

void* GFXGLDevice::mapBuffer(BufferHandle handle, uint32_t offset, uint32_t size)
{
   // At the moment, this is a REALLY SLOW WAY TO UPDATE A BUFFER. We can do _A TON_ of optimizations here
   // For example, with small buffers on certain venders and buffer types (eg. nvidia ubos) we should
   // always use glBufferSubData.
   //
   // For modern drivers (GL_ARB_BUFFER_STORAGE) we should use persistent mapping if the buffer is going to
   // be updated all the time.
   //
   // We should also double or tripple buffer if we can (unless the buffer is ENORMOUS)
   //
   // For an ES2.0 fallback for 2d, we always want to use glBufferSubData() as map buffer isn't a thing!

   const GLBuffer buffer = mBuffers[handle];
   glBindBuffer(buffer.type, buffer.buffer);

   mState.currentMappedBuffer = buffer.buffer;
   mState.currentMappedBufferType = buffer.type;

   return glMapBufferRange(buffer.type, offset, size, GL_MAP_WRITE_BIT);
}

void GFXGLDevice::unmapBuffer(BufferHandle handle)
{
   const GLBuffer buffer = mBuffers[handle];
   if (mState.currentMappedBuffer != buffer.buffer || mState.currentMappedBufferType != buffer.type)
   {
      // Optimization: Bind before use if we're not modifying the same buffer
      glBindBuffer(buffer.type, buffer.buffer);
   }

   glUnmapBuffer(buffer.type);
   mState.currentMappedBuffer = 0;
   mState.currentMappedBufferType = 0;
}

void GFXGLDevice::executeCmdBuffers(const GFXCmdBuffer** cmdBuffers, int count)
{
   for (int i = 0; i < count; i++)
   {
      const GFXCmdBuffer* cmd = cmdBuffers[i];
      const uint32_t* cmdBuffer = cmd->cmdBuffer;

      size_t offset = 0;
      for (;;)
      {
         switch ((CommandType)cmd->cmdBuffer[offset++])
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

         case CommandType::RasterizerState:
         {
            int handle = cmdBuffer[offset++];
            const GLRasterizerState& rasterState = mRasterizerStates[handle];

            if (rasterState.enableFaceCulling)
            {
               glEnable(GL_CULL_FACE);
               glCullFace(rasterState.cullMode);
               glFrontFace(rasterState.windingOrder);
            }
            else
            {
               glDisable(GL_CULL_FACE);
            }

            glPolygonMode(GL_FRONT_AND_BACK, rasterState.polygonFillMode);

            break;
         }

         case CommandType::DepthStencilState:
         {
            int handle = cmdBuffer[offset++];
            const GLDepthStencilState& depthStencil = mDepthStencilStates[handle];

            // Depth Settings
            if (depthStencil.enableDepthTest)
            {
               glEnable(GL_DEPTH_TEST);
               glDepthFunc(depthStencil.depthCompareFunc);
            }
            else
            {
               glDisable(GL_DEPTH_TEST);
            }
            glDepthMask(depthStencil.enableDepthWrite);

            // Stencil Settings
            if (depthStencil.enableStencilTest)
            {
               glEnable(GL_STENCIL_TEST);

               glStencilFuncSeparate(
                  GL_FRONT, 
                  depthStencil.frontFaceStencil.stencilCompareOp, 
                  depthStencil.frontFaceStencil.referenceValue, 
                  depthStencil.frontFaceStencil.stencilReadMask
               );

               glStencilFuncSeparate(
                  GL_BACK,
                  depthStencil.backFaceStencil.stencilCompareOp,
                  depthStencil.backFaceStencil.referenceValue,
                  depthStencil.backFaceStencil.stencilReadMask
               );

               glStencilOpSeparate(
                  GL_FRONT,
                  depthStencil.frontFaceStencil.stencilFailFunc,
                  depthStencil.frontFaceStencil.depthFailFunc,
                  depthStencil.frontFaceStencil.depthPassFunc
               );

               glStencilOpSeparate(
                  GL_BACK,
                  depthStencil.backFaceStencil.stencilFailFunc,
                  depthStencil.backFaceStencil.depthFailFunc,
                  depthStencil.backFaceStencil.depthPassFunc
               );

               glStencilMaskSeparate(GL_FRONT, depthStencil.frontFaceStencil.stencilWriteMask);
               glStencilMaskSeparate(GL_BACK, depthStencil.backFaceStencil.stencilWriteMask);
            }
            else
            {
               glDisable(GL_STENCIL_TEST);
            }

            break;
         }

         case CommandType::BlendState:
         {
            break;
         }

         case CommandType::BindPipeline:
         {
            const BufferHandle handle = static_cast<BufferHandle>(cmdBuffer[offset++]);
            const GFXGLDevice::GLPipeline& pipeline = mPipelines[handle];

            glBindVertexArray(pipeline.vaoHandle);
            glUseProgram(pipeline.shader);

            mState.currentProgram = pipeline.shader;
            mState.primitiveType = pipeline.primitiveType;
            break;
         }

         case CommandType::BindPushConstants:
         {
            // for now assume offset is always 0... TODO: keep a copy in ram
            // and then upload everything (or just update changes if possible?)


            const int pushConstantLookupId = cmdBuffer[offset++];
            const GFXCmdBuffer::PushConstant& pushC = cmd->pushConstantPool[pushConstantLookupId];
            const int count = pushC.size / PUSH_CONSTANT_STRIDE;

            glUniform4fv(mState.pushConstantLocation, count, (const GLfloat*)&pushC.data[0]);
            break;
         }

         case CommandType::BindDescriptorSets:
         {
            //https://developer.nvidia.com/vulkan-shader-resource-binding
            break;
         }

         case CommandType::BindVertexBuffer:
         {
            GLuint bindingSlot = cmdBuffer[offset++];
            GLuint buffer = mBuffers[cmdBuffer[offset++]].buffer;
            GLsizei stride = static_cast<GLsizei>(cmdBuffer[offset++]);
            GLintptr bufferOffset = static_cast<GLintptr>(cmdBuffer[offset++]);

            glBindVertexBuffer(bindingSlot, buffer, bufferOffset, stride);
            break;
         }

         case CommandType::BindVertexBuffers:
         {
            GLuint startBindingSlot = cmdBuffer[offset++];
            GLsizei count = static_cast<GLsizei>(cmdBuffer[offset++]);

            static GLuint buffers[8];
            static GLsizei strides[8];
            static GLintptr offsets[8];

            for (GLsizei i = 0; i < count; i++)
            {
               buffers[i] = mBuffers[cmdBuffer[offset++]].buffer;
               strides[i] = static_cast<GLsizei>(cmdBuffer[offset++]);
               offsets[i] = static_cast<GLintptr>(cmdBuffer[offset++]);
            }

            if (mCaps.hasMultiBind)
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
            const GLuint bufferOffset = cmdBuffer[offset++];
            const GLuint buffer = mBuffers[handle].buffer;

            mState.indexBufferType = type == GFXIndexBufferType::BITS_16 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
            break;
         }

         case CommandType::BindConstantBuffer:
         {
            const uint32_t index = cmdBuffer[offset++];
            const BufferHandle handle = static_cast<BufferHandle>(cmdBuffer[offset++]);
            const uint32_t bufferOffset = cmdBuffer[offset++];
            const uint32_t size = cmdBuffer[offset++];

            const GLuint buffer = mBuffers[handle].buffer;

            glBindBufferRange(GL_UNIFORM_BUFFER, index, buffer, bufferOffset, size);
            break;
         }

         case CommandType::BindTexture:
         {
            break;
         }

         case CommandType::BindTextures:
         {
            break;
         }

         case CommandType::BindSampler:
         {
            const uint32_t index = cmdBuffer[offset++];
            const SamplerHandle handle = static_cast<SamplerHandle>(cmdBuffer[offset++]);
            const GLuint sampler = mSamplers[handle].handle;

            glBindSampler(index, sampler);
            break;
         }

         case CommandType::BindSamplers:
         {
            const uint32_t startingIndex = cmdBuffer[offset++];
            const uint32_t count = cmdBuffer[offset++];

            if (mCaps.hasMultiBind)
            {
               GLuint samplers[32];
               for (uint32_t i = 0; i < count; ++i)
                  samplers[i] = mSamplers[(SamplerHandle)cmdBuffer[offset++]].handle;

               glBindSamplers(startingIndex, count, samplers);
            }
            else
            {
               for (uint32_t i = 0; i < count; i++)
               {
                  uint32_t sampler = mSamplers[(SamplerHandle)cmdBuffer[offset++]].handle;
                  glBindSampler(startingIndex + i, sampler);
               }
            }

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
            GLuint indexBufferOffset = cmdBuffer[offset++];

            glDrawElements(mState.primitiveType, vertexCount, mState.indexBufferType, (void*)indexBufferOffset);
            break;
         }

         case CommandType::DrawIndexedPrimitivesInstanced:
         {
            int vertexCount = cmdBuffer[offset++];
            GLuint indexBufferOffset = cmdBuffer[offset++];
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
}

GLenum GFXGLDevice::_getBufferUsage(GFXBufferUsageEnum usage) const
{
   switch (usage)
   {
   case GFXBufferUsageEnum::STATIC_GPU_ONLY: 
      return GL_STATIC_DRAW;
   case GFXBufferUsageEnum::DYNAMIC_CPU_TO_GPU: 
      return GL_DYNAMIC_DRAW;
   }

   // error
   return 0;
}

GLenum GFXGLDevice::_getBufferType(GFXBufferType type) const
{
   switch (type)
   {
   case GFXBufferType::VERTEX_BUFFER:
      return GL_ARRAY_BUFFER;
   case GFXBufferType::INDEX_BUFFER:
      return GL_ELEMENT_ARRAY_BUFFER;
   case GFXBufferType::CONSTANT_BUFFER:
      return GL_UNIFORM_BUFFER;
   }

   // error
   return 0;
}

GLenum GFXGLDevice::_getPrimitiveType(GFXPrimitiveType primitiveType) const
{
   switch (primitiveType)
   {
   case GFXPrimitiveType::TRIANGLE_LIST:
      return GL_TRIANGLES;
   case GFXPrimitiveType::TRIANGLE_STRIP:
      return GL_TRIANGLE_STRIP;
   case GFXPrimitiveType::POINT_LIST:
      return GL_POINTS;
   case GFXPrimitiveType::LINE_LIST:
      return GL_LINES;
   case GFXPrimitiveType::LINE_STRIP:
      return GL_LINE_STRIP;
   }

   // error
   return 0;
}

GLenum GFXGLDevice::_getStencilFunc(GFXStencilFunc func) const
{
   switch (func)
   {
   case GFXStencilFunc::KEEP:
      return GL_KEEP;
   case GFXStencilFunc::ZERO:
      return GL_ZERO;
   case GFXStencilFunc::REPLACE:
      return GL_REPLACE;
   case GFXStencilFunc::INCR_WRAP:
      return GL_INCR_WRAP;
   case GFXStencilFunc::DECR_WRAP:
      return GL_DECR_WRAP;
   case GFXStencilFunc::INVERT:
      return GL_INVERT;
   case GFXStencilFunc::INCR:
      return GL_INCR;
   case GFXStencilFunc::DECR:
      return GL_DECR;
   }

   // error
   return 0;
}

GLenum GFXGLDevice::_getCompareFunc(GFXCompareFunc func) const
{
   switch (func)
   {
   case GFXCompareFunc::EQUAL:
      return GL_EQUAL;
   case GFXCompareFunc::NEQUAL:
      return GL_NOTEQUAL;
   case GFXCompareFunc::LESS:
      return GL_LESS;
   case GFXCompareFunc::GREATER:
      return GL_GREATER;
   case GFXCompareFunc::LEQUAL:
      return GL_LEQUAL;
   case GFXCompareFunc::GEQUAL:
      return GL_GEQUAL;
   case GFXCompareFunc::ALWAYS:
      return GL_ALWAYS;
   case GFXCompareFunc::NEVER:
      return GL_NEVER;
   }

   // error
   return 0;
}

GLenum GFXGLDevice::_getShaderType(GFXShaderType type) const
{
   switch (type)
   {
   case GFXShaderType::VERTEX:
      return GL_VERTEX_SHADER;
   case GFXShaderType::FRAGMENT:
      return GL_FRAGMENT_SHADER;
   }

   // error
   return 0;
}

GLenum GFXGLDevice::_getInputLayoutType(GFXInputLayoutFormat format) const
{
   switch (format)
   {
   case GFXInputLayoutFormat::FLOAT:
      return GL_FLOAT;
   case GFXInputLayoutFormat::BYTE:
      return GL_BYTE;
   case GFXInputLayoutFormat::SHORT:
      return GL_SHORT;
   case GFXInputLayoutFormat::INT:
      return GL_INT;
   }

   // error
   return 0;
}

GLenum GFXGLDevice::_getSamplerWrapMode(GFXSamplerWrapMode mode) const
{
   switch (mode)
   {
   case GFXSamplerWrapMode::CLAMP_TO_EDGE:
      return GL_CLAMP_TO_EDGE;
   case GFXSamplerWrapMode::MIRRORED_REPEAT:
      return GL_MIRRORED_REPEAT;
   case GFXSamplerWrapMode::REPEAT:
      return GL_REPEAT;
   }

   // error
   return 0;
}

GLenum GFXGLDevice::_getSamplerMagFilterMode(GFXSamplerMagFilterMode mode) const
{
   switch (mode)
   {
   case GFXSamplerMagFilterMode::LINEAR:
      return GL_LINEAR;
   case GFXSamplerMagFilterMode::NEAREST:
      return GL_NEAREST;
   }

   // error
   return 0;
}

GLenum GFXGLDevice::_getSamplerMinFilteRMode(GFXSamplerMinFilterMode mode) const
{
   switch (mode)
   {
   case GFXSamplerMinFilterMode::LINEAR:
      return GL_LINEAR;
   case GFXSamplerMinFilterMode::NEAREST:
      return GL_NEAREST;
   case GFXSamplerMinFilterMode::NEAREST_MIP:
      return GL_NEAREST_MIPMAP_NEAREST;
   case GFXSamplerMinFilterMode::NEAREST_MIP_WEIGHTED:
      return GL_NEAREST_MIPMAP_LINEAR;
   case GFXSamplerMinFilterMode::LINEAR_MIP:
      return GL_LINEAR_MIPMAP_NEAREST;
   case GFXSamplerMinFilterMode::LINEAR_MIP_WEIGHTED:
      return GL_LINEAR_MIPMAP_LINEAR;
   }

   // error
   return 0;
}

GLenum GFXGLDevice::_getSamplerCompareMode(GFXSamplerCompareMode mode) const
{
   switch (mode)
   {
   case GFXSamplerCompareMode::REFERENCE_TO_TEXTURE:
      return GL_COMPARE_REF_TO_TEXTURE;
   case GFXSamplerCompareMode::NONE:
      return GL_NONE;
   }

   // error
   return 0;
}

GLuint GFXGLDevice::_createShaderProgram(const GFXShaderDesc* shader, uint32_t count)
{
   std::vector<GLuint> glHandles;

   for (uint32_t i = 0; i < count; i++)
   {
      const GFXShaderDesc& shaderStage = shader[i];
      GLenum shaderType = _getShaderType(shaderStage.type);

      GLuint handle = glCreateShader(shaderType);
      glShaderSource(handle, 1, &(shaderStage.code), NULL);
      glCompileShader(handle);
      validateShaderCompilation(handle);

      glHandles.push_back(handle);
   }

   GLuint shaderProgram = glCreateProgram();
   for (GLuint handle : glHandles)
      glAttachShader(shaderProgram, handle);

   glLinkProgram(shaderProgram);
   validateShaderLinkCompilation(shaderProgram);

   return shaderProgram;
}