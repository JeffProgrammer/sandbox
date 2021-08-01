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
      glDeleteBuffers(1, &found->second.handle);
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
   pipelineState.shader = _createShaderProgram(desc.shaderStages, desc.shaderStageCount); 

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

void* GFXGLDevice::mapBuffer(BufferHandle handle)
{
   return nullptr;
}

void GFXGLDevice::unmapBuffer(BufferHandle handle)
{
   //const auto& found = mBuffers.find(handle);
   //glUnmapBuffer(found->second.type);
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

   return 0;
}

GLenum GFXGLDevice::_getShaderType(GFXShaderType type) const
{
   switch (type)
   {
   case ShaderType::VERTEX:
      return GL_VERTEX_SHADER;
   case ShaderType::FRAGMENT:
      return GL_FRAGMENT_SHADER;
   }

   return 0;
}

GLuint _createShaderProgram(const GFXShaderDesc* shader, uint32_t count)
{
   std::vector<GLuint> glHandles;

   for (uint32_t i = 0; i < count; i++)
   {
      const GFXShaderDesc& shaderStage = shader[i];
      GLenum shaderType = _getShaderType(shaderStage.type);

      GLuint handle = glCreateShader(shaderType);
      glShaderSource(handle, 1, &shader.code, NULL);
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