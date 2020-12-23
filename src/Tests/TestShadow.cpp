//
// Created by 邱泽鸿 on 2020/12/14.
//

#include "TestShadow.h"
#include "Sphere.h"
#include "Cube.h"
#include "Cylinder.h"
#include <set>
#include <unordered_map>

void test::TestShadow::OnUpdate(GLFWwindow *Window, float deltaTime)
{
    m_Camera->OnKeyAction(Window, deltaTime);
}

void test::TestShadow::OnRender()
{
    DebugCall(glClearColor(0.1f, 0.3f, 0.5f, 0.6f));

    // 清除z-buffer，用于深度测试；以及清除背景颜色
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (m_Camera)
    { // 输入相机位置
        auto Position = m_Camera->getPosition();
        m_Shader->setUniform3f("u_CameraPosition", Position.x, Position.y, Position.z);
    }

    for (auto geometry : m_GeometrySet)
    {
        geometry->draw();
    }
    Floor->draw();
}

void test::TestShadow::OnImGuiRender()
{
    /* 添加集合物体 */
    if (ImGui::Button("add sphere"))
    {
        auto sphere = std::make_shared<Sphere>(m_Camera, m_Shader);
        m_GeometrySet.insert(sphere);
        // 保证创建物体总在相机前面
        sphere->m_Position = m_Camera->getPosition() + 10.0f * m_Camera->getDirection();
        selectedGeometry = sphere; // 总是保证新加的物体是先被选中的
    }
    if (ImGui::Button("add cube"))
    {
        auto cube = std::make_shared<Cube>(m_Camera, m_Shader);
        m_GeometrySet.insert(cube);
        // 保证创建物体总在相机前面
        cube->m_Position = m_Camera->getPosition() + 10.0f * m_Camera->getDirection();
        selectedGeometry = cube; // 总是保证新加的物体是先被选中的
    }
    /*
    if (ImGui::Button("add cone"))
    {
        auto cone = std::make_shared<Cone>(m_Camera, m_Shader);
        m_GeometrySet.insert(cone);
        // 保证创建物体总在相机前面
        cone->m_Position = m_Camera->getPosition() + 10.0f * m_Camera->getDirection();
        selectedGeometry = cone; // 总是保证新加的物体是先被选中的
    }*/
    if (ImGui::Button("add cylinder"))
    {
        auto cylinder = std::make_shared<Cylinder>(m_Camera, m_Shader);
        m_GeometrySet.insert(cylinder);
        // 保证创建物体总在相机前面
        cylinder->m_Position = m_Camera->getPosition() + 10.0f * m_Camera->getDirection();
        selectedGeometry = cylinder; // 总是保证新加的物体是先被选中的
    }
    if (ImGui::Button("add Prism"))
    {
        auto prism = std::make_shared<Cylinder>(m_Camera, m_Shader);
        m_GeometrySet.insert(prism);
        // 保证创建物体总在相机前面
        prism->m_Position = m_Camera->getPosition() + 10.0f * m_Camera->getDirection();
        selectedGeometry = prism; // 总是保证新加的物体是先被选中的

        selectedGeometry->Tag = "Prism"; //备注一下它是棱柱
        prism->updateSubdivision(5); // 细分度小点
    }

    if (ImGui::ColorEdit4("Light Color", &m_Light->m_Color[0]))
    {
        std::set<std::shared_ptr<Light>> temp;
        temp.insert(m_Light);
        m_Light->updateData();
        m_Shadow->updateShadowData();
    }
    if (ImGui::SliderFloat3("Light Position", &m_Light->m_Position.x, -10.0f, 10.0f))
    {
        std::set<std::shared_ptr<Light>> temp;
        temp.insert(m_Light);
        m_Light->updateData();
        m_Shadow->updateShadowData();
    }


    // 接下来的面板都和选中的几何物体有关
    if (!selectedGeometry)
    {
        return ;
    }

    /* 物体的集合 */
    std::unordered_map<int, std::shared_ptr<Geometry>> GeometryMap; // 整数到指针的映射表
    std::vector<std::string> items;
    int selectedItem = -1;
    /* 把现在的几何物体做成listbox，这部分是参考ImGui官方示例的 */
    int i = 0;
    for (auto geometry : m_GeometrySet)
    {
        if (geometry->Tag == "Prism")
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
        GeometryMap[i] = geometry;
        ++i;
    }

    if (ImGui::ListBox("Geometries", &selectedItem, items))
    {
        selectedGeometry = GeometryMap[selectedItem];
    }

    if (ImGui::SliderFloat3("Translation", &selectedGeometry->m_Position.x, -10.0f, 10.0f))
    {

    }

    if (ImGui::SliderFloat3("Rotation", &selectedGeometry->m_Rotation.Pitch, -180.0f, 180.0f))
    {
        selectedGeometry->updateRotation();
    }

    if ((selectedGeometry->getClassName() == "Geometry::Cylinder"))
    {
        auto cylinder = std::dynamic_pointer_cast<Cylinder>(selectedGeometry);
        m_Steps = cylinder->m_Steps;
        int minSteps = 20;
        int maxSteps = 1000;
        if (cylinder->Tag == "Prism")
        { // 棱柱面的细分度小点
            minSteps = 3;
            maxSteps = 10;
        }
        if (ImGui::SliderInt("Subdivision", &m_Steps, minSteps, maxSteps))
        {
            cylinder->updateSubdivision(m_Steps);
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

test::TestShadow::TestShadow()
{
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);
//    glDepthMask(GL_FALSE);

    m_Shader = std::make_shared<Shader>("../resource/TestGeometry.shader");
    m_Shader->bind();
    m_Camera = std::make_shared<Camera>();
    m_Camera->setProjection(glm::perspective(30.0f, 1.0f, 0.1f, 100.0f));
    selectedGeometry = nullptr;

    // 地板
    Floor = std::make_shared<Cube>(m_Camera, m_Shader);
    Floor->m_Scale = {20.0f, 20.0f, 0.0001f, 20.0f};
    Floor->m_Color = glm::vec4(0.18f, 0.6f, 0.96f, 1.0f);
    Floor->updateDrawData();
    Floor->m_Position = m_Camera->getPosition() + 10.0f * m_Camera->getDirection() - glm::vec3(0.0f, 2.0f, 0.0f);

    // 光源
    m_Light = std::make_shared<Light>(m_Shader);
    // 阴影
    m_Shadow = std::make_shared<Shadow>(m_Light);
}

void test::TestShadow::OnKeyAction(int key, int mods)
{
    if (key == GLFW_KEY_BACKSPACE)
    { // 删除当前选中物体
        std::cout << "deleting object" << std::endl;
        if (selectedGeometry)
        {
            m_GeometrySet.erase(selectedGeometry);
            if (!m_GeometrySet.empty())
            {
                selectedGeometry = *m_GeometrySet.begin(); // 选中随机一个物体
            }
            else selectedGeometry = nullptr;
        }
    }
}
