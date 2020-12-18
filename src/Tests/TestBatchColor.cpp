//
// Created by 邱泽鸿 on 2020/12/4.
//

#include "TestBatchColor.h"
#include "Renderer.h"

namespace test
{
    TestBatchColor::TestBatchColor()
            : m_ColorA(glm::vec4(0.18f, 0.6f, 0.96f, 1.0f)),
              m_ColorB(glm::vec4(1.0f, 0.93f, 0.24f, 1.0f)),
              m_Proj(glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -1.0f, 1.0f)), // orthographic matrix
              m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0, 0)))
    {
        float positions[] = {
                /* 第一个矩形 */
                // <点坐标>    <颜色rgba>
                -0.5f, -0.5f, m_ColorA[0], m_ColorA[1], m_ColorA[2], m_ColorA[3], // 0
                0.5f, -0.5f, m_ColorA[0], m_ColorA[1], m_ColorA[2], m_ColorA[3], // 1
                0.5f,  0.5f, m_ColorA[0], m_ColorA[1], m_ColorA[2], m_ColorA[3], // 2
                -0.5f,  0.5f, m_ColorA[0], m_ColorA[1], m_ColorA[2], m_ColorA[3], // 3

                /* 第二个矩形 */
                // <点坐标>
                0.5f,  0.5f, m_ColorB[0], m_ColorB[1], m_ColorB[2], m_ColorB[3], // 4
                1.5f,  0.5f, m_ColorB[0], m_ColorB[1], m_ColorB[2], m_ColorB[3], // 5
                1.5f,  1.5f, m_ColorB[0], m_ColorB[1], m_ColorB[2], m_ColorB[3], // 6
                0.5f,  1.5f, m_ColorB[0], m_ColorB[1], m_ColorB[2], m_ColorB[3], // 7
        };

        /* index buffer */
        unsigned int indices[] = { // 必须用unsigned
                /* 第一个矩形 */
                0, 1, 2, // draw the first triangle
                2, 3, 0,  // draw the second triangle

                /* 第二个矩形 */
                4, 5, 6, // draw the first triangle
                6, 7, 4  // draw the second triangle
        }; // 定义在CPU上

        // blend function参数设置为src = src_alpha, dest = 1 - src_alpha
        // 因为默认的blend function为ADD（即src + dest）
        // 所以这样写的效果是rgb = src_rgb * src_alpha + dest_rgb * (1-src_alpha)
        DebugCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        DebugCall(glEnable(GL_BLEND));

        m_VAO = std::make_unique<VertexArray>();
        m_VertexBuffer = std::make_unique<VertexBuffer>(positions, sizeof(positions), true);
        m_Layout = std::make_unique<VertexBufferLayout>();

        m_Layout->Push<float>(2); // 矩形的点
        m_Layout->Push<float>(4); // 矩形点的颜色


        m_VAO->addBuffer(*m_VertexBuffer, *m_Layout);
        // 放到GPU
        m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 8 * (2 + 4), true);

        m_Shader = std::make_shared<Shader>("../resource/TestBatchColor.shader");
        m_Shader->bind();
    }

    void TestBatchColor::OnRender()
    {
        DebugCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        DebugCall(glClear(GL_COLOR_BUFFER_BIT));

        Renderer renderer;
        {
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader->bind();
            m_Shader->setUniformMat4f("u_MVP", mvp);
            renderer.draw(*m_VAO, *m_IndexBuffer, m_Shader);
        }
    }

    void TestBatchColor::OnImGuiRender()
    {
        ImGui::ColorEdit4("Color A", &m_ColorA.x);
        ImGui::ColorEdit4("Color B", &m_ColorB.x);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

    void TestBatchColor::OnUpdate(GLFWwindow *Window, float deltaTime)
    {

    }
}
