#shader vertex
#version 330 core
layout(location = 0) in vec4 Position;
layout(location = 1) in vec4 Color;

out vec4 v_Color;

uniform mat4 u_MVP; // model view projection matrix

void main()
{
   gl_Position = u_MVP * Position;
   v_Color = Color;
}


#shader fragment
#version 330 core
layout(location = 0) out vec4 o_Color;


in vec4 v_Color;

void main()
{
	o_Color = v_Color;
}