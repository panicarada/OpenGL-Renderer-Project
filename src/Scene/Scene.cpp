//
// Created by 邱泽鸿 on 2020/12/14.
//

#include "Scene.h"
#include "Sphere.h"
#include "Cube.h"
#include "Cylinder.h"
#include <unordered_map>

// 查看目录下的文件
#include <filesystem>

void test::Scene::OnUpdate(GLFWwindow *Window, float deltaTime)
{
    m_Camera->OnKeyAction(Window, deltaTime);
}

void test::Scene::OnRender()
{
    DebugCall(glClearColor(0.1f, 0.3f, 0.5f, 0.6f));

//    m_Shadow_00->setSamples(m_Shader);
    if (updateShadow)
    {
        int Counter = 0;
        for (auto & light : m_LightSet)
        {
            if (Counter == 0)
            {
                m_Shadows[0]->setSamples(m_Shader);
            }
            glActiveTexture(GL_TEXTURE0 + m_TextureArray->getImageNum() + light->m_ID);
            m_Shadows[light->m_ID]->render(m_GeometrySet, light);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_Shadows[light->m_ID]->getDepthMap());
            Counter ++;
        }
        updateShadow = false;
    }

    // 清除z-buffer，用于深度测试；以及清除背景颜色
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // 启用材质
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_TextureArray->getRendererID());

    m_Shader->bind();
    m_Shader->setUniform1f("u_zNear", ZNEAR);
    m_Shader->setUniform1f("u_zFar", ZFAR);

    if (m_Camera)
    { // 输入相机位置
        auto Position = m_Camera->getPosition();
        if (selectedGeometry)
        { // 把相机观察目标设置为选中物体
            m_Camera->TargetPosition = selectedGeometry->m_Position;
        }
        m_Shader->setUniform3f("u_CameraPosition", Position.x, Position.y, Position.z);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& geometry : m_GeometrySet)
    {
        geometry->draw();
    }
    Floor->draw();
}

void test::Scene::OnImGuiRender()
{
    GuiLight();
    GuiShadow();
    GuiGeometry();
    GuiTexture();
    GuiScene();
    GuiObj();

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

test::Scene::Scene()
    : u_Ambient(0.2f, 0.2, 0.2f, 1.0f), u_SampleNum(10), u_SampleArea(0.001)
{
    init(Basic::getFileName("Scene"), Basic::getFileName("Shadow"));
}

void test::Scene::init(const std::string &ShaderFile, const std::string &ShadowFile)
{
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);
    m_Shader = std::make_shared<Shader>(ShaderFile);
    m_Shader->bind();
    m_Camera = std::make_shared<Camera>();
    selectedGeometry = nullptr;

    // 地板
    Floor = std::make_shared<Cube>(m_Camera, m_Shader);
    Floor->m_Scale = {20.0f, 20.0f, 0.0001f, 20.0f};
    Floor->m_Color = glm::vec4(0.18f, 0.6f, 0.96f, 1.0f);
    Floor->updateDrawData();
    Floor->m_Position = m_Camera->getPosition() + 10.0f * m_Camera->getDirection() - glm::vec3(0.0f, 2.0f, 0.0f);

    // 环境光
    m_Shader->setUniform4f("u_Ambient", u_Ambient[0], u_Ambient[1], u_Ambient[2], u_Ambient[3]);

    // 纹理
    m_TextureArray = std::make_shared<TextureArray>(m_Shader);
    m_Shader->setUniform1i("u_Textures", 0); // 纹理是Texture0

    // 阴影
    auto ShadowShader = std::make_shared<Shader>(ShadowFile);
    for (int i = 0;i < Basic::getConstant("Scene", "MAX_LIGHT_NUM"); ++i)
    {
        m_Shader->setUniform1i("u_DepthMap_" + std::to_string(i), m_TextureArray->getImageNum() + i); // TEXTURE 0~ImageNum-1被纹理占用
        m_Shadows.emplace_back(std::make_shared<Shadow>(ShadowShader));
    }


    // 初始采样点数目以及采样范围设置
    m_Shader->bind();
    m_Shader->setUniform1i("u_SampleNum", u_SampleNum);
    m_Shader->setUniform1f("u_SampleArea", u_SampleArea);

    std::cout << Basic::getConstant("Scene", "MAX_SAMPLE_NUM") << std::endl;
}

