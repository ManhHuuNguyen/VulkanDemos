#version 450
#extension GL_ARB_separate_shader_objects : enable
//change this directional light to pointlight later

layout(binding=3) uniform PointLight {
	vec3 positionEyeCoord;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
} light;

layout (binding = 4) uniform sampler2D depthMap;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec4 positionEyeCoord;
layout(location = 2) in vec3 normalEyeCoord;
layout(location = 3) in vec4  positionProjLightCoord;

layout(location = 0) out vec4 outColor;

float isInShadow(vec4 fragPos) {
	//perspective division
	vec3 projCoord = vec3(fragPos) / fragPos.w;
	projCoord = projCoord * 0.5 + vec3(0.5, 0.5, 0.5); // x, y need to be in range (0, 1) bc they are textureCoord and z need to be in range (0, 1) because that is the range of depth value stored
	float closestDepth = texture(depthMap, vec2(projCoord)).r;
	return (projCoord.z > closestDepth)? 1.0: 0.0;
}

void main() {
	float shadow = isInShadow(positionProjLightCoord);
	float dist = length(vec4(light.positionEyeCoord, 1.0) - positionEyeCoord);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);
	vec3 ambient = light.ambient * fragColor * attenuation;
	vec4 l = normalize(vec4(light.positionEyeCoord, 1.0) - positionEyeCoord);
	vec3 diffuse = light.diffuse * fragColor * max(0.0, dot(vec3(l), normalEyeCoord)) * attenuation;
	outColor = vec4(ambient + diffuse * (1.0 - shadow), 1.0);
}

