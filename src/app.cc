#include <GLFW/glfw3.h>
#include "app.h"

const int DEFAULT_WIDTH = 1440;
const int DEFAULT_HEIGHT = 900;
const char* DEFAULT_TITLE = "Application";

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
   
   state.lastTimeStamp = glfwGetTime();
   state.timeFrequency = glfwGetTimerFrequency();

   glfwGetCursorPos(state.window, &state.lastMouseX, &state.lastMouseY);
   
   state.cursorIsLocked = false;
   glfwSetInputMode(state.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
   
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
      case (int)Key::ESCAPE:
         return glfwGetKey(state.window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
      case (int)Key::FORWARD:
         return glfwGetKey(state.window, GLFW_KEY_UP) == GLFW_PRESS ||
                glfwGetKey(state.window, GLFW_KEY_W) == GLFW_PRESS;
      case (int)Key::BACKWARDS:
         return glfwGetKey(state.window, GLFW_KEY_DOWN) == GLFW_PRESS ||
                glfwGetKey(state.window, GLFW_KEY_S) == GLFW_PRESS;
      case (int)Key::LEFT:
         return glfwGetKey(state.window, GLFW_KEY_LEFT) == GLFW_PRESS ||
                glfwGetKey(state.window, GLFW_KEY_A) == GLFW_PRESS;
      case (int)Key::RIGHT:
         return glfwGetKey(state.window, GLFW_KEY_RIGHT) == GLFW_PRESS ||
                glfwGetKey(state.window, GLFW_KEY_D) == GLFW_PRESS;
   }
   
   return false;
}
