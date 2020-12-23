#shader vertex
#version 330 core
layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;

out vec2 v_TexCoord;

uniform mat4 u_MVP; // model view projection matrix

void main()
{
   gl_Position = Position;
   v_TexCoord = TexCoord;
}





#shader fragment
#version 330 core
layout(location = 0) out vec4 o_Color;
in vec2 v_TexCoord;

uniform int u_TexIndex;
uniform sampler2D u_Textures[15];


in vec4 v_Color;

void main()
{
	o_Color = texture(u_Textures[u_TexIndex], v_TexCoord);
}