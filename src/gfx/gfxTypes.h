#pragma once

typedef unsigned int BufferHandle;
typedef unsigned int PipelineHandle;
typedef unsigned int StateBlockHandle;
typedef unsigned int TextureHandle;

enum class BufferUsageEnum
{
   STATIC_DRAW,
   DYNAMIC_DRAW
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

enum BufferAccessFlags
{

   PERSISTENT_MAPPED
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
   FLOAT1,
   FLOAT2,
   FLOAT3,
   FLOAT4,
   UNSIGNED_BYTE,
   UNSIGNED_SHORT,
   UNSIGNED_INT
};

enum class GFXShaderType
{
   VERTEX,
   FRAGMENT
};

enum class InputLayoutClassification
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
   };

   GFXCompareFunc depthCompareFunc;
   bool enableDepthTest;

   GFXStencilDescriptor frontFaceStencil;
   GFXStencilDescriptor backFaceStencil;
};

struct GFXRasterizerStateDesc
{
   bool enableRasterizer;
   WindingMode windingMode;
   FillMode fillMode;
   CullMode cullMode;
};

struct GFXInputLayoutElementDesc
{
   InputLayoutSemanticName semanticName;
   InputLayoutFormat format;
   InputLayoutClassification classification;
   uint32_t slot;
   uint32_t offset;
   uint32_t stride;
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
