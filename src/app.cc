#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "app.h"
#include "apps/main/mainApp.h"

const int DEFAULT_WIDTH =1920;
const int DEFAULT_HEIGHT = 1080;
const char* DEFAULT_TITLE = "Application";

#if !defined(NDEBUG) && !defined(__APPLE__)
#define OPENGL_DEBUG
#endif

#ifdef __APPLE__
#define SHADER_VERSION "#version 410\n"
#else
#define SHADER_VERSION "#version 430\n"
#endif

extern Application* gApplication;

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
   glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
#else
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifdef OPENGL_DEBUG
   glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
#endif
   
   memset(&state, 0, sizeof(state));
   state.window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_TITLE, NULL, NULL);
   glfwMakeContextCurrent(state.window);
   glfwSwapInterval(0);

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
   
   ImGui::CreateContext();
   ImGui::StyleColorsDark();
   ImGui_ImplGlfw_InitForOpenGL(state.window, true); // true installs callbacks automatically
   ImGui_ImplOpenGL3_Init(SHADER_VERSION);
   
   onInit();
}

void Application::destroy()
{
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImGui::DestroyContext();
   
   onDestroy();
   
   glfwDestroyWindow(state.window);
   glfwTerminate();
}

bool Application::update()
{
   if (glfwWindowShouldClose(state.window))
   {
      // If we're main we're done.
      if (dynamic_cast<MainApplication*>(this))
      {
         return false;
      }
      
      // Go ahead and switch back to main
      destroy();
      delete gApplication;
      gApplication = new MainApplication;
      gApplication->init();
      return true;
   }
   
   if (state.isQueued)
   {
      // switch apps
      destroy();
      Application* app = state.queuedApp;
      delete gApplication;
      gApplication = app;
      gApplication->init();

      return true;
   }
 
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
   
   ImGui_ImplOpenGL3_NewFrame();
   ImGui_ImplGlfw_NewFrame();
   onRenderImGUI(deltaInMilliseconds);
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
   
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

void Application::setVerticalSync(bool enabled)
{
   state.vsyncEnabled = enabled;
   glfwSwapInterval(enabled ? 1 : 0);
}

void Application::queueAppSwitch(const std::string &app)
{
   state.queuedApp = ApplicationRep::create(app);
   state.isQueued = true;
}

#pragma warning(push)
#pragma warning(disable: 6387) // buffer possibly empty
#pragma warning(disable: 4996) // unsafe functions
char* Application::readShaderFile(const char* fileName) const
{
   FILE* file = fopen(fileName, "r");
   if (!file)
      abort();

   static size_t shaderLen = strlen(SHADER_VERSION);

   fseek(file, 0, SEEK_END);
   long fileLen = ftell(file);
   fseek(file, 0, SEEK_SET);


   char* buffer = (char*)calloc(fileLen + shaderLen, sizeof(char));
   strncpy(buffer, SHADER_VERSION, shaderLen);
   fread(buffer + shaderLen, sizeof(char), fileLen, file);
      
   fclose(file);

   return buffer;
}
#pragma warning(pop)
