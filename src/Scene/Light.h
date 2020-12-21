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
        : m_Shader(shader), m_ID(ID), m_Position(Position), m_Color(Color), m_Intensity(Intensity), Brightness(1.0f)
    {
        m_Name = "Light";
    }
    static void updateData(std::shared_ptr<Shader> shader, const std::set<std::shared_ptr<Light>> LightSet)
    {
        int i = 0;
        std::vector<glm::vec3> LightPositions;
        std::vector<glm::vec4> LightColors;
        std::vector<float> LightBrightnesses;

        for (auto light : LightSet)
        {
            LightPositions.push_back(light->m_Position);
            LightColors.push_back(light->m_Color);
            LightBrightnesses.push_back(light->Brightness);
        }

        // 设置和光源有关的uniform
        shader->setUniform1i("u_LightNum", LightSet.size());
        shader->setUniform3fv("u_LightPositions", LightPositions);
        shader->setUniform4fv("u_LightColor", LightColors);
        shader->setUniform1fv("u_LightBrightness", LightBrightnesses);
    }
public:
    float Brightness;
    std::shared_ptr<Shader> m_Shader;
    int m_ID;
    std::string m_Name;
    glm::vec3 m_Position;
    glm::vec4 m_Color;
    float m_Intensity;
};
