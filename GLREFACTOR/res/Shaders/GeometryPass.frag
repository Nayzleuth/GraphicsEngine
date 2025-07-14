#version 450 core
layout (location = 0) out vec3 gPos;
layout (location = 1) out vec3 gCol;
layout (location = 2) out vec3 gNorm;

in vec3 fragPos;
in vec2 fragTexCoord;
in vec3 fragNorm;

uniform sampler2D ObjTex;

void main() {
    gPos = fragPos;
    gCol = vec3(1);
    gNorm = fragNorm;
}

