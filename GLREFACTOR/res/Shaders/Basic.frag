#version 450 core
out vec4 FragColor;

struct Light {
	vec4 position;
	vec4 color;
	float intensity;
	float range;
};

layout(set = 0, binding = 0) buffer LightBuffer {
    Light lights[];
};

in vec3 fragPos;
in vec3 fragNorm;

vec4 calculatePointLight(Light light, vec3 fragPos, vec3 fragNormal) {
    vec3 lightDir = normalize(light.position.xyz - fragPos);
    float distance = length(light.position.xyz - fragPos);
    
    float d = distance / light.range;
    
    float attenuation = smoothstep(1.0, 0.0, d);    
    float diff = 
    (dot(fragNormal, lightDir), 0.0);

    vec3 lightEffect = light.color.rgb * light.intensity * diff * attenuation;
    
    return vec4(lightEffect, 1.0);
}

void main() {
    vec4 fragColorCalc = vec4(0);
    for(int i = 0; i < 300; i++){
        fragColorCalc += calculatePointLight(lights[i], fragPos, fragNorm);
    }

    fragColorCalc = clamp(fragColorCalc, 0.0, 1.0);
    
    FragColor = fragColorCalc;
}

