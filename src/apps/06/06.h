#pragma once

#include "app.h"
#include "core/camera.h"
#include "resources/modelObj.h"

struct CameraUbo
{
   glm::mat4 projMatrix;
   glm::mat4 viewMatrix;
};

struct SceneShape
{
   int count;
   int vboOffset;
   int indexOffset;
};

struct SceneMaterialShapeGroup
{
   GLuint diffuseTexture;
   GLuint bumpTexture;
   GLuint specularTexture;
   GLuint shaderProgram;
   std::vector<SceneShape> shapes;
};

struct MaterialBin
{
   std::vector<SceneMaterialShapeGroup> materialGroup;
   GLuint shaderProgram;
};

struct SceneGraph
{
   GLuint vbo;
   GLuint ibo;
   std::vector<MaterialBin> bin;
};

class A06Application : public Application
{
public:
   DECLARE_APPLICATION(A06Application);

   virtual void onWindowSizeUpdate(int width, int height) override;

   virtual void onInit() override;
   virtual void onDestroy() override;
   virtual void onUpdate(double dt) override;
   virtual void onRenderImGUI(double dt) override;

   void updateCamera(double dt);
   void updatePerspectiveMatrix();
   void initGL();
   void initScene();
   void initUBOs();
   void initShader();
   void destroyGL();
   void render(double dt);

private:
   Model sponza;
   SceneGraph scene;

   Camera camera;

   CameraUbo cameraData;

   int windowWidth;
   int windowHeight;

   GLuint shaderProgram;

   GLuint vbo;
   GLuint ibo;
   GLuint vao;
   GLuint cameraUbo;

   GLuint uniformCameraLocation;

   GLuint cameraUboLocation;
};