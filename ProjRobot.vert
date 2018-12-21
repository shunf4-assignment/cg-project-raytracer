#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
out vec3 vColor;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float dim;

void main()
{
	gl_Position = Projection * View * Model * vec4(position, 1.0);
	vColor = dim * color;
}