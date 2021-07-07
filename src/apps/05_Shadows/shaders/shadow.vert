layout(location = 0) in vec3 pos;

layout(std140) uniform CameraBuffer 
{
   mat4 proj;
   mat4 view;
   mat4 shadowProjView;
};

uniform mat4 modelMatrix;

void main() 
{
   mat4 mvp = proj * view * modelMatrix;
   gl_Position = mvp * vec4(pos, 1.0);
}