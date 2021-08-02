#pragma once

#include <unordered_map>
#include <glad/glad.h>
#include "gfx/gfxDevice.h"
#include "gfx/gfxCmdBuffer.h"

class GFXGLDevice : public GFXDevice
{
   friend class GFXGLCmdBuffer;

   struct GLBuffer
   {
      GLuint buffer;
      BufferUsageEnum usage;
      GLenum type;
   };

   struct GLPipeline
   {
      GLuint vaoHandle;
      GLuint shader;
      GLenum primitiveType;
   };

   struct GLRasterizerState
   {

   };

   struct GLDepthState
   {

   };

   struct GLStencilState
   {

   };

   struct GLBlendState
   {

   };

   struct
   {
      GLuint primitiveType;
      GLuint currentProgram;
      GLenum indexBufferType;

      // cached states...
      GLRasterizerState cacheRasterizerState;
      GLDepthState cacheDepthState;
      GLStencilState cacheStencilState;
      GLBlendState cacheBlendState;
   } mState;

   struct
   {
      bool mHasBindVertexBuffers;
   } mCaps;

   std::unordered_map<BufferHandle, GLBuffer> mBuffers;
   int mBufferHandleCounter = 0;

   std::unordered_map<PipelineHandle, GLPipeline> mPipelines;
   int mPipelineHandleCounter = 0;

   std::unordered_map<StateBlockHandle, GLRasterizerState> mRasterizerStates;
   int mRasterizerHandleCounter = 0;

   std::unordered_map<StateBlockHandle, GLDepthState> mDepthStates;
   int mDepthStateHandleCounter = 0;

   std::unordered_map<StateBlockHandle, GLStencilState> mStencilState;
   int mStencilStateHandleCounter = 0;

   std::unordered_map<StateBlockHandle, GLBlendState> mBlendState;
   int mBlendStateHandleCounter = 0;

public:
   virtual BufferHandle createBuffer(const GFXBufferDesc& desc) override;
   virtual void deleteBuffer(BufferHandle handle) override;

   virtual PipelineHandle createPipeline(const GFXPipelineDesc& desc) override;
   virtual void deletePipeline(PipelineHandle handle) override;

   virtual void* mapBuffer(BufferHandle handle) override;
   virtual void unmapBuffer(BufferHandle handle) override;

   virtual void executeCmdBuffers(const GFXCmdBuffer** cmdBuffers, int count) override;

private:
   GLenum _getBufferUsage(BufferUsageEnum usage) const;
   GLenum _getBufferType(BufferType type) const;
   GLenum _getPrimitiveType(PrimitiveType primitiveType) const;
   GLenum _getShaderType(GFXShaderType shaderType) const;
   GLuint _createShaderProgram(const GFXShaderDesc* shader, uint32_t count);
};