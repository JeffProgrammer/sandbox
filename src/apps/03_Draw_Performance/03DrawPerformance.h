#include "app.h"
#include "core/camera.h"

struct CameraUbo
{
   glm::mat4 projMatrix;
   glm::mat4 viewMatrix;
};

struct SunUbo
{
   glm::vec4 sunDir;
   glm::vec4 sunColor;
   glm::vec4 ambientColor;
};

struct CubeData
{
   glm::mat4 matrix;
};

class DrawPerformanceApplication : public Application
{
public:
   DECLARE_APPLICATION(DrawPerformanceApplication);

   virtual void onWindowSizeUpdate(int width, int height) override;

   virtual void onInit() override;
   virtual void onDestroy() override;
   virtual void onUpdate(double dt) override;
   virtual void onRenderImGUI(double dt) override;

   void updateCamera(double dt);
   void updatePerspectiveMatrix();
   void initGL();
   void initUBOs();
   void initShader();
   void destroyGL();
   void render(double dt);
   void createCubeData();

private:
   Camera camera;
   CameraUbo cameraData;
   SunUbo sunData;

   int windowWidth;
   int windowHeight;

   GLuint shaderProgram;

   GLuint vbo;
   GLuint ibo;
   GLuint vao;
   GLuint cameraUbo;
   GLuint sunUbo;

   GLuint uniformModelMatLocation;
   GLuint uniformSunLocationBlock;
   GLuint uniformCameraLocationBlock;

   GLuint cameraUboLocation;
   GLuint sunUboLocation;

   // 1 drawcall per cube, gridSize*gridSize
   int gridSize;

   CubeData* cubeData;
};