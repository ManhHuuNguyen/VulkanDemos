#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding=1) uniform Light {
	vec4 position;
	vec4 intensity;
	vec4 ambientIntensity;
} light;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec4 positionEyeCoord;
layout(location = 2) in vec4 normalEyeCoord;

layout(location = 0) out vec4 outColor;

void main() {
	vec4 l = normalize(light.position - positionEyeCoord);
	vec4 diffuse = light.intensity * vec4(fragColor, 0.0) * max(0.0, dot(l, normalEyeCoord));
	vec4 ambient = light.ambientIntensity * vec4(fragColor, 0.0);
    outColor = ambient + diffuse;
}