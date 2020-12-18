#shader vertex
#version 330 core
layout(location = 0) in vec4 Position;
layout(location = 1) in vec4 Normal;
layout(location = 2) in vec4 Color;

uniform mat4 u_model;
uniform mat4 u_projection;
uniform mat4 u_view;

out vec4 v_Color;
out vec3 v_Normal;
out vec3 FragPosition;

void main()
{
   gl_Position = u_projection * u_view * u_model * Position;
   FragPosition = vec3(u_model * Position);
   v_Color = Color;
   // v_Normal = vec3(Normal);
   v_Normal = mat3(transpose(inverse(u_model))) * vec3(Normal);
}




#shader fragment
#version 330 core
layout(location = 0) out vec4 FragColor;

in vec4 v_Color;
in vec3 v_Normal;
in vec3 FragPosition;

uniform vec3 u_LightPosition; // 光源位置
uniform vec4 u_LightColor; // 光源颜色
uniform vec4 u_Ambient; // 环境光
uniform vec3 u_CameraPosition; // 相机位置

void main()
{
	vec3 Norm = normalize(v_Normal);
	vec3 LightDirection = normalize(u_LightPosition - FragPosition);
	float Diff = max(dot(Norm, LightDirection), 0.0f);
	vec4 Diffuse = Diff * u_LightColor;

	// specular
	float SpecularStrength = 0.5f; // 反射强度
	vec3 ViewDirection = normalize(u_CameraPosition - FragPosition);
	vec3 ReflectDir = reflect(-LightDirection, Norm);
	float Spec = pow(max(dot(ViewDirection, ReflectDir), 0.0), 32);
	vec4 Specular = SpecularStrength * Spec * u_LightColor;



	FragColor = (u_Ambient + Diffuse + Specular) * v_Color;
}