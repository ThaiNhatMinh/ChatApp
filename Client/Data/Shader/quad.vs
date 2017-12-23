#version 140


in vec2 Position;
in vec2 UV;

out vec2 Frag_UV;
out vec4 Frag_Color;

void main()
{
   	Frag_UV = UV;


	gl_Position = vec4(Position,0,1);
}
