#version 430 core
layout (location = 0) in vec3 position;

out vec2 vTexCoordinate;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;


vec3 color_orig[4] = vec3[] (vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0), vec3(1.0, 1.0, 1.0));

void main()
{
	gl_Position = Projection * View * Model * vec4(position, 1.0);
	vTexCoordinate = position.xy + 0.5;
}