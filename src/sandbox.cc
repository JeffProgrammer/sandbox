#include <stdio.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "app.h"
#include "core/camera.h"

const float VIEW_DISTANCE = 500.0f;

class SandboxApplication : public Application
{
   virtual void onInit() override
   {
      camera.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
      camera.setYawPitch(0.0f, -0.45f);
      updatePerspectiveMatrix();

      getWindowSize(windowWidth, windowHeight);
   }
   
   virtual void onDestroy() override
   {
      
   }
   
   virtual void onUpdate(double dt) override
   {
      if (isKeyPressed(Key::ESCAPE))
         toggleCursorLock();
      
      updateCamera(dt);
      render(dt);
   }
   
   virtual void onWindowSizeUpdate(int width, int height) override
   {
      windowWidth = width;
      windowHeight = height;
   }

   void updateCamera(double dt)
   {
      Move move;
      if (isKeyPressed(Key::FORWARD))   move.y += 1.0f;
      if (isKeyPressed(Key::BACKWARDS)) move.y -= 1.0f;
      if (isKeyPressed(Key::RIGHT))     move.x += 1.0f;
      if (isKeyPressed(Key::LEFT))      move.x -= 1.0f;

      updatePerspectiveMatrix();
      camera.update(dt, move);
   }

   void render(double dt)
   {
      glViewport(0, 0, windowWidth, windowHeight);
      glClear(GL_COLOR_BUFFER_BIT);
      glClearColor(0.0, 0.0, 0.0, 1.0);
   }

   void updatePerspectiveMatrix()
   {
      camera.setProjectionMatrix(glm::perspective(1.5708f, getAspectRatio(), 0.01f, VIEW_DISTANCE));
   }

   Camera camera;
   int windowWidth;
   int windowHeight;
};
