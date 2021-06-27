#include <stdio.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "apps/cube/cubeApp.h"

IMPLEMENT_APPLICATION(CubeApplication);

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
"#version 430\n"
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
"layout(location = 0) out vec4 color;\n"
"\n"
"layout(std140) uniform SunBuffer {\n"
"   vec4 sun_dir;\n"
"   vec4 sun_color;\n"
"   vec4 ambient_color;\n"
"} light;\n"
"\n"
"void main() {\n"
"   float nL = clamp(dot(fNORMAL, vec3(light.sun_dir)), 0.0, 1.0);\n"
"   color = light.sun_color * nL + light.ambient_color;\n"
"}";

   void CubeApplication::onInit()
   {
      camera.setPosition(glm::vec3(1.8f, 2.0f, 1.85f));
      camera.setYawPitch(-2.34f, -0.58f);
      updatePerspectiveMatrix();

      getWindowSize(windowWidth, windowHeight);
      toggleCursorLock();

      sunData.sunDir = glm::vec4(0.32f, 0.75f, 0.54f, 0.0f);
      sunData.sunColor = glm::vec4(1.4f, 1.2f, 0.4f, 0.0f);
      sunData.ambientColor = glm::vec4(0.3f, 0.3f, 0.4f, 0.0f);

      initGL();
   }
   
   void CubeApplication::onDestroy()
   {
      destroyGL();
   }
   
   void CubeApplication::onUpdate(double dt)
   {
      if (isKeyPressed(Key::ESCAPE))
         toggleCursorLock();
      
      updateCamera(dt);
      render(dt);
   }
   
   void CubeApplication::onWindowSizeUpdate(int width, int height)
   {
      windowWidth = width;
      windowHeight = height;
   }

   void CubeApplication::updateCamera(double dt)
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

   void CubeApplication::updatePerspectiveMatrix()
   {
      camera.setProjectionMatrix(glm::perspective((float)glm::radians(110.0), getAspectRatio(), 0.01f, VIEW_DISTANCE));

      camera.getMatrices(cameraData.projMatrix, cameraData.viewMatrix);
   }

   void CubeApplication::initGL()
   {
      printf("GL_RENDERER: %s\n", glGetString(GL_RENDERER));
      printf("GL_VENDOR: %s\n", glGetString(GL_VENDOR));
      printf("GL_VERSION: %s\n", glGetString(GL_VERSION));

      glGenVertexArrays(1, &vao);
      glBindVertexArray(vao);

      glGenBuffers(1, &cubeVbo);
      glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertsBuffer), cubeVertsBuffer, GL_STATIC_DRAW);

      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, NULL);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)12);

      glGenBuffers(1, &cubeIbo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIbo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

      initShader();
      initUBOs();
   }

   void CubeApplication::initUBOs()
   {
      glGenBuffers(1, &cameraUbo);
      glBindBuffer(GL_UNIFORM_BUFFER, cameraUbo);
      glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraUbo), NULL, GL_DYNAMIC_DRAW);

      glGenBuffers(1, &sunUbo);
      glBindBuffer(GL_UNIFORM_BUFFER, sunUbo);
      glBufferData(GL_UNIFORM_BUFFER, sizeof(SunUbo), &sunData, GL_STATIC_DRAW);
   }

   void CubeApplication::initShader()
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
      uniformSunLocationBlock = glGetUniformBlockIndex(shaderProgram, "SunBuffer");
      uniformCameraLocationBlock = glGetUniformBlockIndex(shaderProgram, "CameraBuffer");

      cameraUboLocation = 0;
      sunUboLocation = 1;

      glUniformBlockBinding(shaderProgram, uniformCameraLocationBlock, cameraUboLocation);
      glUniformBlockBinding(shaderProgram, uniformSunLocationBlock, sunUboLocation);
   }

   void CubeApplication::destroyGL()
   {
      glUseProgram(0);
      glDeleteProgram(shaderProgram);

      GLuint deleteBuffers[4] = { cubeVbo, cubeIbo, cameraUbo, sunUbo };
      glDeleteBuffers(4, deleteBuffers);

      glBindVertexArray(0);
      glDeleteVertexArrays(1, &vao);
   }

   void CubeApplication::render(double dt)
   {
      glViewport(0, 0, windowWidth, windowHeight);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glClearColor(0.0, 0.0, 0.0, 1.0);
      
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_CULL_FACE);
      glCullFace(GL_FRONT);
      glFrontFace(GL_CW);

      glUseProgram(shaderProgram);

      // Update camera data
      glBindBuffer(GL_UNIFORM_BUFFER, cameraUbo);
      glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraUbo), &cameraData);

      glm::mat4 cubeModelMat = glm::mat4(1.0f);

      // Draw Cube
      glBindVertexArray(vao);
      glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIbo);

      glBindBufferBase(GL_UNIFORM_BUFFER, cameraUboLocation, cameraUbo);
      glBindBufferBase(GL_UNIFORM_BUFFER, sunUboLocation, sunUbo);
      glUniformMatrix4fv(uniformModelMatLocation, 1, GL_FALSE, (const GLfloat*)&cubeModelMat[0]);

      glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, NULL);
   }
