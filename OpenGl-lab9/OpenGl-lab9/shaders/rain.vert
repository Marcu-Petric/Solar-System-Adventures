#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in float size;

uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 viewPos = view * vec4(position, 1.0);
    gl_Position = projection * viewPos;
    
    float dist = length(viewPos.xyz);
    gl_PointSize = size * (4800.0 / dist);
} 