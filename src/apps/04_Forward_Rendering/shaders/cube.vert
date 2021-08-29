#define CUBE_COUNT 512

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

out vec3 fPOSITION;
out vec3 fNORMAL;

layout(std140, binding = 0) uniform CameraBuffer 
{
   mat4 proj;
   mat4 view;
};

layout(std140, binding = 2) uniform CubeInstanceBuffer 
{
    mat4 modelMatrix[CUBE_COUNT];
};

void main() 
{
   mat4 modelMat = modelMatrix[gl_InstanceID];
   mat4 mvp = proj * view * modelMat;

   mat3 inverseModel = mat3(transpose(inverse(modelMat)));

   fNORMAL = (inverseModel * normal);
   fPOSITION = vec3(modelMat * vec4(pos, 1.0));

   gl_Position = mvp * vec4(pos, 1.0);
}