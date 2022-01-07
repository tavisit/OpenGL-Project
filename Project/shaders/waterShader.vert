#version 430 core
#extension GL_EXT_gpu_shader4: enable

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

out vec3 normal;
out vec4 fPosEye;
out vec4 fPosLightSpace;
out vec2 fTexCoords;

out vec3 fPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceTrMatrix;
uniform float time;

out float shininessFromDirectional;
out float shininessFromPoint;
out float shininessFromSpot;

int LFSR_Rand_Gen(in int n)
{
  // <<, ^ and & require GL_EXT_gpu_shader4.
  n = (n << 13) ^ n; 
  return (n * (n*n*15731+789221) + 1376312589) & 0x7fffffff;
}

float LFSR_Rand_Gen_f( in int n )
{
  return float(LFSR_Rand_Gen(n));
}

float noise3f(in vec3 p)
{
  ivec3 ip = ivec3(floor(p));
  vec3 u = fract(p);
  u = u*u*(3.0-2.0*u);

  int n = ip.x + ip.y*57 + ip.z*113;

  float res = mix(mix(mix(LFSR_Rand_Gen_f(n+(0+57*0+113*0)),
                          LFSR_Rand_Gen_f(n+(1+57*0+113*0)),u.x),
                      mix(LFSR_Rand_Gen_f(n+(0+57*1+113*0)),
                          LFSR_Rand_Gen_f(n+(1+57*1+113*0)),u.x),u.y),
                  mix(mix(LFSR_Rand_Gen_f(n+(0+57*0+113*1)),
                          LFSR_Rand_Gen_f(n+(1+57*0+113*1)),u.x),
                      mix(LFSR_Rand_Gen_f(n+(0+57*1+113*1)),
                          LFSR_Rand_Gen_f(n+(1+57*1+113*1)),u.x),u.y),u.z);

  return 1.0 - res*(1.0/1073741824.0);
}
void main() 
{
	vec2 resolution = vec2(1024,1024);
	vec2 cPos = -1.0 + 2.0 * vPosition.xy / resolution.xy;
	float cLength = length(cPos);
	vec2 uv = vPosition.xy/resolution.xy+(cPos/cLength)*cos(cLength*12.0-time*4.0) * 0.05;
	vec3 newPosition = vPosition;
	float disp = noise3f(newPosition*time);
	newPosition.y = newPosition.y  + sin(disp + uv.y)/50.0f;

	//compute eye space coordinates
	fPosEye = view * model * vec4(newPosition, 1.0f);
	normal = transpose(inverse(mat3(model))) * vNormal ;
	fPos = vec3(model* vec4(newPosition,1.0f));
	fTexCoords = vTexCoords;
	fPosLightSpace = lightSpaceTrMatrix * model * vec4(newPosition, 1.0f);
	gl_Position = projection * view * model * vec4(newPosition, 1.0f);

	shininessFromDirectional = 16.0f;
	shininessFromPoint = 8.0f;
	shininessFromSpot = 4.0f;
}
