//
// Created by 邱泽鸿 on 2020/12/13.
//

#pragma once

#include "Basic.h"
#include "Camera.h"
#include <functional>
#include <vector>

namespace test
{
    class Test
    {
    public:
        Test()
        {}

        virtual ~Test() {}
        virtual void OnUpdate(GLFWwindow *Window, float deltaTime){}; // 实时的更新响应，deltaTime表示两轮响应相隔时间
        virtual void OnRender(){}; // 实时的绘制方程
        virtual void OnImGuiRender(){}; // ImGUI响应
        virtual void OnKeyAction(int key, int mods) {};

        virtual std::shared_ptr<Camera> getCamera()
        {
            return nullptr;
        }
    };


    class TestMenu : public Test
    {
    public:
        TestMenu(Test*& currentTestPointer)
            : m_CurrentTest(currentTestPointer){}

        void OnImGuiRender() override
        {
            for (auto& test : m_Tests)
            { // 查看哪个test对应按钮被点击
                if (ImGui::Button(test.first.c_str()))
                {
                    m_CurrentTest = test.second(); // 实例化对应测试
                }
            }
        }

        template<typename T>
        inline void RegisterTest(const std::string& name)
        {
            m_Tests.push_back(std::make_pair(name, [&name]()
            { // lambda表达式，只需类名就可以自己构造一个构造函数
                std::cout << "Registering test " << name << std::endl;
                return new T();
            }));
        }
    private:
        Test*& m_CurrentTest; // 当前的测试实例
        // 每个测试名字对应一个测试实例的构造函数
        std::vector<std::pair<std::string, std::function<Test*()>>> m_Tests;
    };
}
