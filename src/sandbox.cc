#include <stdio.h>
#include <glm/glm.hpp>
#include <glad/glad.h>

#include "app.h"

class SandboxApplication : public Application
{
   virtual void onInit() override
   {
      
   }
   
   virtual void onDestroy() override
   {
      
   }
   
   virtual void onUpdate(double dt) override
   {
      if (isKeyPressed(Key::ESCAPE))
         toggleCursorLock();
      
      render(dt);
   }
   
   void render(double dt)
   {
      glClear(GL_COLOR_BUFFER_BIT);
      glClearColor(0.0, 0.0, 0.0, 1.0);
   }
};
