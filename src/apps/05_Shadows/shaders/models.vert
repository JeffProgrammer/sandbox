layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

out vec3 fPOSITION;
out vec3 fNORMAL;
out vec4 fTEXCOORD0;

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

   mat3 inverseModel = mat3(inverse(transpose(modelMatrix)));

   fNORMAL = (inverseModel * normal);
   fPOSITION = vec3(modelMatrix * vec4(pos, 1.0));
   fTEXCOORD0 = shadowProjView * vec4(pos, 1.0);

   gl_Position = mvp * vec4(pos, 1.0);
}