#pragma once

#include <stdint.h>
#include <glm/glm.hpp>

struct GLFWwindow;

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
   
protected:
   virtual void onInit() = 0;
   virtual void onDestroy() = 0;
   virtual void onUpdate(double dt) = 0;
};
