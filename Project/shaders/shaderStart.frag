#version 430 core
in vec3 normal;
in vec4 fPosEye;
in vec4 fPosLightSpace;
in vec2 fTexCoords;

in vec3 fPos;

out vec4 fColor;

////////////////////////////////////////////////////////////
// General uniform variables

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

////////////////////////////////////////////////////////////
// directional light

float ambientDirectionalStrength = 0.4f;
float specularDirectionalStrength = 0.6f;
float shininessDirectional = 64.0f;

uniform	vec3 directionalLightColor;
uniform	vec3 directionalLightPosition;
uniform	float alpha;

////////////////////////////////////////////////////////////
// point light

struct POINT_LIGHT
{
    vec3 location;
    vec3 color;
    float intensity;
	// 3 locations
};
#define LIGHT_MAX 6
layout(location = 0) uniform POINT_LIGHT pointLights[LIGHT_MAX]; // locations [0, LIGHT_MAX*3)

float ambientPointStrength = 0.5f;
float specularPointStrength = 0.5f;
float shininessPoint = 32.0f;

float pointConstant = 1.0f;
float pointLinear = 0.0225f;
float pointQuadratic = 0.0375;

////////////////////////////////////////////////////////////
// spot light

float spotConstant = 1.0f;
float spotLinear = 0.1f;
float spotQuadratic = 0.02f;
vec3 spotLightColor = vec3(10,10,10);

float ambientSpotStrength = 0.5f;
float specularSpotStrength = 0.5f;
float shininessSpot = 32.0f;

float spotlightCutOff = cos(radians(10));
float spotlightOuterCutOff =  cos(radians(30));

#define SPOT_LIGHTS_MAX 1
struct SPOT_LIGHT
{
    int spotinit;
    vec3 spotLightDirection;
    vec3 spotLightPosition;
	// 3 locations
};
layout(location = 120) uniform SPOT_LIGHT spotLights[SPOT_LIGHTS_MAX]; // locations [120, SPOT_LIGHTS_MAX*3)

////////////////////////////////////////////////////////////
//General global variables

float gamma = 1.25;
vec3 viewDir;
vec3 directionalLightDir;
vec3 currentPointLightPos;
vec3 currentPointLightColor;
vec3 ambient;
vec3 diffuse;
vec3 specular;

////////////////////////////////////////////////////////////
// point light computation
vec3 CalcPointLight(vec3 currentPointLightPos,vec3 currentPointLightColor)
{
	vec3 pointLightDir = normalize(currentPointLightPos - fPos);
    vec3 lightDirTemp = normalize(pointLightDir);
	// Blinn-Phong shading model
	vec3 halfVector = normalize(lightDirTemp + viewDir);

	float diff = max(dot(normal, lightDirTemp), 0.0);
	float spec = pow(max(dot(viewDir, halfVector), 0.0), shininessPoint);
	float distance    = length(currentPointLightPos - fPos);
	float attenuation = pointConstant + pointLinear * distance + pointQuadratic * (distance * distance); 

	vec3 ambient = ambientPointStrength * currentPointLightColor ;
	vec3 diffuse = ambientPointStrength *diff * currentPointLightColor;
	vec3 specular = shininessPoint * spec * currentPointLightColor;
	ambient *= vec3(pow(texture(diffuseTexture, fTexCoords).rgb, vec3(1.0/gamma)));
	diffuse *= vec3(pow(texture(diffuseTexture, fTexCoords).rgb, vec3(1.0/gamma)));
	specular *= vec3(pow(texture(specularTexture, fTexCoords).rgb, vec3(1.0/gamma)));

	return (ambient + diffuse + specular) / attenuation;
}

