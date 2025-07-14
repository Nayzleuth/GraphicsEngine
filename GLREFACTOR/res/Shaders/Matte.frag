#version 450 core
out vec4 FragColor;

uniform vec3 color = vec3(0);

void main() {
    if(color == vec3(0)){
        FragColor = vec4(1);
    } else {
        FragColor = vec4(color, 1);
    }
}