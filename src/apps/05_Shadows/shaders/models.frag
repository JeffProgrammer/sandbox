in vec3 fPOSITION;
in vec3 fNORMAL;
in vec4 fLIGHTVIEWPOSITION;

layout(location = 0) out vec4 color;

layout(std140) uniform SunBuffer 
{
   vec4 sunDir;
   vec4 sunColor;
   vec4 ambientColor;
};

uniform vec4 objectColor;

uniform sampler2DShadow shadowMap;

void main() 
{
   color = ambientColor;

   vec3 lightDir = -sunDir.xyz;

   vec3 projTexCoord;
   projTexCoord.x = fLIGHTVIEWPOSITION.x / fLIGHTVIEWPOSITION.w / 2.0 + 0.5;
   projTexCoord.y = -fLIGHTVIEWPOSITION.y / fLIGHTVIEWPOSITION.w / 2.0 + 0.5;
   projTexCoord.z = fLIGHTVIEWPOSITION.z / fLIGHTVIEWPOSITION.w;
   float depthValue = texture(shadowMap, projTexCoord);
   float lightDepthValue = fLIGHTVIEWPOSITION.z / fLIGHTVIEWPOSITION.w - 0.001;

   if (lightDepthValue < depthValue)
   {
      float lightIntensity = dot(fNORMAL, lightDir);
      lightIntensity = clamp(lightIntensity, 0.0, 1.0);

      color += objectColor * sunColor * lightIntensity;
   }
}
