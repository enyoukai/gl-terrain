#version 330

out vec4 fragColor;
in float height;

void main()
{
	fragColor = vec4(height / 35.0f, height / 35.0f, height / 35.0f, 1.0f);
}