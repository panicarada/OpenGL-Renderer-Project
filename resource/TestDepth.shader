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
out vec4 FragColor;

float near = 0.1; 
float far  = 100.0; 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

void main()
{             
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // 为了演示除以 far
    FragColor = vec4(vec3(depth), 1.0);
}