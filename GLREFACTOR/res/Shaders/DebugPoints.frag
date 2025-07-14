#version 450 core
out vec4 FragColor;

in vec3 pos;
in vec3 color;

void main() {
    vec3 colorCheck = vec3(1);

    if(color == vec3(0)){
        FragColor = vec4(colorCheck, 1);
    } else {
        FragColor = vec4(color, 1);
    } 
}