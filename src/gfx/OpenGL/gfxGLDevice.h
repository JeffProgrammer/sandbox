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
      GFXBufferUsageEnum usage;
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

   struct GLTexture
   {
      GLuint texture;
      GLenum type;
      GLenum internalFormat;
      int32_t width;
      int32_t height;
      int32_t levels;
   };

   struct
   {
      GLuint primitiveType = 0;
      GLuint currentProgram = 0;
      GLenum indexBufferType = 0;
      GLuint pushConstantLocation = 0;
      GLuint currentMappedBuffer = 0;
      GLenum currentMappedBufferType = 0;
      GLuint globalVAO = 0;
   } mState;

   struct GLSampler
   {
      GLuint handle;
   };

   struct GLRenderPass
   {
      struct GLColorRenderTarget
      {
         GLuint textureId;
         GFXLoadAttachmentAction loadAction;
         GLenum attachment;
         float clearColor[4];
      };

      struct GLDepthRenderTarget
      {
         GLuint textureId;
         GFXLoadAttachmentAction loadAction;
         float clearDepth;
      };
      
      struct GLStencilRenderTarget
      {
         GLuint textureId;
         GFXLoadAttachmentAction loadAction;
         int32_t clearStencil;
      };

      GLColorRenderTarget colorTargets[8];
      GLDepthRenderTarget depthTarget;
      GLStencilRenderTarget stencilTarget;

      GLuint drawBuffers[8] = {};
      GLuint numColorAttachments = 0;
      GLuint fbo = 0;
      bool enableDepthAttachment = false;
      bool enableStencilAttachment = false;
   };

   struct
   {
      bool hasMultiBind = true;
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

   std::unordered_map<SamplerHandle, GLSampler> mSamplers;
   int mSamplerHandleCounter = 0;

   std::unordered_map<RenderPassHandle, GLRenderPass> mRenderPasses;
   int mRenderPassHandleCounter = 0;

   std::unordered_map<TextureHandle, GLTexture> mTextures;
   int mTextureHandleCounter = 0;

public:
   GFXGLDevice();
   virtual ~GFXGLDevice();

   virtual BufferHandle createBuffer(const GFXBufferDesc& desc) override;
   virtual void deleteBuffer(BufferHandle handle) override;

   virtual PipelineHandle createPipeline(const GFXPipelineDesc& desc) override;
   virtual void deletePipeline(PipelineHandle handle) override;

   virtual RenderPassHandle createRenderPass(const GFXRenderPassDesc& desc) override;
   virtual void deleteRenderPass(RenderPassHandle handle) override;

   virtual StateBlockHandle createRasterizerState(const GFXRasterizerStateDesc& desc) override;
   virtual StateBlockHandle createDepthStencilState(const GFXDepthStencilStateDesc& desc) override;
   virtual StateBlockHandle createBlendState(const GFXBlendStateDesc& desc) override;
   virtual void deleteStateBlock(StateBlockHandle handle) override;

   virtual SamplerHandle createSampler(const GFXSamplerStateDesc& desc) override;
   virtual void deleteSampler(SamplerHandle handle) override;

   virtual TextureHandle createTexture(const GFXTextureStateDesc& desc) override;
   virtual void deleteTexture(TextureHandle handle) override;

   virtual void* mapBuffer(BufferHandle handle, uint32_t offset, uint32_t size) override;
   virtual void unmapBuffer(BufferHandle handle) override;

   virtual void executeCmdBuffers(const GFXCmdBuffer** cmdBuffers, int count) override;

   virtual void present(RenderPassHandle handle, int width, int height);
private:
   GLenum _getBufferUsage(GFXBufferUsageEnum usage) const;
   GLenum _getBufferType(GFXBufferType type) const;
   GLenum _getPrimitiveType(GFXPrimitiveType primitiveType) const;
   GLenum _getStencilFunc(GFXStencilFunc func) const;
   GLenum _getCompareFunc(GFXCompareFunc func) const;
   GLenum _getShaderType(GFXShaderType shaderType) const;
   GLenum _getInputLayoutType(GFXInputLayoutFormat format) const;
   GLuint _createShaderProgram(const GFXShaderDesc* shader, uint32_t count);

   GLenum _getSamplerWrapMode(GFXSamplerWrapMode mode) const;
   GLenum _getSamplerMagFilterMode(GFXSamplerMagFilterMode mode) const;
   GLenum _getSamplerMinFilteRMode(GFXSamplerMinFilterMode mode) const;
   GLenum _getSamplerCompareMode(GFXSamplerCompareMode mode) const;

   GLenum _getTextureType(GFXTextureType mode) const;
   GLenum _getTextureInternalFormat(GFXTextureInternalFormat format) const;
};