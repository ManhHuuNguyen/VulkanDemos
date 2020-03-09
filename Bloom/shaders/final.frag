#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform sampler2D blurImage;
layout (binding = 1) uniform sampler2D sceneImage;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
	const float gamma = 2.2;
	const float exposure = 1.0;
	vec3 hdrColor = vec3(texture(sceneImage, fragTexCoord));
	vec3 bloomColor = vec3(texture(blurImage, fragTexCoord));
	hdrColor += bloomColor; //addititve blending
	//tonemapping
	vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
	//gamma correction
	result = pow(result, vec3(1.0/gamma));
	outColor = vec4(result, 1.0);	
}