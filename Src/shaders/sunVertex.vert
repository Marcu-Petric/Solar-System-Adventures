#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

out vec3 normal;
out vec2 texCoords;
out vec3 fragPos;
out float noise;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

// Simplified noise function for more explosive effect
float rand(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
    // Create radial explosion effect
    float explosionFrequency = 0.2;  // Much slower frequency
    float explosionAmplitude = 0.01;  // Much smaller amplitude
    
    vec3 pos = vPosition;
    float distance = length(pos);  // Distance from center
    
    // Create radial pulses (reduced frequency)
    float pulse = sin(time * explosionFrequency + distance * 0.5) * 0.5 + 0.5;
    float distortion = pulse * rand(vec2(distance, time * 0.1)); // Slower time variation
    
    // Apply distortion outward from center
    vec3 direction = normalize(pos);
    pos += direction * distortion * explosionAmplitude * distance;
    
    gl_Position = projection * view * model * vec4(pos, 1.0);
    
    normal = mat3(transpose(inverse(model))) * vNormal;
    texCoords = vTexCoords;
    fragPos = vec3(model * vec4(pos, 1.0));
    noise = distortion;
}