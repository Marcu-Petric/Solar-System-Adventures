#version 410 core

in float visibility;
out vec4 FragColor;
in vec3 debugPosition;

void main() {
    vec2 coord = gl_PointCoord * 2.0 - 1.0;
    float r = length(coord);
    
    if (r > 1.0) discard;
    
    float alpha = (1.0 - r * r) * 0.8;
    alpha *= visibility;
    
    // Add slight stretching effect in wind direction
    alpha *= 1.0 + coord.y * 0.5;
    
    FragColor = vec4(0.8, 0.9, 1.0, alpha * 0.8);
} 