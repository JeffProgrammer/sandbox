#include <stdio.h>
#include <stdlib.h>
#include "gl/shader.h"

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

GLuint createVertexAndFragmentShaderProgram(const char* vertSource, const char* fragSource)
{
   GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vShader, 1, &vertSource, NULL);
   glCompileShader(vShader);
   validateShaderCompilation(vShader);

   GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fShader, 1, &fragSource, NULL);
   glCompileShader(fShader);
   validateShaderCompilation(fShader);

   GLuint shaderProgram = glCreateProgram();
   glAttachShader(shaderProgram, vShader);
   glAttachShader(shaderProgram, fShader);
   glLinkProgram(shaderProgram);
   glDetachShader(shaderProgram, vShader);
   glDetachShader(shaderProgram, fShader);
   glDeleteShader(vShader);
   glDeleteShader(fShader);
   validateShaderLinkCompilation(shaderProgram);

   return shaderProgram;
}