bool test::Scene::save(const std::string &FileName) const
{
    std::ofstream Out("../Export/Scenes/" + FileName + ".zephyr");
    // 主Shader文件
    Out << "Shader: " << m_Shader->getFilePath() << std::endl;
    // 阴影
    Out << "Shadow: " << m_Shadows[0]->m_Shader->getFilePath() << std::endl;
    Out << "SampleNum: " << u_SampleNum << std::endl;
    Out << "SampleArea: " <<  u_SampleArea << std::endl;

    // 相机
    m_Camera->save(Out);
    // 光
    Out << "Ambient: " << u_Ambient[0] << " " << u_Ambient[1] << " " << u_Ambient[2] << std::endl;
    for (auto& light : m_LightSet)
    {
        light->save(Out);
    }
    Out << "END_LIGHTING" << std::endl;
    for (auto& geometry : m_GeometrySet)
    {
        if (!geometry->save(Out))
        { // 保存失败
            std::cout << "Fail to save the scene!" << std::endl;
            return false;
        }
    }
    Out << "END_GEOMETRY" << std::endl;
    Out.close();
    return true;
}

bool test::Scene::load(const std::string &FileName)
{
    std::ifstream In("../Export/Scenes/" + FileName);

    if (!In.is_open())
    {
        std::cout << "Invalid Scene Name!" << std::endl;
        return false;
    }

    m_LightSet.clear();
    m_GeometrySet.clear();


    std::string str;
    In >> str >> str;
    Basic::setFileName("Scene", str); // shader文件名
    In >> str >> str;
    Basic::setFileName("Shadow", str); // shadow文件
    // 更新常量表
    Basic::init();
    In >> str >> u_SampleNum;
    In >> str >> u_SampleArea;
//    u_SampleArea /= 100.0f;
    // 重新初始化
    init(Basic::getFileName("Scene"), Basic::getFileName("Shadow"));

    m_Camera->load(In);
    In >> str >> u_Ambient[0] >> u_Ambient[1] >> u_Ambient[2];
    // 环境光
    m_Shader->setUniform4f("u_Ambient", u_Ambient[0], u_Ambient[1], u_Ambient[2], u_Ambient[3]);

    // 光源
    Light::load(In, m_Shader, m_LightSet);
    if (!m_LightSet.empty()) selectedLight = *(m_LightSet.begin());
    // 物体
    std::shared_ptr<Geometry> geometry;
    while ((geometry = Geometry::load(In, m_Camera, m_Shader)))
    {
        m_GeometrySet.insert(geometry);
        selectedGeometry = geometry;
    }
    In.close();
    // 更新纹理
    for (auto& item : m_GeometrySet)
    {
        if (item->m_TextureSlot >= 0)
        {
            item->m_TextureSlot = m_TextureArray->addTexture(item->m_TexturePath);
        }
    }
    updateShadow = true;
    OnRender();
    return true;
}

void test::Scene::GuiGeometry()
{
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

    /* 物体的集合 */
    std::vector<std::string> items;
    std::unordered_map<int, std::shared_ptr<Geometry>> GeometryMap; // 整数到指针的映射表
    items.clear();
    /* 把现在的几何物体做成listbox，这部分是参考ImGui官方示例的 */
    int i = 0;
    int selectedItem = -1;
    for (auto& geometry : m_GeometrySet)
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

    // 接下来的面板都和选中的几何物体有关
    if (!selectedGeometry)
    {
        return ;
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
}

void test::Scene::GuiTexture()
{
    std::vector<std::string> FileCollection;
    for (const auto& File : std::filesystem::directory_iterator("../resource/Textures/"))
    {
        std::string Path = File.path();
        std::string FileName = Path.substr(Path.find_last_of('/')+1);
        if (FileName[0] == '.') continue; // 隐藏文件
        FileCollection.emplace_back(FileName);
    }
    FileCollection.emplace_back(".detach Texture"); // 取消纹理
    static int selectedItem = -1;

    if (ImGui::Combo("Texture Selection", &selectedItem, FileCollection))
    {
        m_TextureName = FileCollection[selectedItem];
    }

    if (ImGui::Button("Load Texture"))
    {
        if (selectedGeometry)
        { // 尝试更换纹理
            if (m_TextureName.length() > 0)
            {
                if (m_TextureName == ".detach Texture")
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
                    selectedGeometry->m_TexturePath = "../resource/Textures/" + m_TextureName;
                }
                else
                {
                    std::cout << "The file: '" << m_TextureName << "' is not available!" << std::endl;
                }
            }
        }
    }
}

void test::Scene::GuiLight()
{
    /* 光源的集合 */
    std::unordered_map<int, std::shared_ptr<Light>> LightMap; // 整数到指针的映射表
    std::vector<std::string> items;
    int selectedItem = -1; // 选中光源在列表中的位置
    /* 把现在的光源做成listbox，这部分是参考ImGui官方示例的 */
    int i = 0;
    for (auto& light : m_LightSet)
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
}

