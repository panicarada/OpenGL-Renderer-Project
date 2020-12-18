//
// Created by 邱泽鸿 on 2020/12/14.
//

#include "TestSphere.h"

void test::TestSphere::OnUpdate(GLFWwindow *Window, float deltaTime)
{
    m_Camera->OnKeyAction(Window, deltaTime);
}

void test::TestSphere::OnRender()
{
    DebugCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    DebugCall(glClear(GL_COLOR_BUFFER_BIT));
    m_Sphere->draw();
}

void test::TestSphere::OnImGuiRender()
{
    if (ImGui::SliderFloat3("Translation", &m_Sphere->m_Position.x, -10.0f, 10.0f))
    {
//        m_Sphere->updateDrawData();
    }
    if (ImGui::SliderFloat3("Rotation", &m_Sphere->m_Rotation.Pitch, -180.0f, 180.0f))
    {
        m_Sphere->updateRotation();
//        m_Sphere->updateDrawData();
    }
    if (ImGui::SliderInt("Horizontal Subdivision", &m_HorizontalSteps, 20, 1000))
    { // 只有球体可以使用
        try
        {
            auto sphere = std::dynamic_pointer_cast<Sphere>(m_Sphere);
            sphere->updateSubdivision(m_VerticalSteps, m_HorizontalSteps);
        }
        catch (...)
        { // 不是球体点击这项不会有反应
            std::cout << "The type " << m_Sphere->getClassName()
            << " is not suppose the change the horizontal subdivision" << std::endl;
        }
    }
    if (ImGui::SliderInt("Vertical Subdivision", &m_VerticalSteps, 20, 1000))
    { // 只有球体可以使用
        try
        {
            auto sphere = std::dynamic_pointer_cast<Sphere>(m_Sphere);
            sphere->updateSubdivision(m_VerticalSteps, m_HorizontalSteps);
        }
        catch (...)
        { // 不是球体点击这项不会有反应
            std::cout << "The type " << m_Sphere->getClassName()
                      << " is not suppose the change the vertical subdivision" << std::endl;
        }
    }
    if (ImGui::ColorEdit4("Color", &m_Sphere->m_Color.x))
    {
        m_Sphere->updateDrawData();
    }
    if (ImGui::SliderFloat3("Scale", &m_Sphere->m_Scale.x, 0.0f, 10.0f))
    {
        m_Sphere->updateDrawData();
    }
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

test::TestSphere::TestSphere()
{
    // blend function参数设置为src = src_alpha, dest = 1 - src_alpha
    // 因为默认的blend function为ADD（即src + dest）
    // 所以这样写的效果是rgb = src_rgb * src_alpha + dest_rgb * (1-src_alpha)
//    DebugCall(glBlendFunc(GL_ONE, GL_ZERO));
//    DebugCall(glEnable(GL_BLEND));


    // 面剔除（不渲染背面）
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    m_Shader = std::make_shared<Shader>("../resource/TestSphere.shader");
    m_Shader->bind();
    m_Camera = std::make_shared<Camera>();
    m_Camera->setProjection(glm::perspective(30.0f, 1.0f, 0.1f, 100.0f));
    m_Sphere = std::make_shared<Sphere>(m_Camera, m_Shader);
    m_Sphere->m_Position = glm::vec3(0.0f, 0.0f, -2.0f);

    try { // 如果是球体，更新细分数
        auto sphere = std::dynamic_pointer_cast<Sphere>(m_Sphere);
        m_HorizontalSteps = sphere->getHorizontalSteps();
        m_VerticalSteps = sphere->getVerticalSteps();
    }
    catch (...)
    {  // 不是球体
        m_HorizontalSteps = m_VerticalSteps = 0;
    }
    m_Radius = 1.0;
    m_Color = glm::vec4(0.5f, 0.4f, 0.7f, 1.0f);
    m_Shader->setUniform4f("u_Color", m_Color[0], m_Color[1], m_Color[2],  m_Color[3]);
}
