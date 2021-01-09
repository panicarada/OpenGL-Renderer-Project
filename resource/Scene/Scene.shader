#shader vertex
#version 330 core
layout(location = 0) in vec4 Position;
layout(location = 1) in vec4 Normal;
// layout(location = 2) in vec4 Color;
layout(location = 3) in vec2 TexCoord;

uniform mat4 u_Model;
uniform mat4 u_Projection;
uniform mat4 u_View;

// out vec4 v_Color;
out vec3 v_Normal;
out vec3 v_FragPosition;
out vec2 v_TexCoord;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * Position;
	v_FragPosition = vec3(u_Model * Position);
	// v_Color = Color;
	v_Normal = mat3(transpose(inverse(u_Model))) * vec3(Normal);
	v_TexCoord = TexCoord;
}


#shader fragment
#version 330 core
layout(location = 0) out vec4 FragColor;

// in vec4 v_Color;
uniform vec4 u_Color;
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
};
const int MAX_LIGHT_NUM = 2; // 最大光源数目
uniform Light u_Lights[MAX_LIGHT_NUM]; // 光源集合
// uniform int u_LightsNum; // 光源数目
uniform vec4 u_Ambient; // 环境光

uniform vec3 u_CameraPosition; // 相机位置


uniform int u_TexIndex; // 使用的纹理下标
uniform sampler2DArray u_Textures; // 纹理数组
const int MAX_TEX_NUM = 15; // 最多的纹理数

//uniform samplerCube u_DepthMap[MAX_LIGHT_NUM];
uniform samplerCube u_DepthMap_0;
uniform samplerCube u_DepthMap_1;
uniform samplerCube u_DepthMap_2;


in vec2 v_TexCoord; // 这个Vertex对应到纹理上的坐标

uniform float u_zFar;



// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash( uint x ) {
	x += ( x << 10u );
	x ^= ( x >>  6u );
	x += ( x <<  3u );
	x ^= ( x >> 11u );
	x += ( x << 15u );
	return x;
}

// Compound versions of the hashing algorithm I whipped together.
uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y)                         ); }
uint hash( uvec3 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z)             ); }
uint hash( uvec4 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w) ); }



const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct( uint m )
{
	m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
	m |= ieeeOne;                          // Add fractional part to 1.0

	float  f = uintBitsToFloat( m );       // Range [1:2]
	return f - 1.0;                        // Range [0:1]
}

// Pseudo-random value in half-open range [0:1].
float random( float x ) { return floatConstruct(hash(floatBitsToUint(x)));}
float random( vec3  v ) { return floatConstruct(hash(floatBitsToUint(v))); }

//const int SampleNum = 27;
uniform int u_SampleNum;
const int MAX_SAMPLE_NUM = 27;
uniform vec3 u_SampledPoints[MAX_SAMPLE_NUM];
uniform float u_SampleImportance[MAX_SAMPLE_NUM];
//uniform float u_SampleImportanceSum;
uniform float u_SampleArea; // 采样范围
float calculateShadow(samplerCube DepthMap, vec3 FragPosition, vec3 LightPos)
{
	// 光线到fragment的向量
	vec3 LightToFrag = FragPosition - LightPos;

	// 该gragment到光的距离就是其深度
	float CurrentDepth = length(LightToFrag);
	float Shadow = 0.0;
	float Bias = 0.12;
	float ViewDistance = length(u_CameraPosition - FragPosition);

	float DiskRadius = (1.0 + (ViewDistance / u_zFar)) * u_SampleArea;

	vec3 Noise = 2.5 * (vec3(random(LightToFrag.x), random(CurrentDepth), random(ViewDistance)) - 0.5);
	// 把for循环展开一点，可以加速
	float SampleImportanceSum = 0.0;
	if (u_SampleNum == 0)
	{ // 没有阴影
		return 0.0;
	}
	for(int i = 0; i < u_SampleNum; i ++)
	{
		// 用LightToFrag采样在光视角下该fragment对应位置最近的深度（理论上如果该点被光直射，则最近深度就是它自己的深度）
		float ClosestDepth = texture(DepthMap, LightToFrag + // 加上一个随机噪声
									(u_SampledPoints[i] + Noise) * DiskRadius).r;
		ClosestDepth *= u_zFar;   // 计算深度时，我们归一到了[0, 1]，现在再展开
		if(CurrentDepth - Bias > ClosestDepth)
		{ // 如果它就是最近深度，则被阴影覆盖，因为有多个采样点，所以阴影值加权平均
			Shadow += u_SampleImportance[i];
		}
		SampleImportanceSum += u_SampleImportance[i];
	}
	// 加权平均
	Shadow /= SampleImportanceSum;

	return Shadow;
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
	vec4 Lighting = vec4(0.0);
	// 为了提速，Diffuse和Specular放到一个for循环
	int i = 0;
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

		// 计算阴影
		float Shadow = calculateShadow(u_DepthMap_0, v_FragPosition, u_Lights[i].Position);
		Lighting += (Diffuse * u_Material.Diffuse + Specular * 0.5 * u_Material.Specular) * (1.0 - Shadow);
	}
	i = 1;
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

		// 计算阴影
		float Shadow = calculateShadow(u_DepthMap_1, v_FragPosition, u_Lights[i].Position);
		Lighting += (Diffuse * u_Material.Diffuse + Specular * 0.5 * u_Material.Specular) * (1.0 - Shadow);
	}

	Lighting = clamp(Lighting, 0.0, 1.0);

	// vec4 Color = v_Color;
	vec4 Color = u_Color;
	// 如果有纹理
	if (u_TexIndex >= 0 && u_TexIndex < MAX_TEX_NUM)
	{
		Color = texture(u_Textures, vec3(v_TexCoord, u_TexIndex));
	}
	FragColor = (u_Ambient * 0.5 * u_Material.Ambient + Lighting) * Color;
}
