//
// Created by 邱泽鸿 on 2020/12/23.
//

#include "TestTexture.h"
#include "Renderer.h"

test::TestTexture::TestTexture()
{
    float positions[] = {
            /* 第一个矩形 */
            // <点坐标>，   <纹理坐标>
            -0.5f, -0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.0f, 1.0f,

            /* 第二个矩形 */
            // <点坐标>，   <纹理坐标>
            0.5f, 0.5f, 0.0f, 0.0f,
            1.5f, 0.5f, 1.0f, 0.0f,
            1.5f, 1.5f, 1.0f, 1.0f,
            0.5f, 1.5f, 0.0f, 1.0f
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
    m_VertexBuffer = std::make_unique<VertexBuffer>(positions, sizeof(positions), false);
    m_Layout = std::make_unique<VertexBufferLayout>();

    m_Layout->Push<float>(2); // 物体坐标
    m_Layout->Push<float>(2); // 纹理坐标

    m_VAO->addBuffer(*m_VertexBuffer, *m_Layout);
    // 放到GPU
    m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 12, true);

    m_Shader = std::make_shared<Shader>("../resource/TestTexture.shader");
    m_Shader->bind();

//    TextureSet.push_back(std::make_shared<TextureArray>(m_Shader, "../resource/Textures/oriental-tiles.png"));
//    TextureSet.push_back(std::make_shared<TextureArray>(m_Shader, "../resource/Textures/moroccan-flower-dark.png"));
//    TextureSet.push_back(std::make_shared<TextureArray>(m_Shader, "../resource/Textures/moroccan-flower.png"));
//    TextureSet.push_back(std::make_shared<TextureArray>(m_Shader, "../resource/Textures/watercolor.png"));
//    TextureSet.push_back(std::make_shared<TextureArray>(m_Shader, "../resource/Textures/double-bubble.png"));
}

void test::TestTexture::OnRender()
{
    DebugCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    DebugCall(glClear(GL_COLOR_BUFFER_BIT));

    Renderer renderer;
    std::shared_ptr<TextureArray> texture;
    static int count = 0;
    if (count & 0b1000000)
    {
        texture = TextureSet[0];
    }
    if (count & 0b0100000)
    {
        count++;
        texture = TextureSet[1];
    }
    else if (count & 0b0010000)
    {
        count++;
        texture = TextureSet[2];
    }
    else if (count & 0b0001000)
    {
        count++;
        texture = TextureSet[3];
    }
    else if (count & 0b0000100)
    {
        count++;
        texture = TextureSet[4];
    }
    else
    {
        count++;
    }

    if (texture)
    {
        texture->bind();
//        m_Shader->setUniform1i("u_TexIndex", texture->getSlotID());
    }
    m_Shader->bind();
    renderer.draw(*m_VAO, *m_IndexBuffer, m_Shader);

    if (texture)
    {
        texture->unbind();
    }
}
