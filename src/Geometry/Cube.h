//
// Created by 邱泽鸿 on 2020/12/17.
//

#pragma once

#include "Geometry.h"
#include "Camera.h"

#include <vector>
#include <random>

class Cube : public Geometry
{
public:
    Cube(const std::shared_ptr<Camera>& Camera, const std::shared_ptr<Shader>& Shader, const glm::vec3 &Position = glm::vec3(0.0f, 0.0f, 0.0f),
         const Material& material = {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec4(1.0f), 1.0f},
         const Rotation& rotation = {0.0f, 0.0f, 0.0f},
         const Scale& Scale = {1.0f, 1.0f, 1.0f, 1.0f});
    void updateDrawData() override {}
    inline std::string getClassName() override
    {
        return "Geometry::Cube";
    }
private:
    void init();
    inline std::vector<Vertex> getSquare(const glm::vec3 Normal)
    {
        // 随机数
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-0.1f, 0.2f);//uniform distribution between 0 and 1

        // 注意，都要逆时针方向
        std::vector<Vertex> Vertices;
        if (Normal.x > 0)
        {
            Vertices.push_back({glm::vec3(1.0f, -1.0f, 1.0f), Normal,
                                m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f),
                                glm::vec2(0.0f, 0.0f)});
            Vertices.push_back({glm::vec3(1.0f, -1.0f, -1.0f), Normal,
                                m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f),
                                glm::vec2(1.0f, 0.0f)});
            Vertices.push_back({glm::vec3(1.0f, 1.0f, -1.0f), Normal,
                                m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f),
                                glm::vec2(1.0f, 1.0f)});
            Vertices.push_back({glm::vec3(1.0f, 1.0f, 1.0f), Normal,
                                m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f),
                                glm::vec2(0.0f, 1.0f)});
        }
        else if (Normal.x < 0)
        {
            Vertices.push_back({glm::vec3(-1.0f, -1.0f, 1.0f), Normal,
                                m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f),
                                glm::vec2(0.0f, 0.0f)});
            Vertices.push_back({glm::vec3(-1.0f, 1.0f, 1.0f), Normal,
                                m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f),
                                glm::vec2(1.0f, 0.0f)});
            Vertices.push_back({glm::vec3(-1.0f, 1.0f, -1.0f), Normal,
                                m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f),
                                glm::vec2(1.0f, 1.0f)});
            Vertices.push_back({glm::vec3(-1.0f, -1.0f, -1.0f), Normal,
                                m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f),
                                glm::vec2(0.0f, 1.0f)});
        }
        else if (Normal.y > 0)
        {
            Vertices.push_back({glm::vec3(-1.0f, 1.0f, 1.0f), Normal,
                                m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f),
                                glm::vec2(0.0f, 0.0f)});
            Vertices.push_back({glm::vec3(1.0f, 1.0f, 1.0f), Normal,
                                m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f),
                                glm::vec2(1.0f, 0.0f)});
            Vertices.push_back({glm::vec3(1.0f, 1.0f, -1.0f), Normal,
                                m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f),
                                glm::vec2(1.0f, 1.0f)});
            Vertices.push_back({glm::vec3(-1.0f, 1.0f, -1.0f), Normal,
                                m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f),
                                glm::vec2(0.0f, 1.0f)});
        }
        else if (Normal.y < 0)
        {
            Vertices.push_back({glm::vec3(-1.0f, -1.0f, 1.0f), Normal,
                                m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f),
                                glm::vec2(0.0f, 0.0f)});
            Vertices.push_back({glm::vec3(-1.0f, -1.0f, -1.0f), Normal,
                                m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f),
                                glm::vec2(1.0f, 0.0f)});
            Vertices.push_back({glm::vec3(1.0f, -1.0f, -1.0f), Normal,
                                m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f),
                                glm::vec2(1.0f, 1.0f)});
            Vertices.push_back({glm::vec3(1.0f, -1.0f, 1.0f), Normal,
                                m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f),
                                glm::vec2(0.0f, 1.0f)});
        }
        else if (Normal.z > 0)
        {
            Vertices.push_back({glm::vec3(-1.0f, -1.0f, 1.0f), Normal,
                                m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f),
                                glm::vec2(0.0f, 0.0f)});
            Vertices.push_back({glm::vec3(1.0f, -1.0f, 1.0f), Normal,
                                m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f),
                                glm::vec2(1.0f, 0.0f)});
            Vertices.push_back({glm::vec3(1.0f, 1.0f, 1.0f), Normal,
                                m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f),
                                glm::vec2(1.0f, 1.0f)});
            Vertices.push_back({glm::vec3(-1.0f, 1.0f, 1.0f), Normal,
                                m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f),
                                glm::vec2(0.0f, 1.0f)});
        }
        else if (Normal.z < 0)
        {
            Vertices.push_back({glm::vec3(-1.0f, -1.0f, -1.0f), Normal,
                                m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f),
                                glm::vec2(0.0f, 0.0f)});
            Vertices.push_back({glm::vec3(-1.0f, 1.0f, -1.0f), Normal,
                                m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f),
                                glm::vec2(1.0f, 0.0f)});
            Vertices.push_back({glm::vec3(1.0f, 1.0f, -1.0f), Normal,
                                m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f),
                                glm::vec2(1.0f, 1.0f)});
            Vertices.push_back({glm::vec3(1.0f, -1.0f, -1.0f), Normal,
                                m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f),
                                glm::vec2(0.0f, 1.0f)});
        }
        return Vertices;
    }
};