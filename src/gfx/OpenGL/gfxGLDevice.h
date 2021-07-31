#pragma once

#include <unordered_map>
#include <glad/glad.h>
#include "gfx/gfxDevice.h"

class GFXGLDevice : public GFXDevice
{
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

   struct
   {
      GLuint primitiveType;
      GLuint currentProgram;
   } mState;

   struct
   {
      bool mHasBindVertexBuffers;
   } mCaps;

   std::unordered_map<BufferHandle, GLBuffer> mBuffers;
   int mBufferHandleCounter = 0;

   std::unordered_map<PipelineHandle, GLPipeline> mPipelines;
   int mPipelineHandleCounter = 0;

   std::unordered_map<StateBlockHandle, RasterizerState> mRasterizerStates;

public:
   virtual BufferHandle createBuffer(const GFXBufferDesc& desc) override;
   virtual void deleteBuffer(BufferHandle handle) override;

   virtual PipelineHandle createPipeline(const GFXPipelineDesc& desc) override;
   virtual void deletePipeline(PipelineHandle handle) override;

   virtual void* mapBuffer(BufferHandle handle) override;
   virtual void unmapBuffer(BufferHandle handle) override;

private:
   GLenum _getBufferUsage(BufferUsageEnum usage) const;
   GLenum _getBufferType(BufferType type) const;
   GLenum _getPrimitiveType(PrimitiveType primitiveType) const;
};