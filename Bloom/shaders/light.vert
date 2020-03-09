#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform PerCamera {
    mat4 view;
    mat4 proj;
} perCamera;

layout (binding = 1) uniform PerObject 
{
	mat4 modelMatrix; 
	vec3 color;
} perObject;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;

void main() {
	fragColor = perObject.color;
    gl_Position = perCamera.proj * perCamera.view * perObject.modelMatrix * vec4(inPosition, 1.0);
}