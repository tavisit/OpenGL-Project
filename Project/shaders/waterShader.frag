#version 430 core
in vec3 normal;
in vec4 fragPosEye;
in vec4 fragPosLightSpace;
in vec2 fragTexCoords;

in vec3 fragPos;

out vec4 fColor;

////////////////////////////////////////////////////////////
// General uniform variables

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

////////////////////////////////////////////////////////////
// directional light

float ambientDirectionalStrength = 0.5f;
float specularDirectionalStrength = 0.7f;
float shininessDirectional = 128.0f;

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

float ambientPointStrength = 0.7f;
float specularPointStrength = 0.7f;
float shininessPoint = 64.0f;

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
float specularSpotStrength = 0.7f;
float shininessSpot = 128.0f;

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

float gamma = 2.2;
vec3 Normal;
vec3 viewDir;
vec3 directionalLightDir;
vec3 currentPointLightPos;
vec3 currentPointLightColor;

////////////////////////////////////////////////////////////
// point light computation
vec3 CalcPointLight(vec3 currentPointLightPos,vec3 currentPointLightColor)
{
	vec3 pointLightDir = normalize(currentPointLightPos - fragPos);
    vec3 lightDirTemp = normalize(pointLightDir);
	// Blinn-Phong shading model
	vec3 halfVector = normalize(lightDirTemp + viewDir);

	float diff = max(dot(Normal, lightDirTemp), 0.0);
	float spec = pow(max(dot(viewDir, halfVector), 0.0), shininessPoint);
	float distance    = length(currentPointLightPos - fragPos);
	float attenuation = pointConstant + pointLinear * distance + pointQuadratic * (distance * distance); 

	vec3 ambient = ambientPointStrength * currentPointLightColor ;
	vec3 diffuse = ambientPointStrength *diff * currentPointLightColor;
	vec3 specular = shininessPoint * spec * currentPointLightColor;
	ambient *= vec3(pow(texture(diffuseTexture, fragTexCoords).rgb, vec3(1.0/gamma)));
	diffuse *= vec3(pow(texture(diffuseTexture, fragTexCoords).rgb, vec3(1.0/gamma)));
	specular *= vec3(pow(texture(specularTexture, fragTexCoords).rgb, vec3(1.0/gamma)));

	return (ambient + diffuse + specular) / attenuation;
}

////////////////////////////////////////////////////////////
// directinal light computation
vec3 CalcDirLight()
{
    vec3 lightDirTemp = normalize(directionalLightDir);
	// Blinn-Phong shading model
	vec3 halfVector = normalize(lightDirTemp + viewDir);
    float diff = max(dot(Normal, lightDirTemp), 0.0);
    float spec = pow(max(dot(viewDir, halfVector), 0.0), shininessDirectional);

    // combine results
    vec3 ambient = ambientDirectionalStrength * directionalLightColor;
	vec3 diffuse = ambientDirectionalStrength * diff * directionalLightColor;
	vec3 specular = specularDirectionalStrength * spec * directionalLightColor;
	ambient *= vec3(pow(texture(diffuseTexture, fragTexCoords).rgb, vec3(1.0/gamma)));
	diffuse *= vec3(pow(texture(diffuseTexture, fragTexCoords).rgb, vec3(1.0/gamma)));
	specular *= vec3(pow(texture(specularTexture, fragTexCoords).rgb, vec3(1.0/gamma)));

    return (ambient + diffuse + specular);
}  
////////////////////////////////////////////////////////////
// spot light computation
vec3 computeLightSpotComponents(vec3 spotLightPosition, vec3 spotLightDirection) {
	vec3 lightDirTemp = normalize(spotLightPosition - fragPos);
	// Blinn-Phong shading model
	vec3 halfVector = normalize(lightDirTemp + viewDir);

	float diff = max(dot(normal, lightDirTemp), 0.0f);
	float spec = pow(max(dot(Normal, halfVector), 0.0f), shininessSpot);
	float distance = length(spotLightPosition - fragPos);
	float attenuation = 1.0f / (spotConstant + spotLinear * distance + spotQuadratic * distance * distance);

	float theta = dot(lightDirTemp, normalize(-spotLightDirection));
	float epsilon = spotlightCutOff  - spotlightOuterCutOff ;
	float intensity = clamp((theta - spotlightOuterCutOff )/epsilon, 0.0, 1.0);

	vec3 ambient = spotLightColor * ambientSpotStrength * attenuation * intensity;
	vec3 diffuse = spotLightColor * ambientSpotStrength * diff *attenuation * intensity;
	vec3 specular = spotLightColor * specularSpotStrength * spec *attenuation * intensity;
	ambient *= vec3(pow(texture(diffuseTexture, fragTexCoords).rgb, vec3(1.0/gamma)));
	diffuse *= vec3(pow(texture(diffuseTexture, fragTexCoords).rgb, vec3(1.0/gamma)));
	specular *= vec3(pow(texture(specularTexture, fragTexCoords).rgb, vec3(1.0/gamma)));
	
	return ambient + diffuse + specular;
}
////////////////////////////////////////////////////////////
// shadow value computation
float computeShadow()
{	
	// perform perspective divide
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    if(normalizedCoords.z > 1.0f)
        return 1.0f;  
		
    normalizedCoords = normalizedCoords *0.5f+0.5f;

    float currentDepth = texture(shadowMap,normalizedCoords.xy).r;
    float bias =  max(0.05 * (1.0 - dot(normal, directionalLightDir)), 0.005);

	float shadow = 0.0;
	//Percentage-closer filtering
	vec2 texelSize = 1.0/textureSize(shadowMap,0);
	for(int x=-1;x<=1;++x)
	{
		for(int y=-1;y<=1;++y)
		{
			float depth = texture(shadowMap,normalizedCoords.xy+vec2(x,y)*texelSize).r;
			shadow+= (currentDepth+bias)<normalizedCoords.z ? 0.0f:1.0f;
		}
	}

    return shadow/9.0f;	
}

