#include <stdio.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "app.h"
#include "core/camera.h"

const float VIEW_DISTANCE = 500.0f;

const float cubeVertsBuffer[] = {
   1,0,1, 1,0,0,   1,1,1, 1,0,0,   1,1,0, 1,0,0,   1,0,0, 1,0,0,  // east
   1,1,1, 0,1,0,   0,1,1, 0,1,0,   0,1,0, 0,1,0,   1,1,0, 0,1,0,  // up
   0,1,1, -1,0,0,  0,0,1, -1,0,0,  0,0,0, -1,0,0,  0,1,0, -1,0,0, // west
   0,0,1, 0,-1,0,  1,0,1, 0,-1,0,  1,0,0, 0,-1,0,  0,0,0, 0,-1,0, // down
   0,1,1, 0,0,1,   1,1,1, 0,0,1,   1,0,1, 0,0,1,   0,0,1, 0,0,1, // north
   0,0,0, 0,0,-1,  1,0,0, 0,0,-1,  1,1,0, 0,0,-1,  0,1,0, 0,0,-1, // south
};

const short cubeIndices[] = {
   0, 2, 1, 0, 3, 2,
   4, 6, 5, 4, 7, 6,
   8, 10, 9, 8, 11, 10,
   12, 14, 13, 12, 15, 14,
   16, 18, 17, 16, 19, 18,
   20, 22, 21, 20, 23, 22
};

constexpr GLchar* vertShader =
#ifdef __APPLE__
"#version 410\n"
#else
"#version 430 core\n"
#endif
"layout(location = 0) in vec3 pos;\n"
"layout(location = 1) in vec3 normal;\n"
"\n"
"out vec3 fNORMAL;\n"
"\n"
"layout(std140) uniform CameraBuffer {\n"
"   mat4 proj;\n"
"   mat4 view;\n"
"} camera;\n"
"\n"
"uniform mat4 modelMatrix;\n"
"\n"
"void main() {\n"
"   fNORMAL = normal;\n"
"   gl_Position = camera.proj * camera.view * modelMatrix * vec4(pos, 1.0);\n"
"}";

constexpr GLchar* fragShader =
#ifdef __APPLE__
"#version 410\n"
#else
"#version 430\n"
#endif
"in vec3 fNORMAL;\n"
"loyout(location = 0) out vec4 color;\n"
"\n"
"layout(std140) uniform SunBuffer {\n"
"   vec4 sun_dir;\n"
"   vec4 sun_color;\n"
"   vec4 ambient_color;\n"
"} light;\n"
"\n"
"void main() {\n"
"   float nL = clamp(dot(fNORMAL, light.sun_dir), 0.0, 1.0);\n"
"   color = light.sun_color * nL + light.ambient_color;\n"
"}";

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

class SandboxApplication : public Application
{
   virtual void onInit() override
   {
      camera.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
      camera.setYawPitch(0.0f, -0.45f);
      updatePerspectiveMatrix();

      getWindowSize(windowWidth, windowHeight);

      sunData.sunDir = glm::vec4(0.32f, 0.75f, 0.54f, 0.0f);
      sunData.sunColor = glm::vec4(1.4f, 1.2f, 0.4f, 0.0f);
      sunData.ambientColor = glm::vec4(0.3f, 0.3f, 0.4f, 0.0f);

      initGL();
   }
   
   virtual void onDestroy() override
   {
      destroyGL();
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

   void updatePerspectiveMatrix()
   {
      camera.setProjectionMatrix(glm::perspective(1.5708f, getAspectRatio(), 0.01f, VIEW_DISTANCE));

      camera.getMatrices(cameraData.projMatrix, cameraData.viewMatrix);
   }

   void initGL()
   {
      glGenVertexArrays(1, &vao);
      glBindVertexArray(vao);

      glGenBuffers(1, &cubeVbo);
      glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertsBuffer), cubeVertsBuffer, GL_STATIC_DRAW);

      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(3*sizeof(float)));

      glGenBuffers(1, &cubeIbo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIbo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

      initShader();
      initUBOs();

      //glEnable(GL_CULL_FACE);
      //glCullFace(GL_BACK);
      //glFrontFace(GL_CW);
   }

   void initUBOs()
   {
      glGenBuffers(1, &cameraUbo);
      glBindBuffer(GL_UNIFORM_BUFFER, cameraUbo);
      glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraUbo), NULL, GL_DYNAMIC_DRAW);

      glGenBuffers(1, &sunUbo);
      glBindBuffer(GL_UNIFORM_BUFFER, sunUbo);
      glBufferData(GL_UNIFORM_BUFFER, sizeof(SunUbo), &sunData, GL_STATIC_DRAW);
   }

   void initShader()
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
      glDeleteShader(vShader);
      glDeleteShader(fShader);
      validateShaderLinkCompilation(shaderProgram);

      uniformModelMatLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
      uniformSunLocationBlock = glGetUniformBlockIndex(shaderProgram, "SunBuffer");
      uniformCameraLocationBlock = glGetUniformBlockIndex(shaderProgram, "CameraBuffer");
   }

   void destroyGL()
   {
      GLuint deleteBuffers[4] = { cubeVbo, cubeIbo, cameraUbo, sunUbo };
      glDeleteBuffers(4, deleteBuffers);

      glBindVertexArray(0);
      glDeleteVertexArrays(1, &vao);
   }

   void render(double dt)
   {
      glViewport(0, 0, windowWidth, windowHeight);
      glClear(GL_COLOR_BUFFER_BIT);
      glClearColor(0.0, 0.0, 0.0, 1.0);

      glUseProgram(shaderProgram);

      // Update camera data
      glBindBuffer(GL_UNIFORM_BUFFER, cameraUbo);
      glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraUbo), &cameraData);

      glm::mat4 cubeModelMat = glm::mat4(1.0f);

      // Draw Cube
      glBindVertexArray(vao);
      glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIbo);

      glBindBufferBase(GL_UNIFORM_BUFFER, uniformCameraLocationBlock, cameraUbo);
      glBindBufferBase(GL_UNIFORM_BUFFER, uniformSunLocationBlock, sunUbo);
      glUniformMatrix4fv(uniformModelMatLocation, 1, GL_FALSE, (const GLfloat*)&cubeModelMat[0]);

      glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, NULL);
   }

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
