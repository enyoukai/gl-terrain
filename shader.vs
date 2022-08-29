#version 330

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float height;

void main()
{
	gl_Position = projection * view * model * vec4(aPosition, 1.0f);
	height = aPosition.y;

	Normal = aNormal;
}