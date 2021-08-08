#pragma once

typedef unsigned int BufferHandle;
typedef unsigned int PipelineHandle;
typedef unsigned int StateBlockHandle;
typedef unsigned int TextureHandle;
typedef unsigned int SamplerHandle;
typedef unsigned int ResourceHandle;

enum class BufferUsageEnum
{
   STATIC_GPU_ONLY,
   DYNAMIC_CPU_TO_GPU,
};

enum class BufferType
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

enum BufferAccessFlags : uint32_t
{
   NONE
};

enum class PrimitiveType
{
   TRIANGLE_LIST,
   TRIANGLE_STRIP,
   POINT_LIST,
   LINE_LIST,
   LINE_STRIP
};

enum class InputLayoutSemanticName
{
   POSITION,
   NORMAL,
   TEXCOORD0,
   TEXCOORD1,
   TEXCOORD2,
   TEXCOORD3
};

enum class InputLayoutFormat
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

enum class InputLayoutDivisor
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

enum class FillMode
{
   SOLID,
   WIREFRAME
};

enum class CullMode
{
   CULL_NONE,
   CULL_BACK,
   CULL_FRONT
};

enum class WindingMode
{
   CLOCKWISE,
   COUNTER_CLOCKWISE
};

// devices

struct GFXBufferDesc
{
   BufferType type;
   size_t sizeInBytes;
   BufferUsageEnum usage;
   uint32_t accessFlags;
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

};

struct GFXDepthStencilStateDesc
{
   struct GFXStencilDescriptor
   {
      GFXCompareFunc stencilCompareOp;
      GFXStencilFunc stencilFailFunc;
      GFXStencilFunc depthPassFunc;
      GFXStencilFunc depthFailFunc;

      uint32_t stencilReadMask;
      uint32_t stencilWriteMask;
      uint32_t referenceValue;
   };

   GFXCompareFunc depthCompareFunc;
   bool enableDepthTest;
   bool enableDepthWrite;
   bool enableStencilTest;

   GFXStencilDescriptor frontFaceStencil;
   GFXStencilDescriptor backFaceStencil;
};

struct GFXRasterizerStateDesc
{
   WindingMode windingMode;
   FillMode fillMode;
   CullMode cullMode;
};

struct GFXInputLayoutElementDesc
{
   InputLayoutSemanticName semanticName;
   InputLayoutFormat type;
   InputLayoutDivisor divisor;
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
   PrimitiveType primitiveType;
};

