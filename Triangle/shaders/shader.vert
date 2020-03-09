#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec4 positionEyeCoord;
layout(location = 2) out vec4 normalEyeCoord;

void main() {
	fragColor = inColor;
	mat4 modelView = ubo.view * ubo.model;
	positionEyeCoord = modelView * vec4(inPosition, 1.0);
	mat4 normalMatrix = transpose(inverse(ubo.model));
	normalEyeCoord = normalize(ubo.view * normalMatrix * vec4(inPosition, 1.0));
    gl_Position = ubo.proj * modelView * vec4(inPosition, 1.0);
}