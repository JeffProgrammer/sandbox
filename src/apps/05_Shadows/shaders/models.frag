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
	float distanceToLight = texture(shadowMap, fTEXCOORD0.xy).r;
	float lightIntensity = 1.0;
	if (fTEXCOORD0.z > distanceToLight) {
		lightIntensity = 1.0 - 0.8;
	}

   vec3 normal = normalize(fNORMAL);

   float angle = dot(normal, normalize(sunDir.xyz));
   angle = clamp(angle, 0.0, 1.0);

   //color = vec4(normal, 1);

   //color = objectColor;

   //color = vec4(vec3(angle), 1);

   vec4 diffuse = objectColor * (sunColor * angle);

   color = diffuse * lightIntensity + ambientColor;
}
