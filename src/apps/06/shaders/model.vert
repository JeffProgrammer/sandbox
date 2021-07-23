layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

out vec3 fPOSITION;
out vec3 fNORMAL;
out vec2 fUV;

layout(std140) uniform CameraBuffer 
{
   mat4 proj;
   mat4 view;
};

void main() 
{
   mat4 modelMat = mat4(1.0);
   mat4 mvp = proj * view * modelMat;

   mat3 inverseModel = mat3(transpose(inverse(modelMat)));

   fNORMAL = (inverseModel * normal);
   fPOSITION = vec3(modelMat * vec4(pos, 1.0));
   fUV = uv;

   gl_Position = mvp * vec4(pos, 1.0);
}