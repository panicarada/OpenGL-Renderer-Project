//
// Created by 邱泽鸿 on 2020/12/14.
//

#pragma once

#include "Basic.h"

#include "Test.h"

#include "VertexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Camera.h"



namespace test
{
    class TestBatchColor : public Test
    {
    public:
        TestBatchColor();
        ~TestBatchColor()
        {
            std::cout << "deleting Test Batch Color" << std::endl;
        };

        void OnUpdate(GLFWwindow *Window, float deltaTime) override;
        void OnRender() override;
        void OnImGuiRender() override;
    private:
        glm::vec4 m_ColorA;
        glm::vec4 m_ColorB;

        std::unique_ptr<VertexArray> m_VAO;
        std::unique_ptr<IndexBuffer> m_IndexBuffer;
        std::shared_ptr<Shader> m_Shader;
        std::unique_ptr<VertexBuffer> m_VertexBuffer;
        std::unique_ptr<VertexBufferLayout> m_Layout;

        glm::mat4 m_Proj;
        glm::mat4 m_View;
    };
}

