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

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;


void main() {
	float depthVal = texture(depthMap, fragTexCoord).r;
	outColor = vec4(vec3(depthVal), 1.0);
}

