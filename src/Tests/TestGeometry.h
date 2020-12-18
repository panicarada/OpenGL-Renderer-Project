//
// Created by 邱泽鸿 on 2020/12/14.
//

#pragma once

#include "Test.h"
#include "Geometry.h"

namespace test
{

    class TestGeometry : public Test
    {
    public:
        TestGeometry();
        ~TestGeometry() = default;

        void OnUpdate(GLFWwindow *Window, float deltaTime) override;
        void OnRender() override;
        void OnImGuiRender() override;
        inline std::shared_ptr<Camera> getCamera() override
        {
            return m_Camera;
        }
    private:
        // 几何物体的集合
        std::vector<std::shared_ptr<Geometry>> m_GeometryGroup;
        std::shared_ptr<Geometry> selectedGeometry;
        std::shared_ptr<Geometry> Floor;
        int selectedIndex;

        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<Camera> m_Camera;
        glm::vec4 m_Color;
        glm::vec3 m_Position;
        int m_Steps;
        int m_HorizontalSteps;
        int m_VerticalSteps;
    };

}