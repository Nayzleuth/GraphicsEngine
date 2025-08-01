#version 450 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D texture1;

void main() {
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    FragColor = texture(texture1, texCoord);
}