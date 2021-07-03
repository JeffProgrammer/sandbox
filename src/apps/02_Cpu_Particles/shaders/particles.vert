layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 color;

out vec4 fCOLOR;

layout(std140) uniform CameraBuffer {
   mat4 proj;
   mat4 view;
} camera;

uniform mat4 modelMatrix;

void main() {
   fCOLOR = color;
   gl_PointSize = 2.0;
   gl_Position = camera.proj * camera.view * modelMatrix * vec4(pos, 1.0);
}