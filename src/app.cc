#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "app.h"

const int DEFAULT_WIDTH = 1440;
const int DEFAULT_HEIGHT = 900;
const char* DEFAULT_TITLE = "Application";

#ifndef NDEBUG
#define OPENGL_DEBUG
#endif

ApplicationRep* ApplicationRep::sLast = nullptr;

static void windowCallback(GLFWwindow* window, int width, int height)
{
   Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
   app->onWindowSizeUpdate(width, height);
}

static void APIENTRY debugGLCallbackProc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userData)
{
   printf("OpenGL %s: Message: %s\n", type == GL_DEBUG_TYPE_ERROR ? "Error" : "Information", message);
}

void Application::init()
{
   glfwInit();
   
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

#ifdef __APPLE__
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifdef OPENGL_DEBUG
   glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
#endif
   
   state.window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_TITLE, NULL, NULL);
   glfwMakeContextCurrent(state.window);

   if (!gladLoadGL())
   {
      abort();
   }

#ifdef OPENGL_DEBUG
   glEnable(GL_DEBUG_OUTPUT);
   glDebugMessageCallback(debugGLCallbackProc, NULL);
#endif
   
   state.lastTimeStamp = glfwGetTime();
   state.timeFrequency = glfwGetTimerFrequency();

   glfwGetCursorPos(state.window, &state.lastMouseX, &state.lastMouseY);
   
   state.cursorIsLocked = false;
   glfwSetInputMode(state.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

   glfwSetWindowUserPointer(state.window, this);

   glfwSetWindowSizeCallback(state.window, windowCallback);
   
   onInit();
}

void Application::destroy()
{
   onDestroy();
   
   glfwDestroyWindow(state.window);
   glfwTerminate();
}

bool Application::update()
{
   if (glfwWindowShouldClose(state.window))
      return false;
 
   glfwPollEvents();
   
   // Update Time
   double currentTime = glfwGetTime();
   double deltaInMilliseconds = (currentTime - state.lastTimeStamp) * 1000;
   state.lastTimeStamp = currentTime;

   // Update Mouse Movement
   double currentX, currentY;
   glfwGetCursorPos(state.window, &currentX, &currentY);
   state.currentMouseX = currentX - state.lastMouseX;
   state.currentMouseY = currentY - state.lastMouseY;
   state.lastMouseX = currentX;
   state.lastMouseY = currentY;
   
   onUpdate(deltaInMilliseconds);
   
   glfwSwapBuffers(state.window);
   
   return true;
}

bool Application::supportsComputeShaders()
{
#ifdef __APPLE__
   return false;
#else
   return true;
#endif
}

glm::vec2 Application::getMouseDelta() const
{
    return glm::vec2(state.currentMouseX, state.currentMouseY);
}

void Application::toggleCursorLock()
{
   state.cursorIsLocked = !state.cursorIsLocked;
   glfwSetInputMode(state.window, GLFW_CURSOR, state.cursorIsLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void Application::setWindowTitle(const char* title)
{
   glfwSetWindowTitle(state.window, title);
}

bool Application::isKeyPressed(Key key) const
{
   switch (key)
   {
      case Key::ESCAPE:
         return glfwGetKey(state.window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
      case Key::FORWARD:
         return glfwGetKey(state.window, GLFW_KEY_UP) == GLFW_PRESS ||
                glfwGetKey(state.window, GLFW_KEY_W) == GLFW_PRESS;
      case Key::BACKWARDS:
         return glfwGetKey(state.window, GLFW_KEY_DOWN) == GLFW_PRESS ||
                glfwGetKey(state.window, GLFW_KEY_S) == GLFW_PRESS;
      case Key::LEFT:
         return glfwGetKey(state.window, GLFW_KEY_LEFT) == GLFW_PRESS ||
                glfwGetKey(state.window, GLFW_KEY_A) == GLFW_PRESS;
      case Key::RIGHT:
         return glfwGetKey(state.window, GLFW_KEY_RIGHT) == GLFW_PRESS ||
                glfwGetKey(state.window, GLFW_KEY_D) == GLFW_PRESS;
   }
   
   return false;
}

float Application::getAspectRatio() const
{
   int width, height;
   getWindowSize(width, height);
   return (float)width / (float)height;
}

void Application::getWindowSize(int& width, int& height) const
{
   glfwGetWindowSize(state.window, &width, &height);
}


void Application::validateShaderCompilation(GLuint shader)
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

void Application::validateShaderLinkCompilation(GLuint program)
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

void checkErrors(const char* fileName, int lineNumber)
{
   GLenum error = GL_NO_ERROR;
   while ((error = glGetError()) != GL_NO_ERROR)
   {
      printf("//--------------------------------------------------------\n");
      printf("OpenGL Error:\n");
      switch (error) {
      case 0x500: printf("Code: Invalid Enum\n"); break;
      case 0x501: printf("Code: Invalid Value\n"); break;
      case 0x502: printf("Code: Invalid Operation\n"); break;
      case 0x503: printf("Code: Stack Overflow\n"); break;
      case 0x504: printf("Code: Stack Underflow\n"); break;
      case 0x505: printf("Code: Out of Memory\n"); break;
      case 0x506: printf("Code: Invalid Framebuffer Operation\n"); break;
      default:    printf("Code: Unkown\n\n"); break;
      }
      printf("File: %s\n", fileName);
      printf("Line Number: %d\n", lineNumber);
      printf("//--------------------------------------------------------\n");
   }
}