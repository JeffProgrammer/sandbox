in vec3 fNORMAL;
layout(location = 0) out vec4 color;

layout(std140) uniform SunBuffer {
   vec4 sun_dir;
   vec4 sun_color;
   vec4 ambient_color;
} light;

void main() {
   float nL = clamp(dot(fNORMAL, vec3(light.sun_dir)), 0.0, 1.0);
   color = light.sun_color * nL + light.ambient_color;
}