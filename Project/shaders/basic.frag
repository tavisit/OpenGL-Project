#version 410 core

in vec3 normal;
in vec4 fragPosEye;
in vec4 fragPosLightSpace;
in vec2 fragTexCoords;

in vec3 fragPos;

out vec4 fColor;


// light
uniform	mat3 normalMatrix;
uniform mat3 lightDirMatrix;
uniform	vec3 lightColor;
uniform	vec3 lightDir;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 64.0f;

uniform vec3 lightPos1;
uniform vec3 lightPos2;

float constant = 1.0f;
float linear = 0.00225f;
float quadratic = 0.00375;

float ambientPoint = 0.5f;
float specularStrengthPoint = 0.5f;
float shininessPoint = 32.0f;

uniform mat4 view;

float spotQuadratic = 0.02f;
float spotLinear = 0.09f;
float spotConstant = 1.0f;

vec3 spotLightAmbient = vec3(0.0f, 0.0f, 0.0f);
vec3 spotLightSpecular = vec3(1.0f, 1.0f, 1.0f);
vec3 spotLightColor = vec3(12,12,12);

uniform float spotlight1;
uniform float spotlight2;

uniform vec3 spotLightDirection;
uniform vec3 spotLightPosition;



vec3 computeLightComponents()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(normalMatrix * normal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDirMatrix * lightDir);	

	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fragPosEye.xyz);
	
	//compute half vector
	vec3 halfVector = normalize(lightDirN + viewDirN);
		
	//compute ambient light
	ambient = ambientStrength * lightColor *2.0f;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	float specCoeff = pow(max(dot(halfVector, normalEye), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;
		
	return (ambient + diffuse + specular);
	
}

// point light
vec3 computePointLight(vec4 lightPosEye)
{
	vec3 cameraPosEye = vec3(0.0f);
	vec3 normalEye = normalize(normalMatrix * normal);
	vec3 lightDirN = normalize(lightPosEye.xyz - fragPosEye.xyz);
	vec3 viewDirN = normalize(cameraPosEye - fragPosEye.xyz);
	vec3 ambient = ambientPoint * lightColor;
	vec3 diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	vec3 halfVector = normalize(lightDirN + viewDirN);
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininessPoint);
	vec3 specular = specularStrengthPoint * specCoeff * lightColor;
	float distance = length(lightPosEye.xyz - fragPosEye.xyz);
	float att = 1.0f / (constant + linear * distance + quadratic * distance * distance);
	return (ambient + diffuse + specular) * att * vec3(2.0f,2.0f,2.0f);
}

// spot light
vec3 computeLightSpotComponents() {
	vec3 cameraPosEye = vec3(0.0f);
	vec3 lightDir = normalize(spotLightPosition - fragPos);
	vec3 normalEye = normalize(normalMatrix * normal);
	vec3 lightDirN = normalize(lightDirMatrix * lightDir);
	vec3 viewDirN = normalize(cameraPosEye - fragPosEye.xyz);
	vec3 halfVector = normalize(lightDirN + viewDirN);

	float diff = max(dot(normal, lightDir), 0.0f);
	float spec = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	float distance = length(spotLightPosition - fragPos);
	float attenuation = 1.0f / (spotConstant + spotLinear * distance + spotQuadratic * distance * distance);

	float theta = dot(lightDir, normalize(-spotLightDirection));
	float epsilon = spotlight1 - spotlight2;
	float intensity = clamp((theta - spotlight2)/epsilon, 0.0, 1.0);

	vec3 ambient = spotLightColor * spotLightAmbient * vec3(texture(diffuseTexture, fragTexCoords));
	vec3 diffuse = spotLightColor * spotLightSpecular * diff * vec3(texture(diffuseTexture, fragTexCoords));
	vec3 specular = spotLightColor * spotLightSpecular * spec * vec3(texture(specularTexture, fragTexCoords));
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;
	
	return ambient + diffuse + specular;
}

float computeShadow()
{	
	// perform perspective divide
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    if(normalizedCoords.z > 1.0f)
        return 0.0f;
    
	// Transform to [0,1] range
    normalizedCoords = normalizedCoords * 0.5f + 0.5f;
   
   // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;    
   
   // Get depth of current fragment from light's perspective
    float currentDepth = normalizedCoords.z;
   
   // Check whether current frag pos is in shadow
    float bias = 0.005f;
    float shadow = currentDepth - bias> closestDepth  ? 1.0f : 0.0f;

    return shadow;	
}

float computeFog()
{
 float fogDensity = 0.01;
 float fragmentDistance = length(fragPosEye);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{
	vec3 light = computeLightComponents();
	
	float shadow = computeShadow();
	
	// modulate with diffuse map
	ambient *= vec3(texture(diffuseTexture, fragTexCoords)) * 1.2f;
	diffuse *= vec3(texture(diffuseTexture, fragTexCoords)) ;//* 1.2f;
	// modulate with specular map
	specular *= vec3(texture(specularTexture, fragTexCoords)) ;//* 1.2f;
	
	// pointlight
	vec4 lightPosEye1 = view * vec4(lightPos1, 1.0f);
	light += computePointLight(lightPosEye1);
	
	vec4 lightPosEye2 =  view * vec4(lightPos2, 1.0f);
	light += computePointLight(lightPosEye2);
	
	vec4 diffuseColor = texture(diffuseTexture, fragTexCoords);
	
	light += computeLightSpotComponents();
	
	
	// modulate with shadow
	vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow) * specular, 1.0f);
    
	
	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	
	vec4 colorWithShadow = vec4(color,1.0f);
	fColor = mix(fogColor, min(colorWithShadow * vec4(light, 1.0f), 1.0f), fogFactor);	
}
