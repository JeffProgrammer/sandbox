#include <stdio.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <imgui.h>
#include "apps/02_Cpu_Particles/cpuParticlesApp.h"
#include "gl/shader.h"

IMPLEMENT_APPLICATION(CpuParticlesApp);

const float VIEW_DISTANCE = 500.0f;

void CpuParticlesApp::onInit()
{
   camera.setPosition(glm::vec3(1.8f, 2.0f, 1.85f));
   camera.setYawPitch(-2.34f, -0.58f);
   updatePerspectiveMatrix();

   getWindowSize(windowWidth, windowHeight);
   setWindowTitle("Cpu Particle App");

   vsync = false;
   freeze = false;

   initParticles();

   initGL();
}

void CpuParticlesApp::onDestroy()
{
   destroyGL();
}

void CpuParticlesApp::onUpdate(double dt)
{
   updateCamera(dt);

   simulateParticles(dt);
   copyParticlesToGLBuffer();

   render(dt);
}

void CpuParticlesApp::onWindowSizeUpdate(int width, int height)
{
   windowWidth = width;
   windowHeight = height;
}

void CpuParticlesApp::initParticles()
{
   for (int i = 0; i < PARTICLE_COUNT; ++i)
   {
      resetParticle(particles[i]);
   }
}

void CpuParticlesApp::resetParticle(Particle& p)
{
   p.pos = glm::vec3(0);
   p.velocity = glm::ballRand(3.0);
   p.lifeTime = 0.0;
   p.color = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
   p.lifeTimeMax = (PARTICLE_TIME_MAX_MS - 1000) - ((rand() % 10 + 1) * 100);
}

void CpuParticlesApp::simulateParticles(double dt)
{
   if (freeze)
      return;

   float deltaSeconds = (float)(dt / 1000.0f);

   for (int i = 0; i < PARTICLE_COUNT; ++i)
   {
      Particle& p = particles[i];
      p.pos += p.velocity * deltaSeconds;
      p.lifeTime += (float)dt;

      if (p.lifeTime >  p.lifeTimeMax)
         resetParticle(p);
   }
}

void CpuParticlesApp::copyParticlesToGLBuffer()
{
   for (int i = 0; i < PARTICLE_COUNT; ++i)
   {
      glParticles[i].pos = particles[i].pos;
      glParticles[i].color = particles[i].color;
   }
}

void CpuParticlesApp::updateCamera(double dt)
{
   Move move = {};
   camera.update(dt, move);
   updatePerspectiveMatrix();
}

void CpuParticlesApp::updatePerspectiveMatrix()
{
   camera.setProjectionMatrix(glm::perspective((float)glm::radians(110.0), getAspectRatio(), 0.01f, VIEW_DISTANCE));

   camera.getMatrices(cameraData.projMatrix, cameraData.viewMatrix);
}

void CpuParticlesApp::initGL()
{
   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);

   glGenBuffers(1, &particleVbo);
   glBindBuffer(GL_ARRAY_BUFFER, particleVbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(glParticles), NULL, GL_STREAM_DRAW);

   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLParticle), (void*)offsetof(GLParticle, pos));
   glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GLParticle), (void*)offsetof(GLParticle, color));

   initShader();
   initUBOs();
}

void CpuParticlesApp::initUBOs()
{
   glGenBuffers(1, &cameraUbo);
   glBindBuffer(GL_UNIFORM_BUFFER, cameraUbo);
   glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraUbo), NULL, GL_DYNAMIC_DRAW);
}

void CpuParticlesApp::initShader()
{
   char* vertShader = readShaderFile("apps/02_Cpu_Particles/shaders/particles.vert");
   char* fragShader = readShaderFile("apps/02_Cpu_Particles/shaders/particles.frag");

   shaderProgram = createVertexAndFragmentShaderProgram(vertShader, fragShader);

   free(vertShader);
   free(fragShader);

   uniformModelMatLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
   uniformCameraLocationBlock = glGetUniformBlockIndex(shaderProgram, "CameraBuffer");

   cameraUboLocation = 0;

   glUniformBlockBinding(shaderProgram, uniformCameraLocationBlock, cameraUboLocation);
}

void CpuParticlesApp::destroyGL()
{
   glUseProgram(0);
   glDeleteProgram(shaderProgram);

   GLuint deleteBuffers[2] = { particleVbo, cameraUbo };
   glDeleteBuffers(2, deleteBuffers);

   glBindVertexArray(0);
   glDeleteVertexArrays(1, &vao);
}

void CpuParticlesApp::render(double dt)
{
   glViewport(0, 0, windowWidth, windowHeight);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glClearColor(0.0, 0.0, 0.0, 1.0);

   glEnable(GL_DEPTH_TEST);
   glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

   glUseProgram(shaderProgram);

   // Update camera data
   glBindBuffer(GL_UNIFORM_BUFFER, cameraUbo);
   glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraUbo), &cameraData);

   glm::mat4 identMat = glm::mat4(1.0f);

   // Draw Particles
   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, particleVbo);

   // Update particles
   glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glParticles), &glParticles[0]);

   glBindBufferBase(GL_UNIFORM_BUFFER, cameraUboLocation, cameraUbo);
   glUniformMatrix4fv(uniformModelMatLocation, 1, GL_FALSE, (const GLfloat*)&identMat[0]);

   glDrawArrays(GL_POINTS, 0, PARTICLE_COUNT);

   glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
}

void CpuParticlesApp::onRenderImGUI(double dt)
{
   ImGui::NewFrame();
   
   ImGui::Begin("Debug Information & Options");
   ImGui::SetWindowSize(ImVec2(700, 180));
   ImGui::Text("Frame Rate: %.1f FPS", ImGui::GetIO().Framerate);
   ImGui::Separator();

   ImGui::Text("OpenGL Driver Information:");
   ImGui::Text("   Renderer: %s", glGetString(GL_RENDERER));
   ImGui::Text("   Vendor: %s", glGetString(GL_VENDOR));
   ImGui::Text("   Version: %s", glGetString(GL_VERSION));

   ImGui::Separator();

   if (ImGui::Checkbox("Enable Vsync", &vsync))
   {
      setVerticalSync(vsync);
   }

   ImGui::Checkbox("Freeze Simulation", &freeze);

   ImGui::End();

   ImGui::Render();
}
