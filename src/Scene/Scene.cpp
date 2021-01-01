//
// Created by 邱泽鸿 on 2020/12/14.
//

#include "Scene.h"
#include "Sphere.h"
#include "Cube.h"
#include "Cylinder.h"
#include <unordered_map>

void test::Scene::OnUpdate(GLFWwindow *Window, float deltaTime)
{
    m_Camera->OnKeyAction(Window, deltaTime);
}

void test::Scene::OnRender()
{
    DebugCall(glClearColor(0.1f, 0.3f, 0.5f, 0.6f));

//    m_Shadow->setSamples(m_Shader);
    if (updateShadow)
    {
        m_Shadow->setSamples(m_Shader);
        m_Shadow->render(m_GeometrySet, m_LightSet);
    }

    // 清除z-buffer，用于深度测试；以及清除背景颜色
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 启用深度图
    glActiveTexture(GL_TEXTURE0 + m_TextureArray->getImageNum());
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_Shadow->getDepthMap());

    // 启用材质
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_TextureArray->getRendererID());

    m_Shader->bind();
    m_Shader->setUniform1f("u_zNear", ZNEAR);
    m_Shader->setUniform1f("u_zFar", ZFAR);

    if (m_Camera)
    { // 输入相机位置
        auto Position = m_Camera->getPosition();
        m_Shader->setUniform3f("u_CameraPosition", Position.x, Position.y, Position.z);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto geometry : m_GeometrySet)
    {
        geometry->draw();
    }
    Floor->draw();
}

