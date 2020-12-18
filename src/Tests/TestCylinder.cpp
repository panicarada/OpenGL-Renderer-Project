//
// Created by 邱泽鸿 on 2020/12/14.
//

#include "TestCylinder.h"

void test::TestCylinder::OnUpdate(GLFWwindow *Window, float deltaTime)
{
    m_Camera->OnKeyAction(Window, deltaTime);
}

void test::TestCylinder::OnRender()
{
    DebugCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    DebugCall(glClear(GL_COLOR_BUFFER_BIT));
    m_Cylinder->draw();
}

void test::TestCylinder::OnImGuiRender()
{
    if (ImGui::SliderFloat3("Translation", &m_Cylinder->m_Position.x, -10.0f, 10.0f))
    {
//        m_Sphere->updateDrawData();
    }
    if (ImGui::SliderFloat3("Rotation", &m_Cylinder->m_Rotation.Pitch, -180.0f, 180.0f))
    {
        m_Cylinder->updateRotation();
//        m_Sphere->updateDrawData();
    }
    if (ImGui::SliderInt("Subdivision", &m_Steps, 20, 1000))
    { // 只有球体可以使用
        try
        {
            auto cylinder = std::dynamic_pointer_cast<Cylinder>(m_Cylinder);
            cylinder->updateSubdivision(m_Steps);
        }
        catch (...)
        { // 不是球体点击这项不会有反应
            std::cout << "The type " << m_Cylinder->getClassName()
                      << " is not suppose the change the vertical subdivision" << std::endl;
        }
    }
    if (ImGui::ColorEdit4("Color", &m_Cylinder->m_Color.x))
    {
        m_Cylinder->updateDrawData();
    }
    if (ImGui::SliderFloat3("Scale", &m_Cylinder->m_Scale.x, 0.0f, 10.0f))
    {
        m_Cylinder->updateDrawData();
    }
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

test::TestCylinder::TestCylinder()
{
    // blend function参数设置为src = src_alpha, dest = 1 - src_alpha
    // 因为默认的blend function为ADD（即src + dest）
    // 所以这样写的效果是rgb = src_rgb * src_alpha + dest_rgb * (1-src_alpha)
//    DebugCall(glBlendFunc(GL_ONE, GL_ZERO));
//    DebugCall(glEnable(GL_BLEND));


    // 面剔除（不渲染背面）
//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_BACK);
//    glFrontFace(GL_CW);


    m_Shader = std::make_shared<Shader>("../resource/TestSphere.shader");
    m_Shader->bind();
    m_Camera = std::make_shared<Camera>();
    m_Camera->setProjection(glm::perspective(30.0f, 1.0f, 0.1f, 100.0f));
    m_Cylinder = std::make_shared<Cylinder>(m_Camera, m_Shader);
    m_Cylinder->m_Position = glm::vec3(0.0f, 0.0f, -2.0f);

    try { // 如果是球体，更新细分数
        auto cylinder = std::dynamic_pointer_cast<Cylinder>(m_Cylinder);
        m_Steps = cylinder->m_Steps;
    }
    catch (...)
    {  // 不是球体
        m_Steps = 0;
    }
    m_Color = glm::vec4(0.5f, 0.4f, 0.7f, 1.0f);
    m_Shader->setUniform4f("u_Color", m_Color[0], m_Color[1], m_Color[2],  m_Color[3]);
}
