#define CUBE_COUNT 64

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

out vec3 fPOSITION;
out vec3 fNORMAL;
flat out int fINSTANCE_ID;

layout(std140) uniform CameraBuffer {
   mat4 proj;
   mat4 view;
} camera;

layout(std140) uniform CubeInstanceBuffer {
    mat4 modelMatrix[CUBE_COUNT];
} buff;

void main() {
   mat4 modelMatrix = buff.modelMatrix[gl_InstanceID];

   fNORMAL = normal;
   fPOSITION = vec3(modelMatrix * vec4(pos, 1.0));
   fINSTANCE_ID = gl_InstanceID;

   gl_Position = camera.proj * camera.view * modelMatrix * vec4(pos, 1.0);
}