void test::Scene::OnImGuiRender()
{
    updateShadow = false; // 是否需要更新阴影

    /* 添加几何物体 */
    if (ImGui::Button("add sphere"))
    {
        auto sphere = std::make_shared<Sphere>(m_Camera, m_Shader);
        m_GeometrySet.insert(sphere);
        // 保证创建物体总在相机前面
        sphere->m_Position = m_Camera->getPosition() + 10.0f * m_Camera->getDirection();
        selectedGeometry = sphere; // 总是保证新加的物体是先被选中的

        // 更新阴影
        updateShadow = true;
    }
    if (ImGui::Button("add cube"))
    {
        auto cube = std::make_shared<Cube>(m_Camera, m_Shader);
        m_GeometrySet.insert(cube);
        // 保证创建物体总在相机前面
        cube->m_Position = m_Camera->getPosition() + 10.0f * m_Camera->getDirection();
        selectedGeometry = cube; // 总是保证新加的物体是先被选中的

        // 更新阴影
        updateShadow = true;
    }
    if (ImGui::Button("add cone"))
    {
        auto cone = std::make_shared<Cylinder>(m_Camera, m_Shader);
        m_GeometrySet.insert(cone);
        // 保证创建物体总在相机前面
        cone->m_Position = m_Camera->getPosition() + 10.0f * m_Camera->getDirection();
        selectedGeometry = cone; // 总是保证新加的物体是先被选中的

        selectedGeometry->Tag = "Cone"; // 备注一下它是圆锥
        selectedGeometry->m_Scale.w = 0.0f; // 上端半径为0
        selectedGeometry->updateDrawData();

        // 更新阴影
        updateShadow = true;
    }
    if (ImGui::Button("add cylinder"))
    {
        auto cylinder = std::make_shared<Cylinder>(m_Camera, m_Shader);
        m_GeometrySet.insert(cylinder);
        // 保证创建物体总在相机前面
        cylinder->m_Position = m_Camera->getPosition() + 10.0f * m_Camera->getDirection();
        selectedGeometry = cylinder; // 总是保证新加的物体是先被选中的

        // 更新阴影
        updateShadow = true;
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

        // 更新阴影
        updateShadow = true;
    }
    if (ImGui::Button("add light"))
    {
        auto light = std::make_shared<Light>(m_Shader);
        m_LightSet.insert(light);
        // 保证创建光源总在相机后面
        light->m_Position = m_Camera->getPosition() - 2.0f * m_Camera->getDirection();
        selectedLight = light; // 总是保证新加的光源是先被选中的
        selectedLight->updateData();

        // 更新阴影
        updateShadow = true;
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
            selectedLight->updateData();
        }
        if (ImGui::SliderFloat3("Light Position", &selectedLight->m_Position.x, -10.0f, 10.0f))
        {
            selectedLight->updateData();
            // 更新阴影
            updateShadow = true;
        }
        if (ImGui::SliderFloat("Light Brightness", &selectedLight->m_Brightness, 0.5f, 5.0f))
        {
            selectedLight->updateData();
        }
        if (ImGui::SliderFloat3("Light Attenuation", &selectedLight->m_Attenuation[0], 0.0f, 2.0f))
        {
            selectedLight->updateData();
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
        // 更新阴影
        updateShadow = true;
    }

    if (ImGui::SliderFloat3("Rotation", &selectedGeometry->m_Rotation.Pitch, -180.0f, 180.0f))
    {
        selectedGeometry->updateRotation();
        // 更新阴影
        updateShadow = true;
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

            // 更新阴影
            updateShadow = true;
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

            // 更新阴影
            updateShadow = true;
        }
        if (ImGui::SliderInt("Vertical Subdivision", &m_VerticalSteps, 20, 300))
        { // 只有球体可以使用
            sphere->updateSubdivision(m_VerticalSteps, m_HorizontalSteps);
            // 更新阴影
            updateShadow = true;
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
            // 更新阴影
            updateShadow = true;
        }
    }
    else
    {
        if (ImGui::SliderFloat3("Scale", &selectedGeometry->m_Scale.x, 0.05f, 10.0f))
        {
            selectedGeometry->updateDrawData();
            // 更新阴影
            updateShadow = true;
        }
    }

    if (ImGui::InputText("Texture Selection", &m_TextureName, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        std::cout << "Hello" << std::endl;
    }
    if (ImGui::Button("Load Texture"))
    {
        if (selectedGeometry)
        { // 尝试更换纹理
            if (m_TextureName == ".detach")
            { // 删除材质
                selectedGeometry->detachTexture(m_TextureArray);
            }
            else if (std::ifstream("../resource/Textures/" + m_TextureName))
            { // 检查文件是否存在
                if (selectedGeometry->m_TextureSlot > 0)
                {
                    m_TextureArray->eraseTexture(selectedGeometry->m_TextureSlot);
                }
                std::cout << (selectedGeometry->m_TextureSlot = m_TextureArray->addTexture( "../resource/Textures/" + m_TextureName)) << std::endl;
            }
            else
            {
                std::cout << "The file: '" << m_TextureName << "' is not available!" << std::endl;
            }
        }
    }

    if (ImGui::SliderInt("Shadow Sample Number", &u_SampleNum,0, Basic::getConstant(m_ShaderFileName, "MAX_SAMPLE_NUM")))
    { // 更新阴影采样点数目
        m_Shader->setUniform1i("u_SampleNum", u_SampleNum);
    }
    if (ImGui::SliderFloat("Shadow Sample Area", &u_SampleArea, 0.0f, 2.0f))
    {
        m_Shader->setUniform1f("u_SampleArea", u_SampleArea / 100.0f);
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    if (updateShadow)
    {

    }
}

test::Scene::Scene()
{

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    m_ShaderFileName = "../resource/Scene/Scene.shader";
    m_Shader = std::make_shared<Shader>(m_ShaderFileName);
    m_Shader->bind();
    m_Camera = std::make_shared<Camera>();

    selectedGeometry = nullptr;

    // 地板
    Floor = std::make_shared<Cube>(m_Camera, m_Shader);
    Floor->m_Scale = {20.0f, 20.0f, 0.0001f, 20.0f};
    Floor->m_Color = glm::vec4(0.18f, 0.6f, 0.96f, 1.0f);
    Floor->updateDrawData();
    Floor->m_Position = m_Camera->getPosition() + 10.0f * m_Camera->getDirection() - glm::vec3(0.0f, 2.0f, 0.0f);

    // Obj
    auto m_ObjLoader = std::make_shared<ObjLoader>(m_Camera, m_Shader);
    m_ObjLoader->loadOBJ("../resource/Obj/chair.obj");
//    m_ObjLoader->loadOBJ("../resource/Obj/Vases.obj", false);
    m_ObjLoader->m_Position = Floor->m_Position + glm::vec3(4.0f, 2.0f, -1.0f);
    m_ObjLoader->m_Scale = {0.1, 0.1f, 0.1f, 0.1f};
    m_GeometrySet.insert(m_ObjLoader);
    selectedGeometry = m_ObjLoader;

    // 环境光
    m_Shader->setUniform4f("u_Ambient", 0.2f, 0.2, 0.2f, 1.0f);

    // 纹理
    m_TextureArray = std::make_shared<TextureArray>(m_Shader);
    m_Shader->setUniform1i("u_Textures", 0); // 纹理是Texture0


    // 阴影
    auto ShadowShader = std::make_shared<Shader>("../resource/Scene/Shadow.shader");
    m_Shader->setUniform1i("u_DepthMap", m_TextureArray->getImageNum()); // TEXTURE 0~ImageNum-1被纹理占用
    m_Shadow = std::make_shared<Shadow>(ShadowShader);
    m_Shadow->setSamples(m_Shader);
    m_Shadow->render(m_GeometrySet, m_LightSet);
    // 初始采样点数目
    u_SampleNum = 10;
    m_Shader->bind();
    m_Shader->setUniform1i("u_SampleNum", u_SampleNum);
    u_SampleArea = 0.001;
    m_Shader->setUniform1f("u_SampleArea", u_SampleArea);
}
