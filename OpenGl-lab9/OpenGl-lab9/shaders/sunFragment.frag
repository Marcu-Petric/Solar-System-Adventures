#version 410 core

in vec3 normal;
in vec2 texCoords;
in vec3 fragPos;
in float noise;

out vec4 fragColor;

uniform sampler2D diffuseTexture;
uniform float time;

void main() {
    // More intense core color (white-hot center)
    vec3 coreColor = vec3(1.0, 0.95, 0.8);
    // Outer flame color (deep orange)
    vec3 flameColor = vec3(1.0, 0.2, 0.0);
    
    // Slower explosive pulses (reduced time multiplier from 2.0 to 0.5)
    float pulse = sin(time * 0.5 + length(fragPos) * 1.5) * 0.5 + 0.5;
    
    // Slower and more subtle flicker (reduced time multiplier from 5.0 to 2.0)
    float flicker = sin(time * 2.0 + noise * 5.0) * 0.2 + 0.8;
    
    // Mix colors based on noise and pulse
    vec3 finalColor = mix(coreColor, flameColor, noise * pulse * flicker);
    
    // Add extra brightness to the core (reduced multiplier from 0.5 to 0.3)
    float coreBrightness = 1.0 - length(texCoords - vec2(0.5)) * 2.0;
    coreBrightness = max(0.0, coreBrightness);
    finalColor += coreColor * coreBrightness * 0.3;
    
    // Slightly reduced intensity (from 1.5 to 1.3)
    finalColor *= 1.3;
    
    // Add texture detail
    vec4 texColor = texture(diffuseTexture, texCoords);
    finalColor *= texColor.rgb;
    
    fragColor = vec4(finalColor, 1.0);
}