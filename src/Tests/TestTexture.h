//
// Created by 邱泽鸿 on 2020/12/14.
//

#pragma once

#include "Test.h"
#include "TextureArray.h"
#include "Light.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "Basic.h"
#include <set>


namespace test
{

    class TestTexture : public Test
    {
    public:
        TestTexture();
        ~TestTexture() = default;

        void OnRender() override;
        inline std::shared_ptr<Camera> getCamera() override
        {
            return m_Camera;
        }
    private:
        std::vector<std::shared_ptr<TextureArray>> TextureSet;
        std::shared_ptr<VertexArray> m_VAO;
        std::shared_ptr<IndexBuffer> m_IndexBuffer;
        std::shared_ptr<VertexBuffer> m_VertexBuffer;
        std::shared_ptr<VertexBufferLayout> m_Layout;

        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<Camera> m_Camera;
    };

}