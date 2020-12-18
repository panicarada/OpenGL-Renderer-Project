//
// Created by 邱泽鸿 on 2020/12/14.
//

#include "TestGeometry.h"
#include "Sphere.h"
#include "Cube.h"
#include "Cylinder.h"
#include "Cone.h"
#include <unordered_map>

namespace ImGui
{ // 重写ImGui一些Api，方便动态处理
    static auto vector_getter = [](void* vec, int idx, const char** out_text)
    {
        auto& vector = *static_cast<std::vector<std::string>*>(vec);
        if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
        *out_text = vector.at(idx).c_str();
        return true;
    };

    bool Combo(const char* label, int* currIndex, std::vector<std::string>& values)
    {
        if (values.empty()) { return false; }
        return Combo(label, currIndex, vector_getter,
                     static_cast<void*>(&values), values.size());
    }

    bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values)
    {
        if (values.empty()) { return false; }
        return ListBox(label, currIndex, vector_getter,
                       static_cast<void*>(&values), values.size());
    }
}

void test::TestGeometry::OnUpdate(GLFWwindow *Window, float deltaTime)
{
    m_Camera->OnKeyAction(Window, deltaTime);
}

void test::TestGeometry::OnRender()
{
    DebugCall(glClearColor(0.1f, 0.3f, 0.5f, 0.6f));

    // 清除z-buffer，用于深度测试；以及清除背景颜色
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto geometry : m_GeometryGroup)
    {

        geometry->draw();
    }
    Floor->draw();
}

