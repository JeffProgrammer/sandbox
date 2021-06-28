#include "app.h"
#include "core/camera.h"

struct CameraUbo
{
   glm::mat4 projMatrix;
   glm::mat4 viewMatrix;
};

struct Particle
{
   glm::vec3 pos;
   glm::vec3 velocity;
   glm::vec4 color;
   float lifeTime;
};

struct GLParticle
{
   glm::vec3 pos;
   glm::vec4 color;
};

#define PARTICLE_COUNT (int)10000
#define PARTICLE_TIME_MS (float)3000

class CpuParticlesApp : public Application
{
public:
   DECLARE_APPLICATION(CpuParticlesApp);

   virtual void onWindowSizeUpdate(int width, int height) override;

   virtual void onInit() override;
   virtual void onDestroy() override;
   virtual void onUpdate(double dt) override;
   virtual void onRenderImGUI(double dt) override;

   void initParticles();

   void updateCamera(double dt);
   void updatePerspectiveMatrix();
   void initGL();
   void initUBOs();
   void initShader();
   void destroyGL();
   void render(double dt);

   void resetParticle(Particle& p);
   void simulateParticles(double dt);
   void copyParticlesToGLBuffer();

private:
   Camera camera;
   CameraUbo cameraData;

   GLParticle glParticles[PARTICLE_COUNT];
   Particle particles[PARTICLE_COUNT];

   int windowWidth;
   int windowHeight;

   GLuint shaderProgram;

   GLuint particleVbo;
   GLuint vao;
   GLuint cameraUbo;

   GLuint uniformCameraLocationBlock;

   GLuint cameraUboLocation;
   GLuint uniformModelMatLocation;
};