////////////////////////////////////////////////////////////
// directinal light computation
vec3 CalcDirLight()
{
    vec3 lightDirTemp = normalize(directionalLightDir);
	// Blinn-Phong shading model
	vec3 halfVector = normalize(lightDirTemp + viewDir);
    float diff = max(dot(normal, lightDirTemp), 0.0);
    float spec = pow(max(dot(viewDir, halfVector), 0.0), shininessDirectional);

    // combine results
    ambient = ambientDirectionalStrength * directionalLightColor;
	diffuse = ambientDirectionalStrength * diff * directionalLightColor;
	specular = specularDirectionalStrength * spec * directionalLightColor;

	ambient *= vec3(pow(texture(diffuseTexture, fTexCoords).rgb, vec3(1.0/gamma)));
	diffuse *= vec3(pow(texture(diffuseTexture, fTexCoords).rgb, vec3(1.0/gamma)));
	specular *= vec3(pow(texture(specularTexture, fTexCoords).rgb, vec3(1.0/gamma)));

    return (ambient + diffuse + specular);
}  
////////////////////////////////////////////////////////////
// spot light computation
vec3 computeLightSpotComponents(vec3 spotLightPosition, vec3 spotLightDirection) {
	vec3 lightDirTemp = normalize(spotLightPosition - fPos);
	// Blinn-Phong shading model
	vec3 halfVector = normalize(lightDirTemp + viewDir);

	float diff = max(dot(normal, lightDirTemp), 0.0f);
	float spec = pow(max(dot(normal, halfVector), 0.0f), shininessSpot);
	float distance = length(spotLightPosition - fPos);
	float attenuation = 1.0f / (spotConstant + spotLinear * distance + spotQuadratic * distance * distance);

	float theta = dot(lightDirTemp, normalize(-spotLightDirection));
	float epsilon = spotlightCutOff  - spotlightOuterCutOff ;
	float intensity = clamp((theta - spotlightOuterCutOff )/epsilon, 0.0, 1.0);

	vec3 ambient = spotLightColor * ambientSpotStrength * attenuation * intensity;
	vec3 diffuse = spotLightColor * ambientSpotStrength * diff * attenuation * intensity;
	vec3 specular = spotLightColor * specularSpotStrength * spec * attenuation * intensity;

	ambient *= vec3(pow(texture(diffuseTexture, fTexCoords).rgb, vec3(1.0/gamma)));
	diffuse *= vec3(pow(texture(diffuseTexture, fTexCoords).rgb, vec3(1.0/gamma)));
	specular *= vec3(pow(texture(specularTexture, fTexCoords).rgb, vec3(1.0/gamma)));
	
	return ambient + diffuse + specular;
}
////////////////////////////////////////////////////////////
// shadow value computation
float computeShadow()
{	
	// perform perspective divide
    vec3 normalizedCoords = fPosLightSpace.xyz / fPosLightSpace.w;		
    normalizedCoords = normalizedCoords *0.5f+0.5f;

	float currentDepth = normalizedCoords.z;
    float closestDepth = texture(shadowMap,normalizedCoords.xy).r;
    float bias =  max(0.05 * (1.0 - dot(normal, directionalLightDir)), 0.005);


	//Percentage-closer filtering
	float shadow = 0.0;
	vec2 texelSize = 1.0/textureSize(shadowMap,0);
	for(int x=-1;x<=1;++x)
	{
		for(int y=-1;y<=1;++y)
		{ 
			float pcfDepth = texture(shadowMap,normalizedCoords.xy+vec2(x,y)*texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0; 
		}
	}

    return shadow/9.0f;	
}

////////////////////////////////////////////////////////////
// fog value computation
float computeFog()
{

 float fmentDistance = length(fPosEye);
 float fogFactor = exp(-pow(fmentDistance * 0.005f, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}

int compareVec3(vec3 a, vec3 b)
{
	if(a.x > b.x)
	{
		if(a.y > b.y)
		{
			if(a.z > b.z)
			{
				return 3;
			}
			else
			{
				return 2;
			}
		}
		else
		{
			return 1;
		}
	}
	else if(a.x < b.x)
	{
		if(a.y < b.y)
		{
			if(a.z < b.z)
			{
				return -3;
			}
			else
			{
				return -2;
			}
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return 0;
	}
}
////////////////////////////////////////////////////////////
void main() 
{
	////////////////////////////////////////////////////////////
	directionalLightDir = normalize(directionalLightPosition - fPos);
    vec3 color = texture(diffuseTexture, fTexCoords).rgb;
    viewDir = normalize(fPosEye.xyz - fPos);
	
	////////////////////////////////////////////////////////////
	vec3 totalLight = CalcDirLight();
	// For all lights, compute the light effect on the fment
	for(int i=0;i<LIGHT_MAX;i++)
	{
		vec3 vectorToLight = normalize(pointLights[i].location - fPos);

		if(dot(vectorToLight,normal)>=0.0f)
		{
			vec3 currentCalcPoint = CalcPointLight(pointLights[i].location,pointLights[i].color);
			currentCalcPoint = currentCalcPoint*clamp(pointLights[i].intensity,0.0f,1.0f);
			totalLight += currentCalcPoint;
		}	
	}

	// calculate the spot light component
	for(int i=0;i<SPOT_LIGHTS_MAX;i++)
	{
		if(spotLights[i].spotinit == 1)
		{
			totalLight += computeLightSpotComponents(spotLights[i].spotLightPosition,spotLights[i].spotLightDirection);
		}
	}
		
	////////////////////////////////////////////////////////////
	float shadow = computeShadow();
	
	float shadowConstant =1.0f;
	if(compareVec3(directionalLightColor,vec3(0.36,0.17,0.015))>0 && compareVec3(directionalLightColor,vec3(0.37,0.19,0.02))>0) // 5c2d04
	{
		shadowConstant = 2.0;
	}
	else if(compareVec3(directionalLightColor,vec3(0.9,0.88,0.77))>0) // ffe4c9
	{
		shadowConstant = 4.0;
	}
	////////////////////////////////////////////////////////////
	vec3 shadowColor = (ambient + (1-shadow) * shadowConstant * (diffuse + specular)) * color; 
    
	////////////////////////////////////////////////////////////
	// Compute the fog factor
	float fogFactor = computeFog();
	vec3 fogColor = vec3(0.3f, 0.3f, 0.3f);
	////////////////////////////////////////////////////////////

	fColor = vec4(mix(fogColor, min(shadowColor * totalLight, vec3(1.0f)), fogFactor),alpha);
}