void test::Scene::GuiScene()
{
    /* 场景 */
    // 导入场景
    // 查找文件夹中所有可以导入的Scene
    std::vector<std::string> FileCollection;
    for (const auto& File : std::filesystem::directory_iterator("../Export/Scenes/"))
    {
        std::string Path = File.path();
        std::string FileName = Path.substr(Path.find_last_of('/')+1);
        if (FileName[0] == '.') continue; // 隐藏文件
        FileCollection.push_back(FileName);
    }
    static int selectedItem = -1;
    if (ImGui::Combo("Scene Selection", &selectedItem, FileCollection))
    {
        m_SceneName_Load = FileCollection[selectedItem];
    }
    if (ImGui::Button("Load Scene"))
    {
        this->load(m_SceneName_Load);
    }

    // 保存场景
    if (ImGui::InputText("Scene Name (Save)", &m_SceneName_Save, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        std::cout << "Hello" << std::endl;
    }
    if (ImGui::Button("Save Scene"))
    {
        this->save(m_SceneName_Save);
    }
}

void test::Scene::GuiObj()
{
    /* Obj文件 */
    // 导入obj
    std::vector<std::string> FileCollection;
    for (const auto& File : std::filesystem::directory_iterator("../resource/Obj/"))
    {
        std::string Path = File.path();
        std::string FileName = Path.substr(Path.find_last_of('/')+1);
        if (FileName[0] == '.') continue; // 隐藏文件
        FileCollection.push_back(FileName);
    }
    static int selectedItem = -1;
    if (ImGui::Combo("Obj Selection", &selectedItem, FileCollection))
    {
        m_ObjName_Import = FileCollection[selectedItem];
    }

    if (ImGui::Button("Import Obj"))
    {
        auto m_ObjLoader = std::make_shared<ObjLoader>(m_Camera, m_Shader);
        m_ObjLoader->loadOBJ(m_ObjName_Import);
        m_ObjLoader->m_Position = m_Camera->getPosition() + 10.0f * m_Camera->getDirection();
        m_ObjLoader->m_Scale = {0.1, 0.1f, 0.1f, 0.1f};
        m_GeometrySet.insert(m_ObjLoader);
        selectedGeometry = m_ObjLoader;
        this->save(m_ObjName_Import);
    }
    // 导出obj
    if (ImGui::InputText("Obj Name (Export)", &m_ObjName_Export, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        std::cout << "Hello" << std::endl;
    }
    if (ImGui::Button("Export Obj") && selectedGeometry)
    { // 选中一个物体导出obj
        selectedGeometry->exportObj(m_ObjName_Export);
    }
}

void test::Scene::GuiShadow()
{
    if (ImGui::SliderInt("Shadow Sample Number", &u_SampleNum,0, Basic::getConstant("Scene", "MAX_SAMPLE_NUM")))
    { // 更新阴影采样点数目
        m_Shader->setUniform1i("u_SampleNum", u_SampleNum);
    }
    if (ImGui::DragFloat("Shadow Sample Area", &u_SampleArea, 0.00008f, 0.0f, 0.023f))
    {
        m_Shader->setUniform1f("u_SampleArea", u_SampleArea);
    }
}

void test::Scene::OnKeyAction(int key, int mods)
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
                updateShadow = true; // 更新阴影
            }
        }
        else
        { // 删除物体
            std::cout << "deleting object" << std::endl;
            if (selectedGeometry)
            {
                m_GeometrySet.erase(selectedGeometry);
                if (!m_GeometrySet.empty())
                {
                    selectedGeometry = *m_GeometrySet.begin(); // 选中随机一个物体作为选中物体
                }
                else selectedGeometry = nullptr;
                updateShadow = true; // 更新阴影
            }
        }
    }
    else if (key == GLFW_KEY_A && mods == GLFW_MOD_SHIFT)
    {
        // 查找文件夹中文件数目
        int Counter = 0;
        for (const auto& File : std::filesystem::directory_iterator("../Export/Screenshots/"))
        {
            std::string Path = File.path();
            std::string FileName = Path.substr(Path.find_last_of('/')+1);
            if (FileName[0] == '.') continue; // 隐藏文件
            Counter ++;
        }
        std::stringstream FileName;
        FileName << "Screenshot_" << Counter << ".png";
        Basic::exportImage(FileName.str());
        std::cout << "Taking Screenshot!" << std::endl;
    }
}
