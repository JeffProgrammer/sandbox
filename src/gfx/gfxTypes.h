#pragma once

typedef unsigned int BufferHandle;
typedef unsigned int PipelineHandle;
typedef unsigned int PipelineHandle;
typedef unsigned int StateBlockHandle;

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

enum class InputLayoutClassification
{
   PER_VERTEX,
   PER_INSTANCE
};

enum class GFXCompareFunc
{
   EQUAL,
   NEQUAL,
   LEQUAL,
   GEQUAL,
   LTEQUAL,
   GREQUAL,
   NEVER,
   ALWAYS
};

enum class GFXStencilFunc
{
   KEEP,
   ZERO,
   REPLACE,
   INCRSAT,
   DECRSAT,
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

struct GFXDepthStateDesc
{
   bool depthTest;
   bool depthWrite;
   GFXCompareFunc depthFunc;
};

struct GFXStencilStateDesc
{
   bool stencilTest;
   GFXStencilFunc stencilFailOp;
   GFXStencilFunc stencilZFailOp;
   GFXStencilFunc stencilPassOp;
   GFXCompareFunc stencilFunc;
   uint32_t stencilRef;
   uint32_t stencilMask;
   uint32_t stencilWriteMask;
};

struct GFXRasterizerStateDesc
{
   bool enableRasterizer;
   bool frontCounterClockwise;
   bool enableScissorTest;
   bool enableDepthClipTest;
   FillMode fillMode;
   CullMode cullMode;
   int depthBias;
   float depthBiasClamp;
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

struct GFXPipelineDesc
{
   GFXInputLayoutDesc inputLayout;
   PrimitiveType primitiveType;
};
