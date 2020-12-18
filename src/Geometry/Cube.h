//
// Created by 邱泽鸿 on 2020/12/17.
//

#pragma once

#include "Geometry.h"
#include "Camera.h"

class Cube : public Geometry
{
public:
    Cube(const std::shared_ptr<Camera>& Camera, const std::shared_ptr<Shader>& Shader, const glm::vec3 &Position = glm::vec3(0.0f, 0.0f, 0.0f),
           Rotation rotation = {0.0f, 0.0f, 0.0f}, Scale Scale = {1.0f, 1.0f, 1.0f, 1.0f});
    void updateDrawData() override;
    inline std::string getClassName() override
    {
        return "Geometry::Cube";
    }
private:
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec4 Color;
    };
};