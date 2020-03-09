#version 450
#extension GL_ARB_separate_shader_objects : enable
//change this directional light to pointlight later

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
layout(location = 1) in vec4 positionEyeCoord;
layout(location = 2) in vec3 normalEyeCoord;

layout(location = 0) out vec4 outColor;

void main() {
	vec3 result = vec3(0.0, 0.0, 0.0);
	for (int i = 0; i < 1; i++) {
		float dist = length(vec4(lights[i].positionEyeCoord, 1.0) - positionEyeCoord);
		float attenuation = 1.0 / (lights[i].constant + lights[i].linear * dist + lights[i].quadratic * dist * dist);
		vec3 ambient = lights[i].ambient * fragColor * attenuation;
		vec4 l = normalize(vec4(lights[i].positionEyeCoord, 1.0) - positionEyeCoord);
		vec3 diffuse = lights[i].diffuse * fragColor * max(0.0, dot(vec3(l), normalEyeCoord)) * attenuation;
		result += ambient;
		result += diffuse;
	}
	
	outColor = vec4(result, 1.0);
}