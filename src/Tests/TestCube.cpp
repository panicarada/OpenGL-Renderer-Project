//
// Created by 邱泽鸿 on 2020/12/14.
//

#include "TestCube.h"

void test::TestCube::OnUpdate(GLFWwindow *Window, float deltaTime)
{
    m_Camera->OnKeyAction(Window, deltaTime);
}

void test::TestCube::OnRender()
{
    DebugCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    DebugCall(glClear(GL_COLOR_BUFFER_BIT));
    m_Cube->draw();
}

void test::TestCube::OnImGuiRender()
{
    if (ImGui::SliderFloat3("Translation", &m_Cube->m_Position.x, -10.0f, 10.0f))
    {
//        m_Cube->updateDrawData();
    }
    if (ImGui::SliderFloat3("Rotation", &m_Cube->m_Rotation.Pitch, -180.0f, 180.0f))
    {
        m_Cube->updateRotation();
//        m_Sphere->updateDrawData();
    }
    if (ImGui::ColorEdit4("Color", &m_Cube->m_Color.x))
    {
        m_Cube->updateDrawData();
    }
    if (ImGui::SliderFloat3("Scale", &m_Cube->m_Scale.x, 0.0f, 10.0f))
    {
        m_Cube->updateDrawData();
    }
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

test::TestCube::TestCube()
{
    // 面剔除（不渲染背面）
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
//    glFrontFace(GL_CW);

    m_Shader = std::make_shared<Shader>("../resource/TestCube.shader");
    m_Shader->bind();
    m_Camera = std::make_shared<Camera>();
    m_Camera->setProjection(glm::perspective(30.0f, 1.0f, 0.1f, 100.0f));
    m_Cube = std::make_shared<Cube>(m_Camera, m_Shader);
    m_Cube->m_Position = glm::vec3(0.0f, 0.0f, -2.0f);

    m_Color = glm::vec4(0.5f, 0.4f, 0.7f, 1.0f);
    m_Shader->setUniform4f("u_Color", m_Color[0], m_Color[1], m_Color[2],  m_Color[3]);
}
