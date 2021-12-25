#version 410 core

in vec3 normal;
in vec4 fragPosEye;
in vec4 fragPosLightSpace;
in vec2 fragTexCoords;

in vec3 fragPos;

out vec4 fColor;

// light
uniform	vec3 lightColor;
uniform	vec3 lightDir;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

// point light
#define LIGHT_MAX 2
uniform vec3 lightsPos[LIGHT_MAX];
vec3 currentPointlightPos;
uniform vec3 lightsColors[LIGHT_MAX];
vec3 currentPointlightColor;
uniform float lightIntensities[LIGHT_MAX];

vec3 overallColor;
float ambientStrength = 0.1f;
float specularStrength = 0.1f;
float shininess = 32.0f;
vec3 Normal;
vec3 viewDir;

float constant = 1.0f;
float linear = 0.00225f;
float quadratic = 0.00375;

// point light
vec3 CalcPointLight(vec3 currentPointlightPos,vec3 currentPointlightColor)
{
	vec3 lightDir = normalize(currentPointlightPos - fragPos);
	float diff = max(dot(Normal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, Normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	float distance    = length(currentPointlightPos - fragPos);
	float attenuation = 1.0 / (constant + linear * distance + 
  			     quadratic * (distance * distance)); 

	vec3 ambient = currentPointlightColor;
	vec3 diffuse = diff * currentPointlightColor;
	vec3 specular = spec * currentPointlightColor;
	ambient *= vec3(texture(diffuseTexture, fragTexCoords));
	diffuse *= vec3(texture(diffuseTexture, fragTexCoords));
	specular *= vec3(texture(specularTexture, fragTexCoords));

	return (ambient + diffuse + specular) * attenuation;
}
vec3 CalcDirLight()
{
    vec3 lightDirTemp = normalize(lightDir);
    // diffuse shading
    float diff = max(dot(Normal, lightDirTemp), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDirTemp, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    // combine results
	vec3 ambient = lightColor;
	vec3 diffuse = diff * lightColor;
	vec3 specular = spec * lightColor;
	ambient *= vec3(texture(diffuseTexture, fragTexCoords));
	diffuse *= vec3(texture(diffuseTexture, fragTexCoords));
	specular *= vec3(texture(specularTexture, fragTexCoords));

    return (ambient + diffuse + specular);
}  
float computeShadow()
{	
	// perform perspective divide
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    if(normalizedCoords.z > 1.0f)
        return 0.0f;
    normalizedCoords = normalizedCoords * 0.5f + 0.5f;
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;    
    float currentDepth = normalizedCoords.z;
    float bias =  max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = currentDepth - bias> closestDepth  ? 1.0f : 0.0f;

    return shadow;	
}

float computeFog()
{

 float fragmentDistance = length(fragPosEye);
 float fogFactor = exp(-pow(fragmentDistance * 0.005f, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{
    vec3 color = texture(diffuseTexture, fragTexCoords).rgb;
    Normal = normalize(normal);
    viewDir = normalize(fragPosEye.xyz - fragPos);
	float shadow = 0.0f;
	
	
	vec3 light = CalcDirLight();
	
	float distance = 1000;
	for(int i=0;i<LIGHT_MAX;i++)
	{
		distance = min(distance,length(lightsPos[i] - fragPos));
		vec3 currentCalcPoint = CalcPointLight(lightsPos[i],lightsColors[i]);
		currentCalcPoint = currentCalcPoint*clamp(lightIntensities[i],0.0f,1.0f);
		light += currentCalcPoint;
	}
	if(distance >= 10.0f && distance < 20.0f)
	{
		shadow = computeShadow();
		distance = 1 - ((distance-10.0f)/10.0f);
		shadow = clamp(shadow-distance,0.0f,1.0f);
	}
	else if(distance > 20.0f)
	{
		shadow = computeShadow();
	}
	vec3 lighting = light;

    vec3 ambient = ambientStrength * light;
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = ambientStrength*diff * light;
    vec3 halfwayDir = normalize(lightDir + viewDir); 
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), shininess);
    vec3 specular = specularStrength*spec * light; 

	ambient *= vec3(texture(diffuseTexture, fragTexCoords));
	diffuse *= vec3(texture(diffuseTexture, fragTexCoords));
	specular *= vec3(texture(specularTexture, fragTexCoords));
	lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * light;    
    
	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	
	fColor = mix(fogColor, min(vec4(lighting * color, 1.0f), vec4(1.0f)), fogFactor);
}
