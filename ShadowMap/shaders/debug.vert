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

layout(binding = 2) uniform PerLight {
    mat4 lightSpaceMatrix; // projectionMat * view mat with light at origin
} perLight;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec2 fragTexCoord;

vec2 corner_pos[6] = {vec2(1.0, 1.0), vec2(1.0, -1.0), vec2(-1.0, -1.0), vec2(-1.0, 1.0), vec2(1.0, 1.0), vec2(-1.0, -1.0)}; 
vec2 corner_texcoord[6] = {vec2(1.0, 1.0), vec2(1.0, 0.0), vec2(0.0, 0.0), vec2(0.0, 1.0), vec2(1.0, 1.0), vec2(0.0, 0.0)};

void main() {
	gl_Position = vec4(corner_pos[gl_VertexIndex], 0.0, 1.0);
	fragTexCoord = corner_texcoord[gl_VertexIndex];
}

