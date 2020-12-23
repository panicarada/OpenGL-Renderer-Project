//
// Created by 邱泽鸿 on 2020/12/13.
//

#pragma once

#include "Basic.h"
#include <fstream>
#include <sstream>
#include <unordered_map>

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
    std::string GeometrySource;
};

class Shader
{
private:
    std::string m_FilePath;
    unsigned int m_RendererID;

    // caching for uniforms
    std::unordered_map<std::string, int> m_UniformLocationCache;
public:
    Shader(const std::string& Filepath);
    ~Shader();

    void bind() const;
    void unbind() const;
    inline unsigned int getRendererID() const
    {
        return m_RendererID;
    }

    void setUniform1i(const std::string Name, int Value);
    void setUniform1iv(const std::string Name, const int Count, const int *Value);
    void setUniform1f(const std::string Name, float Value);
    inline void setUniform1fv(const std::string Name, const std::vector<float>& Value)
    {
        DebugCall(glUniform1fv(getUniformLocation(Name), Value.size(), &Value[0]));
    }
    void setUniform3f(const std::string Name, float v0, float v1, float v2);
    void setUniform3f(const std::string Name, const glm::vec3& v)
    {
        setUniform3f(Name, v[0], v[1], v[2]);
    }
    inline void setUniform3fv(const std::string Name, const std::vector<glm::vec3> &Value)
    {
        DebugCall(glUniform3fv(getUniformLocation(Name), Value.size(), &Value[0][0]));
    }
    void setUniform4fv(const std::string Name, std::vector<glm::vec4>& Value)
    {
        DebugCall(glUniform4fv(getUniformLocation(Name), Value.size(), &Value[0][0]));
    }

    void setUniform4f(const std::string Name, float v0, float v1, float v2, float v3);
    inline void setUniform4f(const std::string Name, const glm::vec4& v)
    {
        DebugCall(glUniform4f(getUniformLocation(Name), v[0], v[1], v[2], v[3]));
    }
    void setUniformMat4f(const std::string Name, glm::mat4& matrix);

private:
    int getUniformLocation(const std::string& name);
    ShaderProgramSource parseShader(const std::string &File);
    unsigned int compileShader(unsigned int type, const std::string &source);
    unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader, const std::string &geometryShader);
};