void test::TestGeometry::OnImGuiRender()
{
    /* 添加集合物体 */
    if (ImGui::Button("add sphere"))
    {
        m_GeometryGroup.push_back(std::make_shared<Sphere>(m_Camera, m_Shader));
        // 保证创建物体总在相机前面
        m_GeometryGroup.back()->m_Position = m_Camera->getPosition() + 10.0f * m_Camera->getDirection();
        selectedGeometry = m_GeometryGroup.back(); // 总是保证新加的物体是先被选中的
    }
    if (ImGui::Button("add cube"))
    {
        m_GeometryGroup.push_back(std::make_shared<Cube>(m_Camera, m_Shader));
        // 保证创建物体总在相机前面
        m_GeometryGroup.back()->m_Position = m_Camera->getPosition() + 10.0f * m_Camera->getDirection();
        selectedGeometry = m_GeometryGroup.back(); // 总是保证新加的物体是先被选中的
    }
    if (ImGui::Button("add cone"))
    {
        m_GeometryGroup.push_back(std::make_shared<Cone>(m_Camera, m_Shader));
        // 保证创建物体总在相机前面
        m_GeometryGroup.back()->m_Position = m_Camera->getPosition() + 10.0f * m_Camera->getDirection();
        selectedGeometry = m_GeometryGroup.back(); // 总是保证新加的物体是先被选中的
    }
    if (ImGui::Button("add cylinder"))
    {
        m_GeometryGroup.push_back(std::make_shared<Cylinder>(m_Camera, m_Shader));
        // 保证创建物体总在相机前面
        m_GeometryGroup.back()->m_Position = m_Camera->getPosition() + 10.0f * m_Camera->getDirection();
        selectedGeometry = m_GeometryGroup.back(); // 总是保证新加的物体是先被选中的
    }
    if (ImGui::Button("add Prism"))
    {
        auto cylinder = std::make_shared<Cylinder>(m_Camera, m_Shader);
        m_GeometryGroup.push_back(cylinder);
        // 保证创建物体总在相机前面
        cylinder->m_Position = m_Camera->getPosition() + 10.0f * m_Camera->getDirection();
        selectedGeometry = m_GeometryGroup.back(); // 总是保证新加的物体是先被选中的

        selectedGeometry->Comment = "Prism"; //备注一下它是棱柱
        cylinder->updateSubdivision(5); // 细分度小点
    }
    if (!selectedGeometry)
    {
        return ;
    }

    if (ImGui::SliderFloat3("Translation", &selectedGeometry->m_Position.x, -10.0f, 10.0f))
    {
//        m_Sphere->updateDrawData();
    }

    if (ImGui::SliderFloat3("Rotation", &selectedGeometry->m_Rotation.Pitch, -180.0f, 180.0f))
    {
        selectedGeometry->updateRotation();
//        m_Sphere->updateDrawData();
    }

    if ((selectedGeometry->getClassName() == "Geometry::Cylinder"))
    {
        auto cylinder = std::dynamic_pointer_cast<Cylinder>(selectedGeometry);
        m_Steps = cylinder->m_Steps;
        int minSteps = 20;
        int maxSteps = 1000;
        if (cylinder->Comment == "Prism")
        { // 棱柱面的细分度小点
            minSteps = 3;
            maxSteps = 10;
        }
        if (ImGui::SliderInt("Subdivision", &m_Steps, minSteps, maxSteps))
        {
            cylinder->updateSubdivision(m_Steps);
        }
    }
    if ((selectedGeometry->getClassName() == "Geometry::Cone"))
    {
        auto cone = std::dynamic_pointer_cast<Cone>(selectedGeometry);
        m_Steps = cone->m_Steps;
        if (ImGui::SliderInt("Subdivision", &m_Steps, 20, 1000))
        {
            cone->updateSubdivision(m_Steps);
        }
    }

    if ((selectedGeometry->getClassName() == "Geometry::Sphere"))
    {
        auto sphere = std::dynamic_pointer_cast<Sphere>(selectedGeometry);
        m_HorizontalSteps = sphere->getHorizontalSteps();
        m_VerticalSteps = sphere->getVerticalSteps();
        if (ImGui::SliderInt("Horizontal Subdivision", &m_HorizontalSteps, 20, 1000))
        { // 只有球体可以使用
            sphere->updateSubdivision(m_VerticalSteps, m_HorizontalSteps);
        }
        if (ImGui::SliderInt("Vertical Subdivision", &m_VerticalSteps, 20, 1000))
        { // 只有球体可以使用
            sphere->updateSubdivision(m_VerticalSteps, m_HorizontalSteps);
        }
    }

//    std::unordered_map<std::string, std::shared_ptr<Geometry>> items; // 字符串到指针的映射表

    std::vector<std::string> items;
    int selectedItem = -1;
    /* 把现在的几何物体做成listbox，这部分是参考ImGui官方示例的 */
    for (int i = 0; i < m_GeometryGroup.size(); ++i)
    {
        auto geometry = m_GeometryGroup[i];
        if (geometry->Comment == "Prism")
        {
            items.push_back("Geometry::Prism" + std::to_string(i));
        }
        else
        {
            items.push_back(geometry->getClassName() + std::to_string(i));
        }
        if (selectedGeometry == geometry)
        {
            selectedItem = i;
        }
    }

    if (ImGui::ListBox("Geometries", &selectedItem, items))
    {
        selectedGeometry = m_GeometryGroup[selectedItem];
    }

    if (ImGui::ColorEdit4("Color", &selectedGeometry->m_Color.x))
    {
        selectedGeometry->updateDrawData();
    }
    if (selectedGeometry->getClassName() == "Geometry::Cylinder")
    {
        if (ImGui::SliderFloat4("Scale", &selectedGeometry->m_Scale.w, 0.0f, 10.0f))
        {
            selectedGeometry->updateDrawData();
        }
    }
    else
    {
        if (ImGui::SliderFloat3("Scale", &selectedGeometry->m_Scale.x, 0.0f, 10.0f))
        {
            selectedGeometry->updateDrawData();
        }
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

test::TestGeometry::TestGeometry()
{
    // blend function参数设置为src = src_alpha, dest = 1 - src_alpha
    // 因为默认的blend function为ADD（即src + dest）
    // 所以这样写的效果是rgb = src_rgb * src_alpha + dest_rgb * (1-src_alpha)
//    DebugCall(glBlendFunc(GL_ONE, GL_ZERO));
//    DebugCall(glEnable(GL_BLEND));

    // 面剔除（不渲染背面）
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);


    m_Shader = std::make_shared<Shader>("../resource/TestGeometry.shader");
    m_Shader->bind();
    selectedIndex = -1;
    m_Camera = std::make_shared<Camera>();
    m_Camera->setProjection(glm::perspective(30.0f, 1.0f, 0.1f, 100.0f));
    selectedGeometry = nullptr;


    // 地板
    Floor = std::make_shared<Cube>(m_Camera, m_Shader);
    Floor->m_Scale = {20.0f, 20.0f, 0.0001f, 20.0f};
    Floor->m_Color = glm::vec4(0.18f, 0.6f, 0.96f, 1.0f);
    Floor->updateDrawData();
    Floor->m_Position = m_Camera->getDirection() + 10.0f * m_Camera->getDirection() - glm::vec3(0.0f, 2.0f, 0.0f);
}
