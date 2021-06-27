#pragma once

#include <stdint.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

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
      Application *queuedApp;
      bool isQueued = false;
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

   void setWindowTitle(const char* title);
   
   bool isKeyPressed(Key key) const;

   float getAspectRatio() const;

   void getWindowSize(int& width, int& height) const;
   
   void validateShaderCompilation(GLuint shader);

   void validateShaderLinkCompilation(GLuint program);

   virtual void onWindowSizeUpdate(int width, int height) = 0;
   
   void queueAppSwitch(const std::string &app);

protected:
   virtual void onInit() = 0;
   virtual void onDestroy() = 0;
   virtual void onUpdate(double dt) = 0;
   virtual void onRenderImGUI(double dt) = 0;
};

class ApplicationRep
{
public:
   static ApplicationRep* sLast;
   ApplicationRep* mNext;
   std::string mName;

   virtual Application* create() = 0;
   
   static Application* create(const std::string &searchName)
   {
      for (ApplicationRep* rep = sLast; rep != nullptr; rep = rep->mNext)
      {
         if (searchName == rep->mName)
            return rep->create();
      }

      return nullptr;
   }
   
   static std::vector<ApplicationRep*> getListOfApplications()
   {
      std::vector<ApplicationRep *> apps;

      for (ApplicationRep* rep = sLast; rep != nullptr; rep = rep->mNext)
         apps.push_back(rep);

      return apps;
   }
};

template<typename T>
class ConcreteApplicationRep : public ApplicationRep
{
public:
   ConcreteApplicationRep(const std::string &name)
   {
      mNext = sLast;
      sLast = this;
      mName = name;
   }

   virtual Application* create() override
   {
      return static_cast<Application*>(new T());
   }
};

#define DECLARE_APPLICATION(klass) \
   static ConcreteApplicationRep<klass> sConcreteClassRep;

#define IMPLEMENT_APPLICATION(klass) \
   ConcreteApplicationRep<klass> klass::sConcreteClassRep(#klass)

#ifndef NDEBUG
#define GL_CHECKERRORS() checkErrors(__FILE__, __LINE__)
#else
#define GL_CHECKERRORS()
#endif

void checkErrors(const char* fileName, int lineNumber);
