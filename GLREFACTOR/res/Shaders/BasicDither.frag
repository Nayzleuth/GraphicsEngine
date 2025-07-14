#version 450 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D screenTexture;

float random(vec2 pos) {
    return fract(sin(dot(pos, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec4 color = texture(screenTexture, texCoord);

    float noise = random(gl_FragCoord.xy);
    vec3 ditheredColor = color.rgb + (noise - 0.5) / 255.0;

    FragColor = vec4(clamp(ditheredColor, 0.0, 1.0), color.a);
}