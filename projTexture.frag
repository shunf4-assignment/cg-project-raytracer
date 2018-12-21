#version 430
in vec2 vTexCoordinate;
in vec3 vColor;
out vec4 finalColor;

uniform sampler2D basic_texture;

void main()
{
	vec4 texel = texture2D(basic_texture, vTexCoordinate);
	//finalColor = vec4(vColor, 1.0f);
	finalColor = texel; 
}
