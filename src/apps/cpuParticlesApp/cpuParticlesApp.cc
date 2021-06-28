#include <stdio.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <imgui.h>
#include "apps/cpuParticlesApp/cpuParticlesApp.h"

IMPLEMENT_APPLICATION(CpuParticlesApp);

const float VIEW_DISTANCE = 500.0f;

constexpr GLchar* vertShader =
#ifdef __APPLE__
"#version 410\n"
#else
"#version 430\n"
#endif
"layout(location = 0) in vec3 pos;\n"
"layout(location = 1) in vec4 color;\n"
"\n"
"out vec4 fCOLOR;\n"
"\n"
"layout(std140) uniform CameraBuffer {\n"
"   mat4 proj;\n"
"   mat4 view;\n"
"} camera;\n"
"\n"
"uniform mat4 modelMatrix;\n"
"\n"
"void main() {\n"
"   fCOLOR = color;\n"
"   gl_PointSize = 5.0;\n"
"   gl_Position = camera.proj * camera.view * modelMatrix * vec4(pos, 1.0);\n"
"}";

constexpr GLchar* fragShader =
#ifdef __APPLE__
"#version 410\n"
#else
"#version 430\n"
#endif
"in vec4 fCOLOR;\n"
"layout(location = 0) out vec4 color;\n"
"\n"
"void main() {\n"
"   color = fCOLOR;\n"
"}";


void CpuParticlesApp::onInit()
{
   camera.setPosition(glm::vec3(1.8f, 2.0f, 1.85f));
   camera.setYawPitch(-2.34f, -0.58f);
   updatePerspectiveMatrix();

   getWindowSize(windowWidth, windowHeight);
   toggleCursorLock();
   setWindowTitle("Cpu Particle App");

   initParticles();

   initGL();
}

void CpuParticlesApp::onDestroy()
{
   destroyGL();
}

void CpuParticlesApp::onUpdate(double dt)
{
   if (isKeyPressed(Key::ESCAPE))
      toggleCursorLock();

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
   p.velocity = glm::ballRand(5.0);
   p.lifeTime = 0.0;
   p.color = glm::vec4(1.0f);
}

void CpuParticlesApp::simulateParticles(double dt)
{
   float deltaSeconds = (float)(dt / 1000.0f);

   for (int i = 0; i < PARTICLE_COUNT; ++i)
   {
      Particle& p = particles[i];
      p.pos += p.velocity * deltaSeconds;
      p.lifeTime += dt;

      if (p.lifeTime > PARTICLE_TIME_MS)
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
   Move move;
   if (isKeyPressed(Key::FORWARD))   move.y += 1.0f;
   if (isKeyPressed(Key::BACKWARDS)) move.y -= 1.0f;
   if (isKeyPressed(Key::RIGHT))     move.x += 1.0f;
   if (isKeyPressed(Key::LEFT))      move.x -= 1.0f;

   glm::vec2 mouse = getMouseDelta();
   move.pitch = mouse.y;
   move.yaw = mouse.x;

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
   GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vShader, 1, &vertShader, NULL);
   glCompileShader(vShader);
   validateShaderCompilation(vShader);

   GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fShader, 1, &fragShader, NULL);
   glCompileShader(fShader);
   validateShaderCompilation(fShader);

   shaderProgram = glCreateProgram();
   glAttachShader(shaderProgram, vShader);
   glAttachShader(shaderProgram, fShader);
   glLinkProgram(shaderProgram);
   glDetachShader(shaderProgram, vShader);
   glDetachShader(shaderProgram, fShader);
   glDeleteShader(vShader);
   glDeleteShader(fShader);
   validateShaderLinkCompilation(shaderProgram);

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
   glEnable(GL_CULL_FACE);
   glCullFace(GL_FRONT);
   glFrontFace(GL_CW);
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
   ImGui::Begin("Debug Information");
   ImGui::SetWindowSize(ImVec2(400, 120));
   ImGui::Text("Frame Rate: %.1f FPS", ImGui::GetIO().Framerate);
   ImGui::Separator();
   ImGui::Text("GL Renderer: %s", glGetString(GL_RENDERER));
   ImGui::Text("GL Vendor: %s", glGetString(GL_VENDOR));
   ImGui::Text("GL Version: %s", glGetString(GL_VERSION));
   ImGui::End();
   ImGui::Render();
}
