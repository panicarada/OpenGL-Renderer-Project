#shader vertex
#version 330 core
layout(location = 0) in vec4 Position;
layout(location = 1) in vec4 Normal;
layout(location = 2) in vec4 Color;
layout(location = 3) in vec2 TexCoord;

uniform mat4 u_Model;
uniform mat4 u_Projection;
uniform mat4 u_View;

out vec4 v_Color;
out vec3 v_Normal;
out vec3 v_FragPosition;
out vec2 v_TexCoord;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * Position;
	v_FragPosition = vec3(u_Model * Position);
	v_Color = Color;
	v_Normal = mat3(transpose(inverse(u_Model))) * vec3(Normal);
	v_TexCoord = TexCoord;
}


	#shader fragment
	#version 330 core
layout(location = 0) out vec4 FragColor;

in vec4 v_Color;
in vec3 v_Normal;
in vec3 v_FragPosition;

struct Material
{
	vec4 Ambient;
	vec4 Diffuse;
	vec4 Specular;
	float Highlight;
};
uniform Material u_Material;

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
// uniform int u_LightsNum; // 光源数目
uniform vec4 u_Ambient; // 环境光
uniform vec3 u_CameraPosition; // 相机位置
uniform int u_TexIndex; // 使用的纹理下标
uniform samplerCube u_DepthMap;
in vec2 v_TexCoord; // 这个Vertex对应到纹理上的坐标
uniform float u_zNear;
uniform float u_zFar;


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
uniform int u_BlockerSampleNum; // 采样数目
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
	if (BlockerNum == u_BlockerSampleNum) return -1.0; // 完全被遮挡
	else return BlockerDistSum / BlockerNum;
}

// 计算半影的宽度，用于之后的PCF采样
// @params:
//    LightSize: 光的大小
//    LightToFrag: Fragment相对于光的坐标
float calPenumbraWidth(float LightSize, vec3 LightToFrag)
{
	float Bias = 0.08;
	float BlockerAvgDistance = calBlockerAvgDistance(LightSize, LightToFrag, Bias);

	if (BlockerAvgDistance < 0) return -1.0f; // 被完全遮挡，不需要PCF采样

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
	float ClosetDepth = u_zFar * texture(u_DepthMap, LightToFrag).r;
	float CurrentDepth = length(LightToFrag);
//	const float Bias = 0.02;
	if (CurrentDepth <= ClosetDepth + 0.008)
	{ // 被光直射
		return 0.0f;
	}
	const float Factor = 5.0f;
	float PCFWidth = calPenumbraWidth(LightSize, LightToFrag) * Factor;
	float Sum = 0.0;
	float Bias = 0.12;
	if (PCFWidth <= 0.0f)
	{ // 被完全遮挡
		return 1.0f;
	}

	for (int i = 0;i < u_PCFSampleNum; ++i)
	{ // 常规的PCF流程
		ClosetDepth = u_zFar * texture(u_DepthMap, LightToFrag + PCFWidth * genRandDirection(i / float(u_PCFSampleNum))).r;
		Sum += (ClosetDepth + Bias < CurrentDepth) ? 1.0 : 0.0;
	}
	return Sum / u_PCFSampleNum;
}

void main()
{
	vec3 Norm = normalize(v_Normal);
	vec3 LightDirection[MAX_LIGHT_NUM];
	// 考虑光的衰减
	float LightAttenuation[MAX_LIGHT_NUM];

	// diffuse
	vec4 Diffuse = vec4(0.0f);
	// specular
	float SpecularStrength = 0.5f; // 反射强度
	vec4 Specular = vec4(0.0f);
	vec3 ViewDirection = normalize(u_CameraPosition - v_FragPosition);
	// 为了提速，Diffuse和Specular放到一个for循环
	for (int i = 0;i < 1; ++i)
	{
		if (u_Lights[i].isOpen != 0)
		{
			vec3 tempVec = u_Lights[i].Position - v_FragPosition; // 光源到fragment的向量
			LightDirection[i] = normalize(tempVec);
			float distance = length(tempVec);

			// 衰减因子
			LightAttenuation[i] = 1.0 / (u_Lights[i].Attenuation[0] +
			u_Lights[i].Attenuation[1] * distance +
			u_Lights[i].Attenuation[2] * distance * distance);

			float Diff = max(dot(Norm, LightDirection[i]), 0.0f);
			Diffuse += Diff * u_Lights[i].Color * u_Lights[i].Brightness * LightAttenuation[i];

			// Specular
			vec3 ReflectDir = reflect(-LightDirection[i], Norm);
			float Spec = pow(max(dot(ViewDirection, ReflectDir), 0.0), u_Material.Highlight);
			Specular += SpecularStrength * Spec * u_Lights[i].Color * u_Lights[i].Brightness * LightAttenuation[i];
		}
	}
	vec4 Color = v_Color;
	// 计算阴影
	float Shadow = calSoftShadow(u_Lights[0].LightSize, v_FragPosition - u_Lights[0].Position);
	//	Shadow = 0.0f;
	FragColor = (u_Ambient + (Diffuse + Specular) * (1.0 - Shadow)) * v_Color;
	//	FragColor = vec4(Shadow);
}