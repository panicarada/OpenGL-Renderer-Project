#shader vertex
#version 330 core
layout(location = 0) in vec4 Position;
layout(location = 1) in vec4 Normal;
layout(location = 2) in vec4 Color;
layout(location = 3) in vec2 TexCoord;

uniform mat4 u_Model;
uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_LightSpaceMatrix;

out vec4 v_Color;
out vec3 v_Normal;
out vec3 v_FragPosition;

out vec4 v_FragPosLightSpace; 

out vec2 v_TexCoord;

out mat4 debug_v_LightSpaceMatrix;

void main()
{
   gl_Position = u_Projection * u_View * u_Model * Position;
   v_FragPosition = vec3(u_Model * Position);
   v_Color = Color;
   // v_Normal = vec3(Normal);
   v_Normal = mat3(transpose(inverse(u_Model))) * Normal.xyz;

   v_TexCoord = TexCoord;

   v_FragPosLightSpace = u_LightSpaceMatrix  * vec4(v_FragPosition, 1.0);

   debug_v_LightSpaceMatrix = u_LightSpaceMatrix;
}




#shader fragment
#version 330 core
out vec4 FragColor;

in vec4 v_Color;
in vec3 v_Normal;
in vec2 v_TexCoord;
in vec3 v_FragPosition;
in vec4 v_FragPosLightSpace;

in mat4 debug_v_LightSpaceMatrix;

uniform sampler2D u_DepthMap;
uniform float u_zNear;
uniform float u_zFar;


struct Light
{
	int isOpen; // 是否开启，1表示开启，0表示关闭
	vec3 Position; // 光源位置
	vec4 Color; // 光源颜色
	float Brightness; // 光源亮度
	vec3 Attenuation; // 光源的衰减系数
};
const int MAX_LIGHT_NUM = 10; // 最大光源数目
uniform Light u_Lights[MAX_LIGHT_NUM]; // 光源集合

uniform vec4 u_Ambient; // 环境光
uniform vec3 u_CameraPosition; // 相机位置

float max3(vec3 v)
{ // 求最大值的分量值
	return max(v.x, max(v.y, v.z));
}
float min3(vec3 v)
{ // 求最小值的分量值
	return min(v.x, min(v.y, v.z));
}


float calculateShadow(vec4 FragPosLightSpace)
{
	// vec3 ProjCoords = FragPosLightSpace.xyz * 0.5 + 0.5;
	// vec3 ProjCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
	// 规约到[0, 1]
	// ProjCoords = ProjCoords/2.0 + 0.5;
	// float ClosetDepth = texture(u_DepthMap, ProjCoords.xy).r;
	// float CurrentDepth = ProjCoords.z;
	// float Shadow = CurrentDepth > ClosetDepth ? 1.0 : 0.0;

	// 把范围拉到[0, 1]
	float minValue = min3(FragPosLightSpace.xyz);
	float maxValue = max3(FragPosLightSpace.xyz);

	vec3 Position = (FragPosLightSpace.xyz - minValue) / (maxValue - minValue);
	float Depth = texture(u_DepthMap, Position.xy).r;
	return Depth;
	// return Depth < FragPosLightSpace.z ? 1.0 : 0.0;
}




void main()
{             
    vec3 Normal = normalize(v_Normal);

   	// diffuse
   	vec3 LightDir = normalize(u_Lights[0].Position - v_FragPosition);
   	float Diff = max(dot(LightDir, Normal), 0.0);
   	vec4 Diffuse = Diff * u_Lights[0].Color;

   	// specular 
   	vec3 ViewDir = normalize(u_CameraPosition - v_FragPosition);
   	vec3 ReflectDir = reflect(-LightDir, Normal);
   	float spec = 0.0;
   	vec3 HalfwayDir = normalize(LightDir + ViewDir);
   	spec = pow(max(dot(Normal, HalfwayDir), 0.0), 64.0);
   	vec4 Specular = spec * u_Lights[0].Color;

   	// calculate Shadow
   	float Shadow = calculateShadow(v_FragPosLightSpace);

   	// FragColor = (u_Ambient + (1.0 - Shadow) * (Diffuse + Specular)) * v_Color;
   	FragColor = ((1.0 - Shadow) * (Diffuse + Specular)) * v_Color;



   	// // vec3 Position = v_FragPosLightSpace.xyz / length(v_FragPosLightSpace);
   	// vec3 Position;
   	// Position[0] = v_FragPosLightSpace.x >= -2 ? 1.0 : 0.0;
   	// Position[1] = v_FragPosLightSpace.y  >= -2 ? 1.0 : 0.0;
   	// Position[2] = v_FragPosLightSpace.z  >= -2 ? 1.0 : 0.0;

   	float minValue = min3(v_FragPosLightSpace.xyz);
	float maxValue = max3(v_FragPosLightSpace.xyz);

	vec3 Position = (v_FragPosLightSpace.xyz - vec3(minValue)) / (maxValue - minValue);



	vec4 Color;
   Color = texture(u_DepthMap, Position.xy);
// 
	// float Color;
	// if (abs(Depth) <= 1.0) Color = 0.0f;
	// else Color  = 1.0f;

	// float Color = 0.0f;
	// if (abs(v_FragPosLightSpace.z) < 1.0f) Color = 0.0f;
	// else Color = 1.0f;

   // FragColor = Color;
   	FragColor = vec4(Position, 1.0);
}