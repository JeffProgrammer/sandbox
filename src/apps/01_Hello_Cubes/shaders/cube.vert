layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

out vec3 fNORMAL;

layout(std140, binding = 0) uniform CameraBuffer 
{
   mat4 proj;
   mat4 view;
} camera;

void main() 
{
   fNORMAL = normal;
   gl_Position = camera.proj * camera.view * mat4(1.0) * vec4(pos, 1.0);
}