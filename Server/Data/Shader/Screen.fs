#version 140

uniform sampler2D Texture;

in vec2 Frag_UV;

out vec4 Out_Color;

void main()
{
	Out_Color = texture( Texture, Frag_UV.st);
	//Out_Color = vec4(0,1,0,1);
}