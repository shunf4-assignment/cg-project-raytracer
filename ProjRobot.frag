#version 430 core
in vec3 vColor;
out vec4 finalColor;

void main()
{
	finalColor = vec4(vColor, 1.0f);
}