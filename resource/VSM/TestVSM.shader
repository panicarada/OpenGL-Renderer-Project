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

in vec2 v_TexCoord; // 这个Vertex对应到纹理上的坐标
uniform float u_zNear;
uniform float u_zFar;

uniform samplerCube u_VSMShadowMap;



float LinearStep(float Low, float High, float Value)
{
	return min(max((Value - Low)/(High - Low), 0.0), 1.0);
}

float calVSM_Shadow(vec3 LightToFrag)
{
	vec2 Moments = texture(u_VSMShadowMap, LightToFrag).rg;
	float d = length(LightToFrag) / u_zFar; // 归一化
	// Surface is fully lit. as the current fragment is before the light occluder
	float Bias = 0.005;
	if (d <= Moments.x + Bias)
	return 0.0;

	// The fragment is either in shadow or penumbra. We now use chebyshev's upperBound to check
	// How likely this pixel is to be lit (p_max)
	float Variance = Moments.y - (Moments.x * Moments.x);
	Variance = u_zFar * max(Variance, 0.0002); // 避免为0
	float p = step(d, Moments.x);

	float d_minus_mean = d - Moments.x;
	float pMax = LinearStep(0.2, 1.0, Variance / (Variance + d_minus_mean * d_minus_mean));
//	float p_max = Variance / (Variance + d_minus_mean * d_minus_mean);

//	return p_max;
	return min(max(p, pMax), 1.0);
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
	float Shadow = calVSM_Shadow(v_FragPosition - u_Lights[0].Position);
	FragColor = (u_Ambient + (Diffuse + Specular) * (1.0 - Shadow)) * v_Color;
//	FragColor = vec4(Shadow);
}