////////////////////////////////////////////////////////////
// fog value computation
float computeFog()
{

 float fragmentDistance = length(fragPosEye);
 float fogFactor = exp(-pow(fragmentDistance * 0.005f, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}

////////////////////////////////////////////////////////////
void main() 
{
	////////////////////////////////////////////////////////////
	directionalLightDir = normalize(directionalLightPosition - fragPos);
    vec3 color = pow(pow(texture(diffuseTexture, fragTexCoords).rgb, vec3(1.0/gamma)).rgb, vec3(1.0/gamma));
    Normal = normalize(normal);
    viewDir = normalize(fragPosEye.xyz - fragPos);
	
	////////////////////////////////////////////////////////////
	vec3 totalLight = CalcDirLight();
	
	float distance = 1000;
	int lightMinIndex = 0;

	// For all lights, compute the light effect on the fragment
	for(int i=0;i<LIGHT_MAX;i++)
	{
		vec3 vectorToLight = normalize(pointLights[i].location - fragPos);

		if(dot(vectorToLight,Normal)>=0.0f)
		{
			float currentDistance = length(pointLights[i].location - fragPos);
			if(currentDistance<distance)
			{
				distance = currentDistance;
				lightMinIndex = i;
			}
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
	// If the light crosses a shadow, it has to illuminate and eliminiate to a degree the shadow
	float shadow = computeShadow();
	float biasShadowDistance = 10.0f;
	if(distance <  biasShadowDistance* pointLights[lightMinIndex].intensity)
	{
		distance = 1 - (distance/(biasShadowDistance* pointLights[lightMinIndex].intensity));
		shadow = shadow-distance;
	}	

	////////////////////////////////////////////////////////////
    vec3 lightDirTemp = normalize(directionalLightDir);
    float diff = max(dot(Normal, lightDirTemp), 0.0);
    vec3 reflectDir = reflect(-lightDirTemp, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininessDirectional);
	vec3 ambient = ambientDirectionalStrength * directionalLightColor;
	vec3 diffuse = ambientDirectionalStrength * diff * directionalLightColor;
	vec3 specular = specularDirectionalStrength * spec * directionalLightColor;

	ambient *= vec3(pow(texture(diffuseTexture, fragTexCoords).rgb, vec3(1.0/gamma)));
	diffuse *= vec3(pow(texture(diffuseTexture, fragTexCoords).rgb, vec3(1.0/gamma)));
	specular *= vec3(pow(texture(specularTexture, fragTexCoords).rgb, vec3(1.0/gamma)));
	////////////////////////////////////////////////////////////

	vec3 shadowColor = (ambient + shadow * (diffuse + specular)) * color;   
    
	////////////////////////////////////////////////////////////
	// Compute the fog factor
	float fogFactor = computeFog();
	vec3 fogColor = vec3(0.5f, 0.5f, 0.5f);
	////////////////////////////////////////////////////////////

	fColor = vec4(mix(fogColor, min(shadowColor * totalLight, vec3(1.0f)), fogFactor),alpha);
}
