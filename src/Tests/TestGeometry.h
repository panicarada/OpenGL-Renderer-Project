//
// Created by 邱泽鸿 on 2020/12/14.
//

#pragma once

#include "Test.h"
#include "Geometry.h"
#include <set>

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
        void OnKeyAction(int key) override;
        inline std::shared_ptr<Camera> getCamera() override
        {
            return m_Camera;
        }
    private:
        std::set<std::shared_ptr<Geometry>> m_GeometrySet; // 几何物体的集合
        std::shared_ptr<Geometry> selectedGeometry; // 当前选中物体
        std::shared_ptr<Geometry> Floor; // 地板

        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<Camera> m_Camera;
        glm::vec4 m_Color;
        glm::vec3 m_Position;
        int m_Steps;
        int m_HorizontalSteps;
        int m_VerticalSteps;
    };

}