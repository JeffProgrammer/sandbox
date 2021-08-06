#pragma once

#include <unordered_map>
#include <glad/glad.h>
#include "gfx/gfxDevice.h"
#include "gfx/gfxCmdBuffer.h"

class GFXGLDevice : public GFXDevice
{
   friend class GFXGLCmdBuffer;

   enum
   {
      PUSH_CONSTANT_STRIDE = 16
   };

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
      bool enableFaceCulling;
      GLenum cullMode;
      GLenum windingOrder;
      GLenum polygonFillMode;
   };

   struct GLDepthStencilState
   {
      struct GLStencilState
      {
         GLenum stencilCompareOp;
         GLenum stencilFailFunc;
         GLenum depthPassFunc;
         GLenum depthFailFunc;

         uint32_t stencilReadMask; // when do we use this???
         uint32_t stencilWriteMask;
         uint32_t referenceValue;
      };

      GLenum depthCompareFunc;
      bool enableDepthTest;
      bool enableDepthWrite;
      bool enableStencilTest;

      GLStencilState frontFaceStencil;
      GLStencilState backFaceStencil;
   };

   struct GLBlendState
   {

   };

   struct
   {
      GLuint primitiveType = 0;
      GLuint currentProgram = 0;
      GLenum indexBufferType = 0;
      GLuint pushConstantLocation = 0;
      GLuint currentMappedBuffer = 0;
      GLenum currentMappedBufferType = 0;
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

   std::unordered_map<StateBlockHandle, GLDepthStencilState> mDepthStencilStates;
   int mDepthStencilStateHandleCounter = 0;

   std::unordered_map<StateBlockHandle, GLBlendState> mBlendState;
   int mBlendStateHandleCounter = 0;

public:
   virtual BufferHandle createBuffer(const GFXBufferDesc& desc) override;
   virtual void deleteBuffer(BufferHandle handle) override;

   virtual PipelineHandle createPipeline(const GFXPipelineDesc& desc) override;
   virtual void deletePipeline(PipelineHandle handle) override;

   virtual StateBlockHandle createRasterizerState(const GFXRasterizerStateDesc& desc) override;
   virtual StateBlockHandle createDepthStencilState(const GFXDepthStencilStateDesc& desc) override;
   virtual StateBlockHandle createBlendState(const GFXBlendStateDesc& desc) override;
   virtual void deleteStateBlock(StateBlockHandle handle) override;

   virtual void* mapBuffer(BufferHandle handle, uint32_t offset, uint32_t size) override;
   virtual void unmapBuffer(BufferHandle handle) override;

   virtual void executeCmdBuffers(const GFXCmdBuffer** cmdBuffers, int count) override;

private:
   GLenum _getBufferUsage(BufferUsageEnum usage) const;
   GLenum _getBufferType(BufferType type) const;
   GLenum _getPrimitiveType(PrimitiveType primitiveType) const;
   GLenum _getStencilFunc(GFXStencilFunc func) const;
   GLenum _getCompareFunc(GFXCompareFunc func) const;
   GLenum _getShaderType(GFXShaderType shaderType) const;
   GLenum _getInputLayoutType(InputLayoutFormat format) const;
   GLuint _createShaderProgram(const GFXShaderDesc* shader, uint32_t count);
};