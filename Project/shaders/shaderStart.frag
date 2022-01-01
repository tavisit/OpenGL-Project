#version 430 core
in vec3 normal;
in vec4 fragPosEye;
in vec4 fragPosLightSpace;
in vec2 fragTexCoords;

in vec3 fragPos;

out vec4 fColor;

// directionalLight light
float ambientStrength = 0.3f;
float specularStrength = 0.7f;
float shininess = 32.0f;

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
float specularPointStrength = 0.5f;
float shininessPoint = 32.0f;

float constant = 1.0f;
float linear = 0.00225f;
float quadratic = 0.00375;

uniform	vec3 directionalLightColor;
uniform	vec3 directionalLightPosition;
uniform	float alpha;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

// spot light
float spotConstant = 1.0f;
float spotLinear = 0.1f;
float spotQuadratic = 0.02f;

vec3 spotLightAmbient = vec3(0.0f, 0.0f, 0.0f);
vec3 spotLightSpecular = vec3(1.0f, 1.0f, 1.0f);
vec3 spotLightColor = vec3(10,10,10);

float spotlightCutOff = cos(radians(10));
float spotlightOuterCutOff =  cos(radians(40));

#define SPOT_LIGHTS_MAX 1
struct SPOT_LIGHT
{
    int spotinit;
    vec3 spotLightDirection;
    vec3 spotLightPosition;
	// 3 locations
};
layout(location = 60) uniform SPOT_LIGHT spotLights[SPOT_LIGHTS_MAX]; // locations [0, SPOT_LIGHTS_MAX*3)

//General global variables
vec3 diffuse;
vec3 ambient;
vec3 specular;
vec3 Normal;
vec3 viewDir;
vec3 directionalLightDir;
vec3 currentPointlightPos;
vec3 currentPointlightColor;

// point light
vec3 CalcPointLight(vec3 currentPointlightPos,vec3 currentPointlightColor)
{
	vec3 pointLightDir = normalize(currentPointlightPos - fragPos);
	float diff = max(dot(Normal, pointLightDir), 0.0);
	vec3 reflectDir = reflect(-pointLightDir, Normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	float distance    = length(currentPointlightPos - fragPos);
	float attenuation = constant + linear * distance + quadratic * (distance * distance); 

	vec3 ambient = ambientPointStrength *currentPointlightColor;
	vec3 diffuse = ambientPointStrength *diff * currentPointlightColor;
	vec3 specular = shininessPoint * spec * currentPointlightColor;
	ambient *= vec3(texture(diffuseTexture, fragTexCoords));
	diffuse *= vec3(texture(diffuseTexture, fragTexCoords));
	specular *= vec3(texture(specularTexture, fragTexCoords));

	return (ambient + diffuse + specular) / attenuation;
}
vec3 CalcDirLight()
{
    vec3 lightDirTemp = normalize(directionalLightDir);
    float diff = max(dot(Normal, lightDirTemp), 0.0);
    vec3 reflectDir = reflect(-lightDirTemp, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    // combine results
	ambient = ambientStrength * directionalLightColor;
	diffuse = ambientStrength * diff * directionalLightColor;
	specular = specularStrength * spec * directionalLightColor;

    return (ambient + diffuse + specular);
}  
// spot light
vec3 computeLightSpotComponents(vec3 spotLightPosition, vec3 spotLightDirection) {
	vec3 cameraPosEye = vec3(0.0f);
	vec3 lightDirTemp = normalize(spotLightPosition - fragPos);
	vec3 viewDirN = normalize(cameraPosEye - fragPosEye.xyz);
	vec3 halfVector = normalize(lightDirTemp + viewDirN);

	float diff = max(dot(normal, lightDirTemp), 0.0f);
	float spec = pow(max(dot(Normal, halfVector), 0.0f), shininess);
	float distance = length(spotLightPosition - fragPos);
	float attenuation = 1.0f / (spotConstant + spotLinear * distance + spotQuadratic * distance * distance);

	float theta = dot(lightDirTemp, normalize(-spotLightDirection));
	float epsilon = spotlightCutOff  - spotlightOuterCutOff ;
	float intensity = clamp((theta - spotlightOuterCutOff )/epsilon, 0.0, 1.0);

	vec3 ambient = spotLightColor * spotLightAmbient * attenuation * intensity;
	vec3 diffuse = spotLightColor * spotLightSpecular * diff *attenuation * intensity;
	vec3 specular = spotLightColor * spotLightSpecular * spec *attenuation * intensity;
	
	return ambient + diffuse + specular;
}
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

float computeFog()
{

 float fragmentDistance = length(fragPosEye);
 float fogFactor = exp(-pow(fragmentDistance * 0.005f, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{
	directionalLightDir = normalize(directionalLightPosition - fragPos);
    vec3 color = texture(diffuseTexture, fragTexCoords).rgb;
    Normal = normalize(normal);
    viewDir = normalize(fragPosEye.xyz - fragPos);

	float shadow = 0.0f;
	vec3 light = CalcDirLight();
	
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
			light += currentCalcPoint;
		}	
	}

	for(int i=0;i<SPOT_LIGHTS_MAX;i++)
	{
		if(spotLights[i].spotinit == 1)
		{
			light+= computeLightSpotComponents(spotLights[i].spotLightPosition,spotLights[i].spotLightDirection);
		}
	}

	
	shadow = computeShadow();
	// If the light crosses a shadow, it has to illuminate and eliminiate to a degree the shadow
	float biasShadowDistance = 10.0f;
	if(distance <  biasShadowDistance* pointLights[lightMinIndex].intensity)
	{
		distance = 1 - (distance/(biasShadowDistance* pointLights[lightMinIndex].intensity));
		shadow = shadow-distance;
		
	}	

	// Modulate ambiental,diffuse and specular components
	
	// Compute overall color
	ambient *= vec3(texture(diffuseTexture, fragTexCoords));
	diffuse *= vec3(texture(diffuseTexture, fragTexCoords));
	specular *= vec3(texture(specularTexture, fragTexCoords));

	vec3 lighting = (shadow*(diffuse+specular)+ambient)*color;    
    
	
	// Compute the fog factor
	float fogFactor = computeFog();
	vec3 fogColor = vec3(0.5f, 0.5f, 0.5f);
	
	fColor = vec4(mix(fogColor, min(lighting * light, vec3(1.0f)), fogFactor),alpha);
}
