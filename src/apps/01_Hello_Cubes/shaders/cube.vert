layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

out vec3 fNORMAL;

layout(std140) uniform CameraBuffer {
   mat4 proj;
   mat4 view;
} camera;

uniform mat4 modelMatrix;

void main() {
   fNORMAL = normal;
   gl_Position = camera.proj * camera.view * modelMatrix * vec4(pos, 1.0);
}