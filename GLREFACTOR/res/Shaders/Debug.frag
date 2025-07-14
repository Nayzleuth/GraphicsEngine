#version 450 core
out vec4 FragColor;

in vec3 aNorms;

void main() {
    FragColor = vec4(aNorms, 1.0f) + 0.25f;
} 