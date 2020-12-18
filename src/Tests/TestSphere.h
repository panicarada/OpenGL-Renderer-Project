//
// Created by 邱泽鸿 on 2020/12/14.
//

#pragma once

#include "Test.h"
#include "Sphere.h"
#include "Geometry.h"

namespace test
{

    class TestSphere : public Test
    {
    public:
        TestSphere();
        ~TestSphere() = default;

        void OnUpdate(GLFWwindow *Window, float deltaTime) override;
        void OnRender() override;
        void OnImGuiRender() override;
        inline std::shared_ptr<Camera> getCamera() override
        {
            return m_Camera;
        }

    private:
        std::shared_ptr<Geometry> m_Sphere;
        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<Camera> m_Camera;
        glm::vec4 m_Color;
        glm::vec3 m_Position;

        float m_Radius;
        int m_HorizontalSteps;
        int m_VerticalSteps;
    };

}