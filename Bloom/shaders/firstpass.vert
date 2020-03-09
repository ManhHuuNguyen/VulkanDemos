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
layout(location = 1) out vec4 positionEyeCoord;
layout(location = 2) out vec3 normalEyeCoord;


void main() {
	fragColor = perObject.color;
	mat4 modelView = perCamera.view * perObject.modelMatrix;
	positionEyeCoord = modelView * vec4(inPosition, 1.0);
	mat3 normalMatrix = mat3(transpose(inverse(perObject.modelMatrix)));
	normalEyeCoord =  normalize(mat3(perCamera.view) * normalMatrix * inNormal);
    gl_Position = perCamera.proj * modelView * vec4(inPosition, 1.0);
}

