//
// Created by 邱泽鸿 on 2020/12/14.
//

#pragma once
#include "Geometry.h"
#include "Renderer.h"

class Sphere : public Geometry
{
public:
    Sphere(const std::shared_ptr<Camera>& Camera, const std::shared_ptr<Shader>& Shader, const glm::vec3 &Position = glm::vec3(0.0f, 0.0f, 0.0f),
           const Material& material = {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec4(1.0f), 1.0f},
           const Rotation& rotation = {0.0f, 0.0f, 0.0f},
           const Scale& Scale = {1.0f, 1.0f, 1.0f, 1.0f});
    void updateSubdivision(int VerticalSteps, int HorizontalSteps);
    void updateDrawData() override;
    inline std::string getClassName() override
    {
        return "Geometry::Sphere";
    }
    inline int getVerticalSteps() const
    {
        return m_VerticalSteps;
    }
    inline int getHorizontalSteps() const
    {
        return m_HorizontalSteps;
    }
private:
    int m_VerticalSteps;
    int m_HorizontalSteps;
};
