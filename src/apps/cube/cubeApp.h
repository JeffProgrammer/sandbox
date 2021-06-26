#include "app.h"
#include "core/camera.h"

struct CameraUbo
{
   glm::mat4 viewMatrix;
   glm::mat4 projMatrix;
};

struct SunUbo
{
   glm::vec4 sunDir;
   glm::vec4 sunColor;
   glm::vec4 ambientColor;
};

class CubeApplication : public Application
{
public:
   DECLARE_APPLICATION(CubeApplication);

   virtual void onWindowSizeUpdate(int width, int height) override;

   virtual void onInit() override;
   virtual void onDestroy() override;
   virtual void onUpdate(double dt) override;

   void updateCamera(double dt);
   void updatePerspectiveMatrix();
   void initGL();
   void initUBOs();
   void initShader();
   void destroyGL();
   void render(double dt);

private:
   Camera camera;
   CameraUbo cameraData;
   SunUbo sunData;

   int windowWidth;
   int windowHeight;

   GLuint shaderProgram;

   GLuint cubeVbo;
   GLuint cubeIbo;
   GLuint vao;
   GLuint cameraUbo;
   GLuint sunUbo;

   GLuint uniformModelMatLocation;
   GLuint uniformSunLocationBlock;
   GLuint uniformCameraLocationBlock;
};