#pragma once

typedef unsigned int BufferHandle;
typedef unsigned int PipelineHandle;
typedef unsigned int StateBlockHandle;
typedef unsigned int TextureHandle;
typedef unsigned int SamplerHandle;
typedef unsigned int ResourceHandle;
typedef unsigned int RenderPassHandle;

enum class GFXApi
{
   OpenGL,
   Metal
};

enum
{
   MAX_COLOR_ATTACHMENTS = 8
};

enum class GFXBufferUsageEnum
{
   STATIC_GPU_ONLY,
   DYNAMIC_CPU_TO_GPU,
};

enum class GFXBufferType
{
   VERTEX_BUFFER,
   INDEX_BUFFER,
   CONSTANT_BUFFER
};

enum class GFXIndexBufferType
{
   BITS_16,
   BITS_32
};

enum GFXBufferAccessFlags : uint32_t
{
   NONE
};

enum class GFXPrimitiveType
{
   TRIANGLE_LIST,
   TRIANGLE_STRIP,
   POINT_LIST,
   LINE_LIST,
   LINE_STRIP
};

enum class GFXInputLayoutFormat
{
   FLOAT,
   BYTE,
   SHORT,
   INT
};

enum class GFXShaderType
{
   VERTEX,
   FRAGMENT
};

enum GFXShaderStageBit
{
   VERTEX_BIT = 1,
   FRAGMENT_BIT = 1 << VERTEX_BIT
};

enum class GFXInputLayoutDivisor
{
   PER_VERTEX,
   PER_INSTANCE
};

enum class GFXCompareFunc
{
   EQUAL,
   NEQUAL,
   LESS,
   GREATER,
   LEQUAL,
   GEQUAL,
   NEVER,
   ALWAYS
};

enum class GFXStencilFunc
{
   KEEP,
   ZERO,
   REPLACE,
   INCR_WRAP,
   DECR_WRAP,
   INVERT,
   INCR,
   DECR
};

enum class GFXSamplerWrapMode
{
   CLAMP_TO_EDGE,
   MIRRORED_REPEAT,
   REPEAT
};

enum class GFXSamplerMagFilterMode
{
   LINEAR,
   NEAREST
};

enum class GFXSamplerMinFilterMode
{
   LINEAR,
   NEAREST,
   NEAREST_MIP,
   NEAREST_MIP_WEIGHTED,
   LINEAR_MIP,
   LINEAR_MIP_WEIGHTED
};

enum class GFXSamplerCompareMode
{
   REFERENCE_TO_TEXTURE,
   NONE
};

enum class GFXFillMode
{
   SOLID,
   WIREFRAME
};

enum class GFXCullMode
{
   CULL_NONE,
   CULL_BACK,
   CULL_FRONT
};

enum class GFXWindingMode
{
   CLOCKWISE,
   COUNTER_CLOCKWISE
};

enum class GFXTextureType
{
   TEXTURE_1D,
   TEXTURE_2D,
   TEXTURE_3D,
   TEXTURE_CUBEMAP
};

enum class GFXTextureInternalFormat
{
   RGBA8,
   DEPTH_16
};

// devices

struct GFXBufferDesc
{
   GFXBufferType type;
   size_t sizeInBytes;
   GFXBufferUsageEnum usage;
   //uint32_t accessFlags;
   void* data;
};

struct GFXBlendStateDesc
{

};

// Uses OpenGL defaults
struct GFXSamplerStateDesc
{
   GFXSamplerMinFilterMode minFilterMode = GFXSamplerMinFilterMode::NEAREST_MIP_WEIGHTED;
   GFXSamplerMagFilterMode magFilterMode = GFXSamplerMagFilterMode::LINEAR;
   int32_t minLOD = -1000;
   int32_t maxLOD = 1000;
   GFXSamplerWrapMode wrapS = GFXSamplerWrapMode::REPEAT;
   GFXSamplerWrapMode wrapT = GFXSamplerWrapMode::REPEAT;
   GFXSamplerWrapMode wrapR = GFXSamplerWrapMode::REPEAT;
   float borderColorR = 0.0f;
   float borderColorG = 0.0f;
   float borderColorB = 0.0f;
   float borderColorA = 0.0f;
   GFXSamplerCompareMode compareMode = GFXSamplerCompareMode::NONE;
   GFXCompareFunc compareFunc = GFXCompareFunc::ALWAYS;
};

