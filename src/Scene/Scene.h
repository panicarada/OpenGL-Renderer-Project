//
// Created by 邱泽鸿 on 2020/12/14.
//

#pragma once

#include "Test.h"
#include "Geometry.h"
#include "Light.h"
#include "Shadow.h"
#include "ObjLoader.h"
#include <set>

namespace test
{

    class Scene : public Test
    {
    private:
        void init(const std::string& ShaderFile, const std::string& ShadowFile); // 初始化函数，在构造器以及load中都有调用
    public:
        Scene();
        ~Scene() = default;
        bool save(const std::string& FileName) const; // 保存场景
        bool load(const std::string& FileName);// 读取场景

        void OnUpdate(GLFWwindow *Window, float deltaTime) override;
        void OnRender() override;
        void OnImGuiRender() override;
        inline std::shared_ptr<Camera> getCamera() override
        {
            return m_Camera;
        }
        void OnKeyAction(int key, int mods) override;
    private:
        void GuiGeometry(); // 处理集合物体部分的GUI
        void GuiTexture(); // 处理纹理部分的GUI
        void GuiLight(); // 处理光源部分的GUI
        void GuiShadow(); // 处理阴影部分的GUI
        void GuiScene(); // 处理Scene导入导出
        void GuiObj(); // 处理Obj导入导出
    private:
        // 几何物体
        std::set<std::shared_ptr<Geometry>> m_GeometrySet; // 几何物体的集合
        std::shared_ptr<Geometry> selectedGeometry; // 当前选中物体
        glm::vec4 m_Color; // 当前选中物体的颜色
        glm::vec3 m_Position; // 当前选中物体的位置
        // 当前选中物体的细分度
        int m_Steps;
        int m_HorizontalSteps;
        int m_VerticalSteps;

        std::shared_ptr<Geometry> Floor; // 地板

        std::set<std::shared_ptr<Light>> m_LightSet; // 光源
        std::shared_ptr<Light> selectedLight; // 选中光源
        glm::vec4 u_Ambient; // 环境光

        std::shared_ptr<TextureArray> m_TextureArray; // 纹理

        // 阴影
        std::shared_ptr<Shadow> m_Shadow_0;
        std::shared_ptr<Shadow> m_Shadow_1;

        int u_SampleNum; // 阴影使用的采样点数目
        bool updateShadow; // 是否要更新阴影
        float u_SampleArea; // 采样的范围


        // GUI输入
        std::string m_TextureName; // 用户输入的纹理名称
        std::string m_SceneName_Save; // Scene名称，用于保存
        std::string m_SceneName_Load; // Scene名称，用于导入

        std::string m_ObjName_Import; // 要导入的obj文件名称
        std::string m_ObjName_Export; // 要导出的obj文件名称

        std::shared_ptr<Shader> m_Shader; // 主shader
        std::shared_ptr<Camera> m_Camera; // 相机
    };

}