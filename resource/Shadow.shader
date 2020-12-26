#shader vertex
// 几何着色器是负责将所有世界空间的顶点变换到6个不同的光空间的着色器。
// 因此顶点着色器简单地将顶点变换到世界空间，然后直接发送到几何着色器：
#version 330 core
layout(location = 0) in vec4 Position;

uniform mat4 u_LightSpaceMatrix;
uniform mat4 u_Model;
void main()
{
    gl_Position = u_LightSpaceMatrix * u_Model * Position;
}




#shader fragment
#version 330 core
void main()
{
	
}