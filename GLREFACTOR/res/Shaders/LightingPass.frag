#version 450 core
out vec4 FragColor;

uniform sampler2D positionTex;
uniform sampler2D albedoTex;
uniform sampler2D normalTex;

in vec2 texCoord;

struct Light {
	vec4 position;
	vec4 color;
	float intensity;
	float range;
};

layout(set = 0, binding = 0) buffer LightBuffer {
    Light lights[];
};

vec4 calculatePointLight(Light light, vec3 fragPos, vec3 fragNormal) {
    vec3 lightDir = normalize(light.position.xyz - fragPos);

    float distance = length(light.position.xyz - fragPos);
    
    float d = distance / light.range;
    
    float attenuation = smoothstep(1.0, 0.0, d);    
    float diff = max(dot(fragNormal, lightDir), 0.0);

    vec3 lightEffect = light.color.rgb * light.intensity * diff * attenuation;

    return vec4(lightEffect, 1.0);
}

void main() {
    vec3 position = texture(positionTex, texCoord).rgb;
    vec3 albedo = texture(albedoTex, texCoord).rgb;
    vec3 normal = texture(normalTex, texCoord).rgb;

    vec4 PointLightingPerCurrentPixel = vec4(0);
    for(int i = 0; i < 300; i++){
        PointLightingPerCurrentPixel += calculatePointLight(lights[i], position, normal);
    }

    PointLightingPerCurrentPixel = clamp(PointLightingPerCurrentPixel, 0.0, 1.0);
    
    FragColor = PointLightingPerCurrentPixel;
}