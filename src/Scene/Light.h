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
    Light(const std::shared_ptr<Shader>& shader, int ID = -1, glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4 Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), float Intensity = 1.0f)
        : m_Shader(shader), m_Position(Position), m_Color(Color), m_Brightness(1.0f), m_Attenuation(glm::vec3(1.0f, 0.06f, 0.02f))
    {
        // 初始化
        if (!isInited)
        {
            availableSlots.clear();
            isInited = true;
            for (int i = 0;i < Basic::getConstant("Scene", "MAX_LIGHT_NUM"); ++i)
            {
                availableSlots.insert(i);
                m_Shader->setUniform1i("u_Lights[" + std::to_string(i) + "].isOpen", 0);
            }
        }
        m_ID = *availableSlots.begin(); // 任意获取一个可用的下标
        std::cout << "Light ID: " << m_ID << std::endl;
        availableSlots.erase(m_ID); // 这个下标被占用
        m_Name = "Light";

        m_Shader->setUniform1i("u_Lights[" + std::to_string(m_ID) + "].isOpen", 1);
    }
    ~Light()
    { // 被删除的下标恢复
        std::cout << "Deleting light " << m_ID << std::endl;
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
            m_Shader->setUniform1i("u_Lights[" + std::to_string(m_ID) + "].isOpen", 1);
            m_Shader->setUniform3f("u_Lights[" + std::to_string(m_ID) + "].Position", m_Position);
            m_Shader->setUniform4f("u_Lights[" + std::to_string(m_ID) + "].Color", m_Color);
            m_Shader->setUniform1f("u_Lights[" + std::to_string(m_ID) + "].Brightness", m_Brightness);
            m_Shader->setUniform3f("u_Lights[" + std::to_string(m_ID) + "].Attenuation", m_Attenuation);
        }
    }
    inline void save(std::ostream& Out) const
    {
        Out << "Brightness: " << m_Brightness << std::endl;
        Out << "Name: " << m_Name << std::endl;
        Out << "Position: " << m_Position[0] << " " << m_Position[1] << " " << m_Position[2] << std::endl;
        Out << "Color: " << m_Color[0] << " " << m_Color[1] << " " << m_Color[2] << " " << m_Color[3] << std::endl;
        Out << "Attenuation: " << m_Attenuation[0] << " " << m_Attenuation[1] << " " << m_Attenuation[2] << std::endl;
    }
    inline static void load(std::ifstream& In, const std::shared_ptr<Shader>& shader, std::set<std::shared_ptr<Light>>& LightSet)
    {
        isInited = false;
        std::string Property;
        std::string Line;
        std::shared_ptr<Light> light;
        while (std::getline(In, Line))
        {
            if (Line.length() == 0)
            { // 跳过空行
                std::cout << "***" << std::endl;
                continue;
            }
            if (Line == "END_LIGHTING") break; // 读完所有的光
            std::stringstream ss;
            ss << Line;
            std::string Property = "";

            ss >> Property;
            if (Property == "Brightness:")
            {
                light = std::make_shared<Light>(shader);
                ss >> light->m_Brightness;
            }
            else if (Property == "Name:") ss >> light->m_Name;
            else if (Property == "Position:")
            {
                ss >> light->m_Position[0] >> light->m_Position[1] >> light->m_Position[2];
            }
            else if (Property == "Color:")
            {
                ss >> light->m_Color[0] >> light->m_Color[1] >> light->m_Color[2] >> light->m_Color[3];
            }
            else if (Property == "Attenuation:")
            {
                ss >> light->m_Attenuation[0] >> light->m_Attenuation[1] >> light->m_Attenuation[2];
                // 读完一个光源，更新并加入集合
                light->updateData();
                LightSet.insert(light);
                std::cout << "Light inserted!" << std::endl;
            }
        }
    }
public:
    float m_Brightness;
    std::shared_ptr<Shader> m_Shader;
    int m_ID;
    std::string m_Name;
    glm::vec3 m_Position;
    glm::vec4 m_Color;
    glm::vec3 m_Attenuation;
private:
    static std::set<int> availableSlots; // 可以用的u_Lights下标池
private:
    static bool isInited; // 是否初始化过
};
