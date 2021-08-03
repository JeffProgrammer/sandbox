#include "gfx/OpenGL/gfxGLDevice.h"

static void validateShaderCompilation(GLuint shader)
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

static void validateShaderLinkCompilation(GLuint program)
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
   case CullMode::CULL_NONE:
      state.enableFaceCulling = false;
      state.cullMode = GL_NONE;
      break;
   case CullMode::CULL_BACK:
      state.enableFaceCulling = true;
      state.cullMode = GL_BACK;
      break;
   default:
      state.enableFaceCulling = true;
      state.cullMode = GL_FRONT;
   }

   state.windingOrder = desc.windingMode == WindingMode::CLOCKWISE ? GL_CW : GL_CCW;
   state.polygonFillMode = desc.fillMode == FillMode::SOLID ? GL_FILL : GL_LINE;

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
   state.frontFaceStencil.stencilPassFunc = _getStencilFunc(desc.frontFaceStencil.stencilPassFunc);
   state.frontFaceStencil.stencilReadMask = desc.frontFaceStencil.stencilReadMask;
   state.frontFaceStencil.stencilWriteMask = desc.frontFaceStencil.stencilWriteMask;

   state.backFaceStencil.depthFailFunc = _getStencilFunc(desc.backFaceStencil.depthFailFunc);
   state.backFaceStencil.depthPassFunc = _getStencilFunc(desc.backFaceStencil.depthPassFunc);
   state.backFaceStencil.stencilFailFunc = _getStencilFunc(desc.backFaceStencil.stencilFailFunc);
   state.backFaceStencil.stencilPassFunc = _getStencilFunc(desc.backFaceStencil.stencilPassFunc);
   state.backFaceStencil.stencilReadMask = desc.backFaceStencil.stencilReadMask;
   state.backFaceStencil.stencilWriteMask = desc.backFaceStencil.stencilWriteMask;

   StateBlockHandle handle = mDepthStencilStateHandleCounter++;
   mDepthStencilStates[handle] = state;
   return handle;
}

StateBlockHandle GFXGLDevice::createBlendState(const GFXBlendStateDesc& desc)
{
   return 0;
}

void GFXGLDevice::deleteStateBlock(StateBlockHandle handle)
{

}

void* GFXGLDevice::mapBuffer(BufferHandle handle)
{
   return nullptr;
}

void GFXGLDevice::unmapBuffer(BufferHandle handle)
{
   //const auto& found = mBuffers.find(handle);
   //glUnmapBuffer(found->second.type);
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

         case CommandType::UpdatePushConstants:
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
            GLintptr offset = static_cast<GLintptr>(cmdBuffer[offset++]);

            glBindVertexBuffer(bindingSlot, buffer, offset, stride);
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

            if (true)//mCaps.hasBindVertexBuffers)
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

GLenum GFXGLDevice::_getBufferUsage(BufferUsageEnum usage) const
{
   switch (usage)
   {
   case BufferUsageEnum::STATIC_DRAW: 
      return GL_STATIC_DRAW;
   case BufferUsageEnum::DYNAMIC_DRAW: 
      return GL_DYNAMIC_DRAW;
   }

   // error
   return 0;
}

GLenum GFXGLDevice::_getBufferType(BufferType type) const
{
   switch (type)
   {
   case BufferType::VERTEX_BUFFER:
      return GL_ARRAY_BUFFER;
   case BufferType::INDEX_BUFFER:
      return GL_ELEMENT_ARRAY_BUFFER;
   case BufferType::CONSTANT_BUFFER:
      return GL_UNIFORM_BUFFER;
   }

   // error
   return 0;
}

GLenum GFXGLDevice::_getPrimitiveType(PrimitiveType primitiveType) const
{
   switch (primitiveType)
   {
   case PrimitiveType::TRIANGLE_LIST:
      return GL_TRIANGLES;
   case PrimitiveType::TRIANGLE_STRIP:
      return GL_TRIANGLE_STRIP;
   case PrimitiveType::POINT_LIST:
      return GL_POINTS;
   case PrimitiveType::LINE_LIST:
      return GL_LINES;
   case PrimitiveType::LINE_STRIP:
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

GLuint GFXGLDevice::_createShaderProgram(const GFXShaderDesc* shader, uint32_t count)
{
   std::vector<GLuint> glHandles;

   for (uint32_t i = 0; i < count; i++)
   {
      const GFXShaderDesc& shaderStage = shader[i];
      GLenum shaderType = _getShaderType(shaderStage.type);

      GLuint handle = glCreateShader(shaderType);
      glShaderSource(handle, 1, &shader->code, NULL);
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