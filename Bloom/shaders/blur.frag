#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform sampler2D image;
layout (constant_id = 0) const int horizontal = 0;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
	float weights[5];
	weights[0] = 0.227027;
	weights[1] = 0.1945946;
	weights[2] = 0.1216216;
	weights[3] = 0.054054;
	weights[4] = 0.016216;
	vec2 tex_offset = 1.0 / textureSize(image, 0);
	vec4 result = texture(image, fragTexCoord) * weights[0];
	if (horizontal == 1) {
		for (int i = 1; i < 5; i++) {
			result += texture(image, fragTexCoord + vec2(tex_offset.x  * i, 0.0)) * weights[i];
			result += texture(image, fragTexCoord - vec2(tex_offset.x  * i, 0.0)) * weights[i];
		}
	}
	else {
		for (int i = 1; i < 5; i++) {
			result += texture(image, fragTexCoord + vec2(0.0, tex_offset.y * i)) * weights[i];
			result += texture(image, fragTexCoord - vec2(0.0, tex_offset.y * i)) * weights[i];
		}
	}
	outColor = vec4(vec3(result), 1.0);
}