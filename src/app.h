#pragma once

#include <stdint.h>
#include <glm/glm.hpp>

struct GLFWwindow;

typedef uint32_t GLuint;

class Application
{
private:
   struct
   {
      GLFWwindow *window;
      uint64_t timeFrequency;
      double lastTimeStamp;
      double lastMouseX;
      double lastMouseY;
      double currentMouseX;
      double currentMouseY;
      bool cursorIsLocked;
   } state;
   
public:
   enum Key : int
   {
      FORWARD = 0,
      BACKWARDS,
      LEFT,
      RIGHT,
      ESCAPE
   };
   
   void init();
   
   void destroy();
   
   bool update();
   
   bool supportsComputeShaders();

   glm::vec2 getMouseDelta() const;

   void toggleCursorLock();
   
   bool isKeyPressed(Key key) const;

   float getAspectRatio() const;

   void getWindowSize(int& width, int& height) const;
   
   void validateShaderCompilation(GLuint shader);

   void validateShaderLinkCompilation(GLuint program);

   virtual void onWindowSizeUpdate(int width, int height) = 0;

protected:
   virtual void onInit() = 0;
   virtual void onDestroy() = 0;
   virtual void onUpdate(double dt) = 0;
};
