#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoordinate;
out vec2 vTexCoordinate;
out vec3 vColor;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

vec3 color_orig[4] = vec3[] (vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0), vec3(1.0, 1.0, 1.0));

void main()
{
	gl_Position = Projection * View * Model * vec4(position, 1.0);
	vColor = color_orig[gl_VertexID];
	//vColor = vec3(1.0, 1.0, 1.0);
	vTexCoordinate = texCoordinate;
}