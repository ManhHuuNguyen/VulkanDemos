#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding=2) uniform PointLight {
	vec3 positionEyeCoord;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
} lights[4];

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
	float brightness = dot(fragColor, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0) {
		outColor = vec4(fragColor, 1.0);
	}
	else {
		outColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}