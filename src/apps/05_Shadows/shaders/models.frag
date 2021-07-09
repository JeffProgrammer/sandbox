in vec3 fPOSITION;
in vec3 fNORMAL;
in vec4 fTEXCOORD0;

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
   vec3 normal = normalize(fNORMAL);

   float angle = dot(normal, -normalize(sunDir.xyz));
   angle = clamp(angle, 0.0, 1.0);

   float visibility = texture(shadowMap, vec3(fTEXCOORD0.xy, fTEXCOORD0.z/fTEXCOORD0.w));

   color = vec4(visibility * vec3(objectColor), 1.0); //(sunColor * objectColor * angle) + ambientColor;
}
