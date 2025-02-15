#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;
in vec4 position;

out vec4 fColor;

//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 lightColor1;
uniform vec3 lightPos;
uniform vec3 lightPos1;
uniform vec3 lightPos2;
uniform vec3 lightPos3;
uniform vec3 lightPos4;

//texture
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

// Point light constants
float constant = 1.0f;
float linear = 0.07f;
float quadratic = 0.14f;
float ambientPoint = 0.5f;
float specularStrengthPoint = 0.5f;

// Existing lighting variables
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

// Add point light calculation function
vec3 calcPointLight(vec3 lightPosition, vec3 lightCol) {
    vec3 cameraPosEye = vec3(0.0f);
    vec3 normalEye = normalize(fNormal);
    
    vec3 lightDirection = normalize(lightPosition - position.xyz);
    float distance = length(lightPosition - position.xyz);
    
    vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
    vec3 halfVector = normalize(lightDirection + viewDirN);
    
    float attenuation = 1.0f / (constant + linear * distance + quadratic * (distance * distance));
    
    vec3 ambient1 = ambientPoint * lightCol;
    vec3 diffuse1 = max(dot(normalEye, lightDirection), 0.0f) * lightCol;
    
    float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), 32.0f);
    vec3 specular1 = specularStrengthPoint * specCoeff * lightCol;
    
    return (ambient1 + diffuse1 + specular1) * attenuation;
}

void computeLightComponents()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;
}

float computeShadow()
{
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	normalizedCoords = normalizedCoords * 0.5 + 0.5;
	if (normalizedCoords.z > 1.0f) return 0.0f;
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	float currentDepth = normalizedCoords.z;
	float bias = 0.005f;
	float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
	return shadow;
}

float computeFog()
{
	float fogDensity = 0.0005f;
	float fragmentDistance = length(fPosEye.xyz);
	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));
	return clamp(fogFactor, 0.0f, 1.0f);
}

// Add with other uniforms at the top
uniform bool isNightTime;
uniform bool isLightning;  // New uniform for lightning effect

void main() {
    vec3 color;
    
    if (isNightTime) {
        // Only directional light (night)
        computeLightComponents();
        
        ambient *= texture(diffuseTexture, fTexCoords).rgb;
        diffuse *= texture(diffuseTexture, fTexCoords).rgb;
        specular *= texture(specularTexture, fTexCoords).rgb;

        float shadow = computeShadow();
        color = min((ambient + (1.0f - shadow) * diffuse) + (1.0f - shadow) * specular, 1.0f);
        
        // Add lightning effect
        if (isLightning) {
            // Bright white flash during lightning
            vec3 lightningColor = vec3(1.0, 1.0, 1.0) * 2.0;
            color = mix(color, lightningColor, 0.7);
        }
    } else {
        // Only point lights (day)
        color = vec3(0.0f);  // Start with no light
        
        // Add all point light contributions
        vec3 pointLight1 = calcPointLight(lightPos, lightColor1);
        vec3 pointLight2 = calcPointLight(lightPos1, lightColor1);
        vec3 pointLight3 = calcPointLight(lightPos2, lightColor1);
        vec3 pointLight4 = calcPointLight(lightPos3, lightColor1);
        vec3 pointLight5 = calcPointLight(lightPos4, lightColor1);
        
        color = pointLight1 + pointLight2 + pointLight3 + pointLight4 + pointLight5;
        color *= texture(diffuseTexture, fTexCoords).rgb;
    }
    
    float fogFactor = computeFog();
    vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
    
    // Modify fog color during lightning
    if (isLightning) {
        fogColor = vec4(0.8f, 0.8f, 1.0f, 1.0f);
        fogFactor = mix(fogFactor, 1.0, 0.3); // Reduce fog during lightning
    }
    
    fColor = mix(fogColor, vec4(color, 1.0f), fogFactor);
}
