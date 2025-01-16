#version 410 core

out vec4 FragColor;

void main() {
    vec2 coord = gl_PointCoord * 2.0 - 1.0;
    float r = length(coord);
    
    if (r > 1.0) discard;
    
    float alpha = (1.0 - r * r);
    FragColor = vec4(0.9, 0.9, 1.0, alpha);
} 