#shader vertex
// 几何着色器是负责将所有世界空间的顶点变换到6个不同的光空间的着色器。
// 因此顶点着色器简单地将顶点变换到世界空间，然后直接发送到几何着色器：
#version 330 core
layout(location = 0) in vec4 Position;

uniform mat4 u_Model;
void main()
{
    gl_Position = u_Model * Position;
}


#shader geometry
#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;
uniform mat4 u_LightSpaceMatrices[6];
//
out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main()
{

    for(int Face = 0; Face < 6; ++Face)
    {
        gl_Layer = Face; // built-in variable that specifies to which Face we render.
        for(int i = 0; i < 3; ++i) // for 3表示三角形的每个顶点
        { // 计算这个fragment在每个光投影面上，在光视角下的坐标
            FragPos = gl_in[i].gl_Position;
            gl_Position = u_LightSpaceMatrices[Face] * FragPos;
            EmitVertex(); // 计算完fragment，把得到的值发出
        }
        EndPrimitive();
    }
}



#shader fragment
#version 330 core
in vec4 FragPos;

out vec2 FragColor;

uniform vec3 u_LightPosition;
uniform float u_zFar;

void main()
{
    // 点到光源的距离就是原始的深度
    float Depth = length(FragPos.xyz - u_LightPosition);
    // 归一化到[0, 1]
    Depth = Depth / u_zFar;
    // 一阶矩和二阶矩
    vec2 Moments = vec2(Depth, Depth * Depth);

    float dx = dFdx(Depth);
    float dy = dFdy(Depth);
    Moments += vec2(0.0, 0.25 * (dx * dx + dy * dy)); // 解决acne问题

    FragColor = Moments;
}