//
// Created by 邱泽鸿 on 2020/12/14.
//

#include "TestGeometry.h"
#include "Sphere.h"
#include "Cube.h"
#include "Cylinder.h"
#include "Cone.h"
#include <unordered_map>

void test::TestGeometry::OnUpdate(GLFWwindow *Window, float deltaTime)
{
    m_Camera->OnKeyAction(Window, deltaTime);
}

void test::TestGeometry::OnRender()
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

void test::TestGeometry::OnImGuiRender()
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
    if (ImGui::Button("add cone"))
    {
        auto cone = std::make_shared<Cone>(m_Camera, m_Shader);
        m_GeometrySet.insert(cone);
        // 保证创建物体总在相机前面
        cone->m_Position = m_Camera->getPosition() + 10.0f * m_Camera->getDirection();
        selectedGeometry = cone; // 总是保证新加的物体是先被选中的
    }
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
    if (ImGui::Button("add light"))
    {
        auto light = std::make_shared<Light>(m_Shader);
        m_LightSet.insert(light);
        // 保证创建光源总在相机后面
        light->m_Position = m_Camera->getPosition() - 2.0f * m_Camera->getDirection();
        selectedLight = light; // 总是保证新加的光源是先被选中的

        Light::updateData(m_Shader, m_LightSet);
    }

    /* 光源的集合 */
    std::unordered_map<int, std::shared_ptr<Light>> LightMap; // 整数到指针的映射表
    std::vector<std::string> items;
    int selectedItem = -1; // 选中光源在列表中的位置
    /* 把现在的光源做成listbox，这部分是参考ImGui官方示例的 */
    int i = 0;
    for (auto light : m_LightSet)
    {
        light->m_ID = i;
        items.push_back(light->m_Name + std::to_string(i));
        if (selectedLight == light)
        {
            selectedItem = i;
        }
        LightMap[i] = light;
        ++i;
    }

    if (ImGui::ListBox("Lights", &selectedItem, items))
    {
        selectedLight = LightMap[selectedItem];
    }

    if (selectedLight)
    {
        if (ImGui::ColorEdit4("Light Color", &selectedLight->m_Color[0]))
        {
            Light::updateData(m_Shader, m_LightSet);
        }
        if (ImGui::SliderFloat3("Light Position", &selectedLight->m_Position.x, -10.0f, 10.0f))
        {
            Light::updateData(m_Shader, m_LightSet);
        }
        if (ImGui::SliderFloat("Light Brightness", &selectedLight->Brightness, 0.5f, 5.0f))
        {
            Light::updateData(m_Shader, m_LightSet);
        }
    }


    // 接下来的面板都和选中的几何物体有关
    if (!selectedGeometry)
    {
        return ;
    }

    /* 物体的集合 */
    std::unordered_map<int, std::shared_ptr<Geometry>> GeometryMap; // 整数到指针的映射表
    items.clear();
    /* 把现在的几何物体做成listbox，这部分是参考ImGui官方示例的 */
    i = 0;
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
        int maxSteps = 300;
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
    if ((selectedGeometry->getClassName() == "Geometry::Cone"))
    {
        auto cone = std::dynamic_pointer_cast<Cone>(selectedGeometry);
        m_Steps = cone->m_Steps;
        if (ImGui::SliderInt("Subdivision", &m_Steps, 20, 300))
        {
            cone->updateSubdivision(m_Steps);
        }
    }

    if ((selectedGeometry->getClassName() == "Geometry::Sphere"))
    {
        auto sphere = std::dynamic_pointer_cast<Sphere>(selectedGeometry);
        m_HorizontalSteps = sphere->getHorizontalSteps();
        m_VerticalSteps = sphere->getVerticalSteps();
        if (ImGui::SliderInt("Horizontal Subdivision", &m_HorizontalSteps, 20, 300))
        { // 只有球体可以使用
            sphere->updateSubdivision(m_VerticalSteps, m_HorizontalSteps);
        }
        if (ImGui::SliderInt("Vertical Subdivision", &m_VerticalSteps, 20, 300))
        { // 只有球体可以使用
            sphere->updateSubdivision(m_VerticalSteps, m_HorizontalSteps);
        }
    }

    // 材质和颜色
    if (ImGui::ColorEdit4("Color", &selectedGeometry->m_Color.x))
    {
        selectedGeometry->updateDrawData();
    }
    if (ImGui::ColorEdit4("Material Ambient", &selectedGeometry->m_Material.Ambient[0]))
    {
//        selectedGeometry->updateDrawData();
    }
    if (ImGui::ColorEdit4("Material Diffuse", &selectedGeometry->m_Material.Diffuse[0]))
    {
//        selectedGeometry->updateDrawData();
    }
    if (ImGui::ColorEdit4("Material Specular", &selectedGeometry->m_Material.Specular[0]))
    {
//        selectedGeometry->updateDrawData();
    }
    if (ImGui::SliderFloat("Material Highlight", &selectedGeometry->m_Material.Highlight, 1.0f, 180.0f))
    {
//        selectedGeometry->updateRotation();
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

    if (ImGui::InputText("Texture Selection", &m_TextureName, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        std::cout << "Hello" << std::endl;
    }
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

test::TestGeometry::TestGeometry()
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
//    auto LightPosition = m_Camera->getPosition() - 10.0f * m_Camera->getDirection() + glm::vec3(-2.0f, 5.0f, 0.0f);
//    m_Shader->setUniform3f("u_LightPosition", LightPosition.x, LightPosition.y, LightPosition.z);
//    m_Shader->setUniform4f("u_LightColor", 1.0f, 1.0f, 1.0f, 1.0f);
    m_Shader->setUniform4f("u_Ambient", 0.2f, 0.2, 0.2f, 1.0f);
}

void test::TestGeometry::OnKeyAction(int key, int mods)
{
    if (key == GLFW_KEY_BACKSPACE)
    { // 删除当前选中物体/光源
        if (mods == GLFW_MOD_SHIFT)
        { // 组合键删除光源
            std::cout << "deleting light" << std::endl;
            if (selectedLight)
            {
                m_LightSet.erase(selectedLight);
                if (!m_LightSet.empty())
                {
                    selectedLight = *m_LightSet.begin();
                }
                else selectedLight = nullptr;
                Light::updateData(m_Shader, m_LightSet);
            }
        }
        else
        {
            std::cout << "deleting object" << std::endl;
//            if (selectedGeometry)
//            {
//                m_GeometrySet.erase(selectedGeometry);
//                if (!m_GeometrySet.empty())
//                {
//                    selectedGeometry = *m_GeometrySet.begin(); // 选中随机一个物体
//                }
//                else selectedGeometry = nullptr;
//            }
        }
    }
    else if (key == GLFW_KEY_ENTER)
    {
        if (selectedGeometry)
        { // 尝试更换纹理
            // 检查文件是否存在
            if (std::ifstream("../resource/Textures/" + m_TextureName + ".png"))
            {
                selectedGeometry->m_Texture = std::make_shared<Texture>(m_Shader,  "../resource/Textures/" + m_TextureName + ".png");
            }
            else
            {
                std::cout << "The file: '" << m_TextureName << "' is not available!" << std::endl;
            }


        }

    }
}
