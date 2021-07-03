#define LIGHT_COUNT 64
#define CUBE_COUNT 64

in vec3 fPOSITION;
in vec3 fNORMAL;
in flat int fINSTANCE_ID;
layout(location = 0) out vec4 color;

vec4 ambient_color = vec4(0.3, 0.3, 0.3, 1.0);

struct Light {
   vec3 position;
   float radius;
   vec4 color;
};

layout(std140) uniform LightBuffer {
   uniform int lightCount;
   Light lights[LIGHT_COUNT];
};

layout(std140) uniform CubeInstanceBuffer {
    mat4 modelMatrix[CUBE_COUNT];
} buff;

void main() {
   mat3 normalMat = transpose(inverse(mat3(buff.modelMatrix[fINSTANCE_ID])));
   vec3 normal = normalize(normalMat * fNORMAL);
   float normalLen = length(normal);

   vec4 lightColor = vec4(0.0);

   for (int i = 0; i < lightCount; i++) {
      vec3 dist = vec3(lights[i].position) - fPOSITION;
      float lightLen = length(dist);

      float angle = dot(normal, dist) / lightLen * normalLen;
      angle = clamp(angle, 0.0, 1);

      float attenuation = 1.0 / (1.0 + lights[i].radius * (lightLen * lightLen));

      lightColor += ambient_color + attenuation * vec4(lights[i].color * angle);
   }

   color = lightColor + ambient_color;
}