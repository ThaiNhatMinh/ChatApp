#version 140

uniform mat4 Proj;
in vec2 Position;
in vec2 UV;

out vec2 Frag_UV;

void main()
{
   	Frag_UV = UV;
	gl_Position = Proj*vec4(Position,0,1);
}