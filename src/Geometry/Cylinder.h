//
// Created by 邱泽鸿 on 2020/12/17.
//

#pragma once

#include "Geometry.h"
#include "Camera.h"


class Cylinder : public Geometry
{
public:
    Cylinder(const std::shared_ptr<Camera>& Camera, const std::shared_ptr<Shader>& Shader, const glm::vec3 &Position = glm::vec3(0.0f, 0.0f, 0.0f),
             const Material& material = {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec4(1.0f), 1.0f},
             const Rotation& rotation = {0.0f, 0.0f, 0.0f},
             const Scale& Scale = {1.0f, 1.0f, 1.0f, 1.0f});
    void updateDrawData() override;
    inline std::string getClassName() override
    {
        return "Geometry::Cylinder";
    }
    void updateSubdivision(int Steps);
public:
    int m_Steps; // 细分度
//    glm::vec4

private:
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec4 Color;
        glm::vec2 TexCoord;
    };
};