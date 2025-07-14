#version 450 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D screenTexture;
uniform vec2 TexelSize;  
uniform bool horizontal; 


void main() {
    vec2 TexelResized = TexelSize * 2.0;
    
    vec2 offset = horizontal ? 
        vec2(TexelResized.x, 0.0) :
        vec2(0.0, TexelResized.y); 

    float weights[4] = float[](0.25, 0.125, 0.0625, 0.03125);
    float totalWeight = weights[0] + 2.0 * (weights[1] + weights[2] + weights[3]);

    vec3 col = texture(screenTexture, texCoord).rgb * weights[0];

    for(int i = 1; i < 4; i++){
        col += texture(screenTexture, texCoord + offset * i).rgb * weights[i];
        col += texture(screenTexture, texCoord - offset * i).rgb * weights[i];
    }

    FragColor = vec4(col / totalWeight, 1.0);
}