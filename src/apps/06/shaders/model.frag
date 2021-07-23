in vec3 fPOSITION;
in vec3 fNORMAL;
in vec2 fUV;
layout(location = 0) out vec4 color;

layout(std140) uniform SunBuffer {
   vec4 sun_dir;
   vec4 sun_color;
   vec4 ambient_color;
} sun;

uniform sampler2D diffuseTexture;

void main() 
{
   float nL = clamp(dot(fNORMAL, vec3(sun.sun_dir)), 0.0, 1.0);
   color = texture(diffuseTexture, fUV) * sun.sun_color * nL + sun.ambient_color;
}