struct GFXTextureStateDesc
{
   GFXTextureType type;
   GFXTextureInternalFormat internalFormat;
   int32_t levels;
   int32_t width;
   int32_t height;
};

struct GFXDepthStencilStateDesc
{
   struct GFXStencilDescriptor
   {
      GFXCompareFunc stencilCompareOp = GFXCompareFunc::ALWAYS;
      GFXStencilFunc stencilFailFunc = GFXStencilFunc::KEEP;
      GFXStencilFunc depthPassFunc = GFXStencilFunc::KEEP;
      GFXStencilFunc depthFailFunc = GFXStencilFunc::KEEP;

      uint32_t stencilReadMask = 0xFF;
      uint32_t stencilWriteMask = 0xFF;
      uint32_t referenceValue = 0;
   };

   GFXCompareFunc depthCompareFunc = GFXCompareFunc::LESS;
   bool enableDepthTest = false;
   bool enableDepthWrite = false;

   bool enableStencilTest = false;
   GFXStencilDescriptor frontFaceStencil;
   GFXStencilDescriptor backFaceStencil;
};

struct GFXRasterizerStateDesc
{
   GFXWindingMode windingMode = GFXWindingMode::COUNTER_CLOCKWISE;
   GFXFillMode fillMode = GFXFillMode::SOLID;
   GFXCullMode cullMode = GFXCullMode::CULL_NONE;
   bool enableDynamicPointSize = false; // DX11/12 doesn't have this
};

struct GFXInputLayoutElementDesc
{
   uint32_t slot;
   GFXInputLayoutFormat type;
   GFXInputLayoutDivisor divisor;
   uint32_t bufferBinding;
   uint32_t offset;
   uint32_t count;
};

struct GFXInputLayoutDesc
{
   const GFXInputLayoutElementDesc* descs;
   uint32_t count;
};

struct GFXShaderDesc
{
   GFXShaderType type;
   const char* code;
   uint32_t codeLength;
};

struct GFXPipelineDesc
{
   GFXShaderDesc* shadersStages;
   uint32_t shaderStageCount;
   GFXInputLayoutDesc inputLayout;
   GFXPrimitiveType primitiveType;
};

// TODO(Jeff):
// FOR NOW not used in GL, for Metal...
// GL can probably eventually use them with glInvalidateFramebuffer
enum class GFXStoreAttachmentAction
{
   DONT_CARE, // render to render buffer
   PRESERVE_TO_TEXTURE // render to texture
};

enum class GFXLoadAttachmentAction
{
   DONT_CARE, // don't clear, just write new pixels
   CLEAR, // clear it with a default value
};

struct GFXColorRenderPassAttachment
{
   TextureHandle texture;
   //GFXStoreAttachmentAction storeAction;
   GFXLoadAttachmentAction loadAction;
   float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
};

struct GFXDepthRenderPassAttachment
{
   TextureHandle texture;
   //GFXStoreAttachmentAction storeAction;
   GFXLoadAttachmentAction loadAction;
   float clearDepth = 1.0;
};

struct GFXStencilRenderPassAttachment
{
   TextureHandle texture;
   //GFXStoreAttachmentAction storeAction;
   GFXLoadAttachmentAction loadAction;
   int32_t clearStencil = 0;
};

struct GFXRenderPassDesc
{
   GFXDepthRenderPassAttachment depthAttachment = {};
   GFXStencilRenderPassAttachment stencilAttachment = {};
   GFXColorRenderPassAttachment colorAttachments[MAX_COLOR_ATTACHMENTS];
   uint32_t colorAttachmentCount = 0;
   bool depthAttachmentEnabled = false;
   bool stencilAttachmentEnabled = false;
};