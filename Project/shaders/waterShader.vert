#version 430 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

out vec3 normal;
out vec4 fragPosEye;
out vec4 fragPosLightSpace;
out vec2 fragTexCoords;

out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceTrMatrix;
uniform float time;

void main() 
{
	vec2 resolution = vec2(1024,1024);
	vec2 cPos = -1.0 + 2.0 * vPosition.xy / resolution.xy;
	float cLength = length(cPos);
	vec2 uv = vPosition.xy/resolution.xy+(cPos/cLength)*cos(cLength*12.0-time*4.0) * 0.05;

	vec3 newPosition = vPosition;
	newPosition.y = newPosition.y  + clamp((time/100.0f)*newPosition.x*newPosition.z+uv.y,-0.3f,0.07f);
	//compute eye space coordinates
	fragPosEye = view * model * vec4(newPosition, 1.0f);
	normal = transpose(inverse(mat3(model))) * vNormal ;
	fragPos = vec3(model* vec4(newPosition,1.0f));
	fragTexCoords = vTexCoords;
	fragPosLightSpace = lightSpaceTrMatrix * model * vec4(newPosition, 1.0f);
	gl_Position = projection * view * model * vec4(newPosition, 1.0f);
}
