in vec3 fPOSITION;
in vec3 fNORMAL;
layout(location = 0) out vec4 color;

layout(std140) uniform SunBuffer 
{
   vec4 sunDir;
   vec4 sunColor;
   vec4 ambientColor;
};

uniform vec4 objectColor;

void main() 
{
   vec3 normal = normalize(fNORMAL);

   float angle = dot(normal, normalize(sunDir.xyz));
   angle = max(angle, 0.0);

   color = vec4(1.0, 0.0, 0.0, 1.0);//(sunColor * angle) + (ambientColor * objectColor);
}