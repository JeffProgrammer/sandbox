#define LIGHT_COUNT 512

in vec3 fPOSITION;
in vec3 fNORMAL;
layout(location = 0) out vec4 color;

vec3 ambient_color = vec3(0.3, 0.3, 0.3);

struct Light 
{
   vec4 position;
   vec4 attenuation;
   vec4 color;
};

layout(std140) uniform LightBuffer 
{
   Light lights[LIGHT_COUNT];
   int lightCount;
   int pad[3];
};

float calcAttenuationFactor(float constant, float linear, float quadratic, float lightLen) 
{
   return 1.0;// / (constant + linear * lightLen + quadratic * (lightLen * lightLen));
}

vec4 computePointLight(Light light, vec3 position, vec3 normal)
{
   vec3 lightVec = (light.position.xyz - position);
   float lightLen = length(lightVec);
   float radius = light.position.w;

   vec4 result = vec4(0.0);
   if (lightLen < radius)
   {
      float angle = dot(normal, normalize(lightVec));
      angle = clamp(angle, 0.0, 1.0);

      float attenuation = calcAttenuationFactor(light.attenuation.x, light.attenuation.y, light.attenuation.z, lightLen);

      result = attenuation * vec4(light.color * angle);
   }

   return result; 
}

void main() 
{
   vec3 normal = normalize(fNORMAL);

   vec4 lightColor = vec4(0.0);

   for (int i = 0; i < lightCount; i++) 
   {
      Light light = lights[i];
      lightColor += computePointLight(light, fPOSITION, normal);
   }

   color = vec4(lightColor.xyz + ambient_color, 1.0);
   //color = vec4(1.0, 0.0, 0.0, 0.0); //vec4(normal, 1.0);
}