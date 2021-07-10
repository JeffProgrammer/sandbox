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
uniform sampler2D shadowMap;

void main() 
{
   vec3 coords = fTEXCOORD0.xyz / fTEXCOORD0.w;
   coords = coords * 0.5 + 0.5;
   
   float bias = 0.005;
	float distanceToLight = texture(shadowMap, coords.xy).r - bias;
   
   float depth = coords.z;
   
	float lightIntensity = 1.0;
	if (distanceToLight < depth) {
		lightIntensity = 1.0 - 0.3;
	}

   vec3 normal = normalize(fNORMAL);

   float angle = dot(normal, normalize(sunDir.xyz));
   angle = clamp(angle, 0.0, 1.0);

   //color = vec4(normal, 1);

   //color = objectColor;

   //color = vec4(vec3(angle), 1);

   //vec4 diffuse = objectColor * (sunColor * angle);

   //color = diffuse * lightIntensity + ambientColor;

   color = vec4(vec3(distanceToLight), 1.0);

   //color = vec4(fTEXCOORD0.x, fTEXCOORD0.y, 0.0f, 1.0f);
   
   //color = vec4(coords, 1.0);
}
