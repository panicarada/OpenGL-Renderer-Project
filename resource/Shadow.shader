#shader vertex
// 几何着色器是负责将所有世界空间的顶点变换到6个不同的光空间的着色器。
// 因此顶点着色器简单地将顶点变换到世界空间，然后直接发送到几何着色器：
#version 330 core
layout(location = 0) in vec4 Position;
uniform mat4 u_model;
void main()
{
    gl_Position = u_model * Position;
}


#shader geometry
#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 u_ShadowMatrices[6];
out vec4 FragPosition; // 从几何着色器输出的Fragment着色器中的坐标 (对Vertex着色器输出的每个Vertex都这样处理)
void main()
{
    for (int Face = 0; Face < 6; ++Face)
    {
    	gl_Layer = Face; // 几何着色器内置变量，声明现在要渲染哪个面
    	for (int i  = 0;i < 3; ++i)
    	{ // 对这个面每个顶点
    		FragPosition = gl_in[i].gl_Position;
    		gl_Position = u_ShadowMatrices[Face] * FragPosition;
    		EmitVertex();
    	}
    	EndPrimitive();
    }
}



#shader fragment
#version 330 core
// 计算深度
// 这个深度就是每个fragment位置和光源位置之间的线性距离
in vec4 FragPosition;
struct Light
{
    int isOpen; // 是否开启，1表示开启，0表示关闭
    vec3 Position; // 位置
};
const int MAX_LIGHT_NUM = 10; // 最大光源数目
uniform Light u_Lights[MAX_LIGHT_NUM]; // 光源集合
uniform float u_zFar; // 定义相机的View Matrix时，设置的zFar

void main()
{
	for (int i = 0; i < MAX_LIGHT_NUM; ++i)
    {
        if (u_Lights[i].isOpen != 0)
        {
            float Distance = length(FragPosition.xyz - u_Lights[i].Position);
            // 把深度映射到[0, 1]上，并写入Buffer
            gl_FragDepth = Distance / u_zFar;
        }
    }
}