#version 410 core

layout(location = 0) in vec3 worldPos;
layout(location = 1) in float scale;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 airFlow;
uniform float flowStrength;

out float opacity;
out vec3 debugWorldPos;

void main() {
    debugWorldPos = worldPos;
    
    vec3 flowOffset = airFlow * flowStrength * 0.1;
    vec3 adjustedPos = worldPos + flowOffset;
    
    vec4 viewPos = view * vec4(adjustedPos, 1.0);
    gl_Position = projection * viewPos;
    
    float dist = length(viewPos.xyz);
    gl_PointSize = clamp(scale * 800.0 / dist, 2.0, 8.0);
    
    opacity = clamp(1.0 - (dist / 2000.0), 0.2, 1.0);
} 