//
// Created by 邱泽鸿 on 2020/12/14.
//

#pragma once

#include "Test.h"
#include "Cylinder.h"
#include "Geometry.h"

namespace test
{

    class TestCylinder : public Test
    {
    public:
        TestCylinder();
        ~TestCylinder() = default;

        void OnUpdate(GLFWwindow *Window, float deltaTime) override;
        void OnRender() override;
        void OnImGuiRender() override;
        inline std::shared_ptr<Camera> getCamera() override
        {
            return m_Camera;
        }
    private:
        std::shared_ptr<Geometry> m_Cylinder;
        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<Camera> m_Camera;
        glm::vec4 m_Color;
        glm::vec3 m_Position;
        int m_Steps;
    };

}