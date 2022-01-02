#version 430 core

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

void main() 
{
	//compute eye space coordinates
	fPosEye = view * model * vec4(vPosition, 1.0f);
	normal = vNormal;
	fPos = vec3(model* vec4(vPosition,1.0f));
	fTexCoords = vTexCoords;
	fPosLightSpace = lightSpaceTrMatrix * model * vec4(vPosition, 1.0f);
	gl_Position = projection * view * model * vec4(vPosition, 1.0f);
}
