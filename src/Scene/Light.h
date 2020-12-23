//
// Created by 邱泽鸿 on 2020/12/18.
//

#pragma once
#include "Basic.h"
#include "Shader.h"
#include <set>

class Light
{
public:
    Light(std::shared_ptr<Shader> shader, int ID = -1, glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4 Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), float Intensity = 1.0f)
        : m_Shader(shader), m_Position(Position), m_Color(Color), m_Intensity(Intensity), m_Brightness(1.0f), m_Attenuation(glm::vec3(1.0f, 0.14f, 0.17f))
    {
        m_ID = *availableSlots.begin(); // 任意获取一个可用的下标
        std::cout << "Light ID: " << m_ID << std::endl;
        availableSlots.erase(m_ID); // 这个下标被占用
        m_Name = "Light";

        // 初始化
        if (!isInited)
        {
            isInited = true;
            for (int i = 0;i < availableSlots.size(); ++i)
            {
                m_Shader->setUniform1i("u_Lights[" + std::to_string(i) + "].isOpen", 0);
            }
        }
        m_Shader->setUniform1i("u_Lights[" + std::to_string(m_ID) + "].isOpen", 1);
    }
    ~Light()
    { // 被删除的下标恢复
        if (m_ID >= 0)
        {
            availableSlots.insert(m_ID);
            // 把对应亮度设为0，这样就不会影响计算
            m_Shader->setUniform1i("u_Lights[" + std::to_string(m_ID) + "].isOpen", 0);
        }
    }
    inline void updateData() const
    {
        if (m_ID >= 0)
        {
            m_Shader->setUniform3f("u_Lights[" + std::to_string(m_ID) + "].Position", m_Position);
            m_Shader->setUniform4f("u_Lights[" + std::to_string(m_ID) + "].Color", m_Color);
            m_Shader->setUniform1f("u_Lights[" + std::to_string(m_ID) + "].Brightness", m_Brightness);
            m_Shader->setUniform3f("u_Lights[" + std::to_string(m_ID) + "].Attenuation", m_Attenuation);
        }
    }
public:
    float m_Brightness;
    std::shared_ptr<Shader> m_Shader;
    int m_ID;
    std::string m_Name;
    glm::vec3 m_Position;
    glm::vec4 m_Color;
    float m_Intensity;
    glm::vec3 m_Attenuation;
private:
    static std::set<int> availableSlots; // 可以用的u_Lights下标池
    static bool isInited; // 是否初始化过
};
