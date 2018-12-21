#version 430
in vec2 vTexCoordinate;
out vec4 finalColor;

uniform sampler2D sceneTexture;

void main()
{
	vec4 texel = texture(sceneTexture, vTexCoordinate);
	finalColor = texel;
}