//
// Created by 邱泽鸿 on 2020/12/14.
//

#pragma once

#include "Test.h"
#include "Geometry.h"
#include "Light.h"
#include <set>

namespace test
{

    class TestDepth : public Test
    {
    public:
        TestDepth();
        ~TestDepth() = default;

        void OnUpdate(GLFWwindow *Window, float deltaTime) override;
        void OnRender() override;
        void OnImGuiRender() override;
        void OnKeyAction(int key, int mods) override;
        inline std::shared_ptr<Camera> getCamera() override
        {
            return m_Camera;
        }
    private:
        std::shared_ptr<Geometry> Floor; // 地板
        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<Camera> m_Camera;
    };

}