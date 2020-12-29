//
// Created by 邱泽鸿 on 2020/12/14.
//

#pragma once

#include "Test.h"
#include "Geometry.h"
#include "Light.h"
#include "Shadow.h"
#include "SoftShadow.h"
#include <set>
#include <PoissonMap.h>

namespace test
{

    class TestShadowVSM : public Test
    {
    public:
        TestShadowVSM();
        ~TestShadowVSM() = default;

        void OnUpdate(GLFWwindow *Window, float deltaTime) override;
        void OnRender() override;
        void OnImGuiRender() override;
        inline std::shared_ptr<Camera> getCamera() override
        {
            return m_Camera;
        }
    private:
        std::set<std::shared_ptr<Geometry>> m_GeometrySet; // 几何物体的集合
        std::shared_ptr<Geometry> Floor; // 地板
        std::set<std::shared_ptr<Light>> m_LightSet; // 光源
        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<Camera> m_Camera;
        std::shared_ptr<Light> selectedLight;

        std::shared_ptr<SoftShadow> m_SoftShadow;
    };

}