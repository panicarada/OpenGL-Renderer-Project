//
// Created by 邱泽鸿 on 2020/12/13.
//

#include "Shader.h"

Shader::Shader(const std::string &Filepath)
{
    ShaderProgramSource source = parseShader(Filepath);
    m_RendererID = createShader(source.VertexSource, source.FragmentSource, source.GeometrySource);
}

Shader::~Shader()
{
    DebugCall(glDeleteProgram(m_RendererID));
}

ShaderProgramSource Shader::parseShader(const std::string &File)
{
    std::ifstream stream(File);
    std::string line;
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2
    };
    std::stringstream ss[3];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
            else if (line.find("geometry") != std::string::npos)
            {
                type = ShaderType::GEOMETRY;
            }
        }
        else
        {
            ss[(int)type] << line << "\n";
        }
    }
    return {ss[0].str(), ss[1].str(), ss[2].str()};
}

unsigned int Shader::createShader(const std::string &vertexShader, const std::string &fragmentShader, const std::string &geometryShader)
{ // 已经获取了两个shader的代码，现在载入OpenGL
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);
    unsigned int gs = compileShader(GL_GEOMETRY_SHADER, geometryShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    if (!geometryShader.empty())
    {
        glAttachShader(program, gs);
    }

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);
    if (!geometryShader.empty())
    {
        glDeleteShader(gs);
    }
    return program;
}

unsigned int Shader::compileShader(unsigned int type, const std::string &source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " <<
                  (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
        std::cout << message << std::endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

int Shader::getUniformLocation(const std::string &name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    { // 看看表里有没有查过这个uniform的location
        return m_UniformLocationCache[name];
    }
    else
    {
//        std::cout << name << std::endl;
        DebugCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
        if (location == -1)
            std::cout << "Warning: uniform '" << name << "' doesn't exist currently!   "  << __FILE_NAME__ << ": " << __LINE__ << std::endl;
        m_UniformLocationCache[name] = location;
        return location;
    }
}

void Shader::bind() const
{
//    std::cout << m_RendererID << std::endl;
    DebugCall(glUseProgram(m_RendererID));
}

void Shader::unbind() const
{
    DebugCall(glUseProgram(0));
}

void Shader::setUniform1i(const std::string Name, int Value)
{
    DebugCall(glUniform1i(getUniformLocation(Name), Value));
}

void Shader::setUniform1iv(const std::string Name, const int Count, const int *Value)
{
    DebugCall(glUniform1iv(getUniformLocation(Name), Count, Value));
}

void Shader::setUniform1f(const std::string Name, float Value)
{
    DebugCall(glUniform1f(getUniformLocation(Name), Value));
}

void Shader::setUniform4f(const std::string Name, float v0, float v1, float v2, float v3)
{
    DebugCall(glUniform4f(getUniformLocation(Name), v0, v1, v2, v3));
}

void Shader::setUniformMat4f(const std::string Name, const glm::mat4 &matrix)
{
    // &matrix[0][0]表示第一个元素的地址，之后OpenGL会读取连续的16个数
    DebugCall(glUniformMatrix4fv(getUniformLocation(Name), 1, GL_FALSE, &matrix[0][0]));
}

void Shader::setUniform3f(const std::string Name, float v0, float v1, float v2)
{
    DebugCall(glUniform3f(getUniformLocation(Name), v0, v1, v2));
}