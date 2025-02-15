#version 410 core

in float opacity;
out vec4 FragColor;
in vec3 debugWorldPos;

void main() {
    vec2 uv = gl_PointCoord * 2.0 - 1.0;
    float dist = length(uv);
    
    if (dist > 1.0) discard;
    
    float fadeValue = (1.0 - dist * dist) * 0.8;
    fadeValue *= opacity;
    
    fadeValue *= 1.0 + uv.y * 0.5;
    
    FragColor = vec4(0.8, 0.9, 1.0, fadeValue * 0.8);
} 