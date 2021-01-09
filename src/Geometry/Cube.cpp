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
    init();
}

void Cube::init()
{
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
    m_Indices.clear();
    for (unsigned int i = 0; i < 6; ++i)
    {
        unsigned int offset = (i << 2);
        m_Indices.push_back(offset); m_Indices.push_back(offset + 1); m_Indices.push_back(offset + 2);
        m_Indices.push_back(offset); m_Indices.push_back(offset + 2); m_Indices.push_back(offset + 3);
    }
    // 立方体只需要为buffer分配一次空间
    m_VertexBuffer = std::make_unique<VertexBuffer>(&m_Vertices[0], m_Vertices.size() * sizeof(Vertex), true);
    m_IndexBuffer = std::make_unique<IndexBuffer>(&m_Indices[0], m_Indices.size(), true);
    // 绑定VAO
    m_VAO->addBuffer(m_VertexBuffer, m_Layout);
}

