#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "app.h"

const int DEFAULT_WIDTH = 1440;
const int DEFAULT_HEIGHT = 900;
const char* DEFAULT_TITLE = "Application";

static void windowCallback(GLFWwindow* window, int width, int height)
{
   Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
   app->onWindowSizeUpdate(width, height);
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
#endif
   
   state.window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_TITLE, NULL, NULL);
   glfwMakeContextCurrent(state.window);

   if (!gladLoadGL())
   {
      abort();
   }
   
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