//
// Created by 邱泽鸿 on 2020/12/14.
//

#include "TestDepth.h"
#include "Sphere.h"
#include "Cube.h"
#include "Cylinder.h"
#include "Cone.h"
#include <unordered_map>

void test::TestDepth::OnUpdate(GLFWwindow *Window, float deltaTime)
{
    m_Camera->OnKeyAction(Window, deltaTime);
}

void test::TestDepth::OnRender()
{
    DebugCall(glClearColor(0.1f, 0.3f, 0.5f, 0.6f));

    // 清除z-buffer，用于深度测试；以及清除背景颜色
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (m_Camera)
    { // 输入相机位置
        auto Position = m_Camera->getPosition();
        m_Shader->setUniform3f("u_CameraPosition", Position.x, Position.y, Position.z);
    }

    Floor->draw();
}

void test::TestDepth::OnImGuiRender()
{

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

test::TestDepth::TestDepth()
{
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    m_Shader = std::make_shared<Shader>("../resource/TestDepth.shader");
    m_Shader->bind();
    m_Camera = std::make_shared<Camera>();
    m_Camera->setProjection(glm::perspective(30.0f, 1.0f, 0.1f, 100.0f));

    // 地板
    Floor = std::make_shared<Cube>(m_Camera, m_Shader);
    Floor->m_Scale = {20.0f, 20.0f, 0.0001f, 20.0f};
    Floor->m_Color = glm::vec4(0.18f, 0.6f, 0.96f, 1.0f);
    Floor->updateDrawData();
    Floor->m_Position = m_Camera->getPosition() + 10.0f * m_Camera->getDirection() - glm::vec3(0.0f, 2.0f, 0.0f);
}

void test::TestDepth::OnKeyAction(int key, int mods)
{

}
