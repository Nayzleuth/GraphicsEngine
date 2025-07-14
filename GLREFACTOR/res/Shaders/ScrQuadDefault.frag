#version 450 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D screenTexture;

void main() {
    vec4 color = texture(screenTexture, texCoord);
    FragColor = color;
}