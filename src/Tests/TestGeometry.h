//
// Created by 邱泽鸿 on 2020/12/14.
//

#pragma once

#include "Test.h"
#include "Geometry.h"
#include "Light.h"
#include "Shadow.h"
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
        void OnKeyAction(int key, int mods) override;
        inline std::shared_ptr<Camera> getCamera() override
        {
            return m_Camera;
        }
    private:
        std::set<std::shared_ptr<Geometry>> m_GeometrySet; // 几何物体的集合
        std::shared_ptr<Geometry> selectedGeometry; // 当前选中物体
        std::shared_ptr<Geometry> Floor; // 地板
        std::set<std::shared_ptr<Light>> m_LightSet; // 光源
        std::shared_ptr<Light> selectedLight;
        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<Camera> m_Camera;
        std::shared_ptr<TextureArray> m_TextureArray;
        std::shared_ptr<Shadow> m_Shadow;

        glm::vec4 m_Color;
        glm::vec3 m_Position;
        int m_Steps;
        int m_HorizontalSteps;
        int m_VerticalSteps;

        std::string m_TextureName; // 用户输入的纹理名称，只有当按下回车后才会被应用
    };

}