#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(binding = 0) uniform PerLight {
    mat4 lightSpaceMatrix; // projectionMat * view mat with light at origin
} perLight;

layout (binding = 1) uniform PerObject 
{
	mat4 modelMatrix; 
} perObject;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

void main() {
    gl_Position = perLight.lightSpaceMatrix * perObject.modelMatrix * vec4(inPosition, 1.0);
}