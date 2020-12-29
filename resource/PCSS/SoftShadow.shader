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
out vec4 g_FragPosition; // FragPos from GS (output per emitvertex)

void main()
{
    for(int Face = 0; Face < 6; ++Face)
    {
        gl_Layer = Face; // built-in variable that specifies to which Face we render.
        for(int i = 0; i < 3; ++i) // for 3表示三角形的每个顶点
        { // 计算这个fragment在每个光投影面上，在光视角下的坐标
            g_FragPosition = gl_in[i].gl_Position;
            gl_Position = u_LightSpaceMatrices[Face] * g_FragPosition;
            EmitVertex(); // 计算完fragment，把得到的值发出
        }
        EndPrimitive();
    }
}


#shader fragment
#version 330 core
in vec4 FragPos;
uniform samplerCube u_DepthMap;
uniform float u_zFar;
struct Light
{
    int isOpen; // 是否开启，1表示开启，0表示关闭
    vec3 Position; // 光源位置
    vec4 Color; // 光源颜色
    float Brightness; // 光源亮度
    vec3 Attenuation; // 光源的衰减系数

    float LightSize; // 光源大小，用于PCSS
};
const int MAX_LIGHT_NUM = 10; // 最大光源数目
uniform Light u_Lights[MAX_LIGHT_NUM]; // 光源集合


// 计算blocker平均深度时搜索的范围
// @params:
//    LightSize: 光的大小
//    LightToFrag: Fragment相对于光的坐标
float calBlockerSearchWidth(float LightSize, vec3 LightToFrag)
{
    float CurrentDepth = length(LightToFrag);
    float ClosetDepth = u_zFar * texture(u_DepthMap, LightToFrag).r;
    return LightSize * (CurrentDepth - ClosetDepth) / CurrentDepth; // 遮挡这个Fragment的物体越近，要搜的范围越大
}

uniform sampler1D u_RandomMap; // 噪声图，用于随机采样
// 根据噪声图随机生成一个vec3，范围是[-0.5, 0.5]^3
vec3 genRandDirection(float Seed)
{ // 原本噪声图值的范围是[0, 1]
   float Rand1 = texture(u_RandomMap, fract(Seed)).x;
   float Rand2 = texture(u_RandomMap, fract(Seed + Rand1)).x;
   float Rand3 = texture(u_RandomMap, fract(Seed + Rand1 * Rand2)).x;
   return vec3(Rand1, Rand2, Rand3) - vec3(1.0);
}

// 计算blocker的平均深度
// @params:
//    LightSize: 光的大小
//    LightToFrag: Fragment相对于光的坐标
//    Bias: 深度比较允许的偏差值
uniform int u_BlockerSampleNum; // Blocker采样数目
float calBlockerAvgDistance(float LightSize, vec3 LightToFrag, float Bias)
{
    int BlockerNum = 0; // 采样点中，blocker的数目（即比采样点更接近光的点）
    float BlockerDistSum = 0.0;
    float CurrentDepth = length(LightToFrag);
    const float Factor = 1.0f; // 调整范围的常数
    float SearchWidth = calBlockerSearchWidth(LightSize, LightToFrag) * Factor;
    for (int i = 0;i < u_BlockerSampleNum; ++i)
    {
        // 在Depth Map中找到采样点的深度
        float SampleDepth = u_zFar * texture(u_DepthMap, LightToFrag + SearchWidth *
                                            genRandDirection(abs(CurrentDepth * i * LightToFrag.x))).r;
        if (SampleDepth + Bias < CurrentDepth)
        { // 采样点深度小于Fragment，所以是blocker
            BlockerNum ++;
            BlockerDistSum += SampleDepth / u_zFar; // 深度要归一化到[0, 1]
        }
    }
    if (BlockerNum > 0) return BlockerDistSum / BlockerNum;
    return -1;  // 表示没有blocker，Fragment被光线直射
}


// 计算半影的宽度，用于之后的PCF采样
// @params:
//    LightSize: 光的大小
//    LightToFrag: Fragment相对于光的坐标
float calPenumbraWidth(float LightSize, vec3 LightToFrag)
{
    float Bias = 0.12;
    float BlockerAvgDistance = calBlockerAvgDistance(LightSize, LightToFrag, Bias);

    if (BlockerAvgDistance < 0) return -1.0f; // 毫无疑问没有遮挡，不需要PCF采样

    // 半影宽度计算公式，由nvidia文章给出
    float PenumbraWidth = (length(LightToFrag) - BlockerAvgDistance) * LightSize / BlockerAvgDistance;

    return PenumbraWidth;
}


// 用PCF计算阴影值，0表示被光直射，1表示完全阴影
// @params:
//    LightSize: 光的大小
//    LightToFrag: Fragment相对于光的坐标
uniform int u_PCFSampleNum; // PCF采样点的数目
float calSoftShadow(float LightSize, vec3 LightToFrag)
{
    const float Factor = 1.5f;
    float PCFWidth = calPenumbraWidth(LightSize, LightToFrag) * Factor;
    float Sum = 0.0;
    float Bias = 0.12;
    float CurrentDepth = length(LightToFrag);
    if (PCFWidth <= 0.0f)
    { // 毫无疑问没有遮挡
        return 0.0f;
    }

    for (int i = 0;i < u_PCFSampleNum; ++i)
    { // 常规的PCF流程
       float ClosetDepth = u_zFar * texture(u_DepthMap, LightToFrag + PCFWidth * genRandDirection(i / float(u_PCFSampleNum))).r;
       Sum += (ClosetDepth + Bias < CurrentDepth) ? 1.0 : 0.0;
    }
    return Sum / u_PCFSampleNum;
}



void main()
{
    // 写入soft shadow map
    gl_FragDepth = calSoftShadow(u_Lights[0].LightSize, g_FragPosition - u_Lights[0].Position);
}