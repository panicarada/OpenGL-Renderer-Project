#shader vertex
#version 330 core
layout(location = 0) in vec4 Position;
layout(location = 1) in vec4 Normal;
layout(location = 2) in vec4 Color;

uniform mat4 u_model;
uniform mat4 u_projection;
uniform mat4 u_view;
 
void main()
{
   gl_Position = u_model * vec4(Position.x, Position.y, Position.z, 1.0);
}

#version 330 core
layout (location = 0) in vec4 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec4 Color;

out vec3 FragPosition;
out vec3 v_Normal;
out vec4 v_Color;

uniform mat4 u_model;
uniform mat4 u_projection;
uniform mat4 u_view;

void main()
{
    gl_Position = u_projection * u_view * u_model * vec4(position);
    FragPosition = vec3(u_model * vec4(Position, 1.0));
    v_Normal = transpose(inverse(mat3(u_model))) * Normal;
    v_Color = Color;
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
in vec3 FragPosition;
in vec3 v_Normal;
in vec4 v_Color;


layout(location = 0) out vec4 FragColor;

uniform vec3 u_LightPosition;
uniform vec3 u_CameraPosition; // 相机位置
uniform vec4 u_LightColor; // 光源颜色
uniform float u_zFar;

uniform samplerCube depthMap;

float ShadowCalculation(vec3 fragPos)
{
    // Get vector between fragment position and light position
    vec3 FragToLight = FragPosition - u_LightPos;
    // Use the light to fragment vector to sample from the depth map    
    float ClosetDepth = texture(depthMap, FragToLight).r;


    // It is currently in linear range between [0,1]. Re-transform back to original value
    ClosetDepth *= u_zFar;
    // Now get current linear depth as the length between the fragment and light position
    float CurrentDepth = length(FragToLight);
    // Now test for shadows
    float Bias = 0.05; 
    // 看看哪个阴影更加接近，从而选择保留
    float Shadow = currentDepth -  Bias > closestDepth ? 1.0 : 0.0;

    return Shadow;
}


void main()
{
	vec3 Normal = normalize(v_Normal);

	// 环境光
	vec4 Ambient = 0.3 * v_Color;

	// 满射光
	vec3 LightDirection = normalize(u_LightPosition - FragPosition);
	float Diff = max(dot(LightDirection, Normal), 0.0f);
	vec3 Diffuse = Diff * u_LightColor;

	// 反射光
	vec3 ViewDirection = normalize(ViewPosition - FragPosition);
	vec3 ReflectDirection = reflect(-LightDirection, Normal);
	float Spec = 0.0f;
	vec3 HalfwayDirection = normalize(LightDirection + ViewDirection);
	Spec = pow(max(dot(normal, HalfwayDirection), 0.0), 64.0);
	vec3 Specular = Spec * u_LightColor;

	// 计算阴影
	float Shadow = ShadowCalculation(FragPosition);
	FragColor = (Ambient + (1.0f - Shadow) * (Diffuse + Specular)) * v_Color;
}