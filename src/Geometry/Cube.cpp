//
// Created by 邱泽鸿 on 2020/12/17.
//

#include "Cube.h"
#include "Renderer.h"

#include <random>

Cube::Cube(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Shader> &shader,
           const glm::vec3 &position, const Material& material, const Rotation& rotation, const Scale& Scale)
        : Geometry(camera, shader, position, material, rotation, Scale)
{
    m_Layout->Push<float>(3); // 点坐标
    m_Layout->Push<float>(3); // 法向量
    m_Layout->Push<float>(4); // 颜色
    m_Layout->Push<float>(2); // 纹理坐标
    updateDrawData();
}

void Cube::updateDrawData()
{
// 随机数
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-0.1f, 0.2f);//uniform distribution between 0 and 1

    // 重新计算球面上的点，并放入buffer中
    m_Vertices.clear();

    std::vector<Vertex> temp = getSquare(glm::vec3(1.0f, 0.0f, 0.0f));
    m_Vertices.insert(m_Vertices.end(), temp.begin(), temp.end());
    temp.clear();
    temp = getSquare(glm::vec3(-1.0f, 0.0f, 0.0f));
    m_Vertices.insert(m_Vertices.end(), temp.begin(), temp.end());
    temp.clear();
    temp = getSquare(glm::vec3(0.0f, 1.0f, 0.0f));
    m_Vertices.insert(m_Vertices.end(), temp.begin(), temp.end());
    temp.clear();
    temp = getSquare(glm::vec3(0.0f, -1.0f, 0.0f));
    m_Vertices.insert(m_Vertices.end(), temp.begin(), temp.end());
    temp.clear();
    temp = getSquare(glm::vec3(0.0f, 0.0f, 1.0f));
    m_Vertices.insert(m_Vertices.end(), temp.begin(), temp.end());
    temp.clear();
    temp = getSquare(glm::vec3(0.0f, 0.0f, -1.0f));
    m_Vertices.insert(m_Vertices.end(), temp.begin(), temp.end());
    temp.clear();

    // 立方体只需要为buffer分配一次空间
    m_VertexBuffer = std::make_unique<VertexBuffer>(&m_Vertices[0], 24 * sizeof(Vertex), true);

    m_Indices.clear();
    for (unsigned int i = 0; i < 6; ++i)
    {
        unsigned int offset = (i << 2);
        m_Indices.push_back(offset); m_Indices.push_back(offset + 1); m_Indices.push_back(offset + 2);
        m_Indices.push_back(offset); m_Indices.push_back(offset + 2); m_Indices.push_back(offset + 3);
    }

    // 立方体只需要为buffer分配一次空间
    m_VertexBuffer = std::make_unique<VertexBuffer>(&m_Vertices[0], 24 * sizeof(Vertex), false);
    m_IndexBuffer = std::make_unique<IndexBuffer>(&m_Indices[0], 36, true);
    // 绑定VAO
    m_VAO->addBuffer(*m_VertexBuffer, *m_Layout);
}

