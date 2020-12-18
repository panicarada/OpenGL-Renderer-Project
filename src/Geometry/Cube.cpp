//
// Created by 邱泽鸿 on 2020/12/17.
//

#include "Cube.h"
#include "Renderer.h"

#include <random>

Cube::Cube(const std::shared_ptr<Camera> &Camera, const std::shared_ptr<Shader> &Shader, const glm::vec3 &Position,
           Rotation rotation, Scale Scale) : Geometry(Camera, Shader, Position, rotation, Scale)
{
//    m_VAO = std::make_unique<VertexArray>();
//    m_Layout = std::make_unique<VertexBufferLayout>();
    m_Layout->Push<float>(3); // 点坐标
    m_Layout->Push<float>(3); // 法向量
    m_Layout->Push<float>(4); // 颜色

    struct temp
    { // 纯粹为了下面的Indices读起来方便而定义的
        unsigned int x;
        unsigned int y;
        unsigned int z;
    };
    std::vector<temp> Indices =
    {
            {0, 1, 2}, {0, 2, 3}, // 每一行代表一个正方形
            {1, 0, 4}, {1, 4, 5},
            {2, 1, 5}, {2, 5, 6},
            {3, 2, 6}, {3, 6, 7},
            {6, 5, 4}, {6, 4, 7},
            {4, 0, 3}, {4, 3, 7}
    };

    // 立方体只需要为buffer分配一次空间
    m_VertexBuffer = std::make_unique<VertexBuffer>(nullptr, 8 * sizeof(Vertex), false);
    m_IndexBuffer = std::make_unique<IndexBuffer>(&Indices[0].x, 6*6, false);
    updateDrawData();
}

void Cube::updateDrawData()
{

    // 随机数
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-0.1f, 0.2f);//uniform distribution between 0 and 1

    // 重新计算球面上的点，并放入buffer中
    std::vector<Vertex> Vertices =
    {
            {{-m_Scale.x, m_Scale.y, -m_Scale.z}, {0.0f, 0.0f, 0.0f}, m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)},
            {{m_Scale.x, m_Scale.y, -m_Scale.z}, {0.0f, 0.0f, 0.0f}, m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)},
            {{m_Scale.x, m_Scale.y, m_Scale.z}, {0.0f, 0.0f, 0.0f}, m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)},
            {{-m_Scale.x, m_Scale.y, m_Scale.z}, {0.0f, 0.0f, 0.0f}, m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)},
            {{-m_Scale.x, -m_Scale.y, -m_Scale.z}, {0.0f, 0.0f, 0.0f}, m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)},
            {{m_Scale.x, -m_Scale.y, -m_Scale.z}, {0.0f, 0.0f, 0.0f}, m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)},
            {{m_Scale.x, -m_Scale.y, m_Scale.z}, {0.0f, 0.0f, 0.0f}, m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)},
            {{-m_Scale.x, -m_Scale.y, m_Scale.z}, {0.0f, 0.0f, 0.0f}, m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)}
    };

    // 绑定VAO
    glBufferSubData(GL_ARRAY_BUFFER, 0, Vertices.size() * sizeof(Vertex), &Vertices[0]); // No allocation, only send data
    m_VAO->addBuffer(*m_VertexBuffer, *m_Layout);
}