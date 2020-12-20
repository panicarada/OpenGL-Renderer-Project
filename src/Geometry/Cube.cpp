//
// Created by 邱泽鸿 on 2020/12/17.
//

#include "Cube.h"
#include "Renderer.h"

#include <random>

Cube::Cube(const std::shared_ptr<Camera>& Camera, const std::shared_ptr<Shader> &shader,
           const glm::vec3 &position, const Material& material, const Rotation& rotation, const Scale& Scale)
        : Geometry(Camera, shader, position, material, rotation, Scale)
{
    m_Layout->Push<float>(3); // 点坐标
    m_Layout->Push<float>(3); // 法向量
    m_Layout->Push<float>(4); // 颜色

    struct temp
    { // 纯粹为了下面的Indices读起来方便而定义的
        unsigned int x;
        unsigned int y;
        unsigned int z;
    };
    std::vector<temp> Indices;
    for (unsigned int i = 0; i < 6; ++i)
    {
        unsigned int offset = (i << 2);
        Indices.push_back({offset, offset + 1, offset + 2});
        Indices.push_back({offset, offset + 2, offset + 3});
    }


    // 立方体只需要为buffer分配一次空间
    m_VertexBuffer = std::make_unique<VertexBuffer>(nullptr, 24 * sizeof(Vertex), false);
    m_IndexBuffer = std::make_unique<IndexBuffer>(&Indices[0].x, 36, true);
    updateDrawData();
}

void Cube::updateDrawData()
{

    // 随机数
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-0.1f, 0.2f);//uniform distribution between 0 and 1

    // 重新计算球面上的点，并放入buffer中
    std::vector<Vertex> Vertices;

    std::vector<Vertex> temp = getSquare(glm::vec3(1.0f, 0.0f, 0.0f));
    Vertices.insert(Vertices.end(), temp.begin(), temp.end());
    temp.clear();
    temp = getSquare(glm::vec3(-1.0f, 0.0f, 0.0f));
    Vertices.insert(Vertices.end(), temp.begin(), temp.end());
    temp.clear();
    temp = getSquare(glm::vec3(0.0f, 1.0f, 0.0f));
    Vertices.insert(Vertices.end(), temp.begin(), temp.end());
    temp.clear();
    temp = getSquare(glm::vec3(0.0f, -1.0f, 0.0f));
    Vertices.insert(Vertices.end(), temp.begin(), temp.end());
    temp.clear();
    temp = getSquare(glm::vec3(0.0f, 0.0f, 1.0f));
    Vertices.insert(Vertices.end(), temp.begin(), temp.end());
    temp.clear();
    temp = getSquare(glm::vec3(0.0f, 0.0f, -1.0f));
    Vertices.insert(Vertices.end(), temp.begin(), temp.end());
    temp.clear();

    // 立方体只需要为buffer分配一次空间
    m_VertexBuffer = std::make_unique<VertexBuffer>(nullptr, 24 * sizeof(Vertex), false);
    // 绑定VAO
    glBufferSubData(GL_ARRAY_BUFFER, 0, Vertices.size() * sizeof(Vertex), &Vertices[0]); // No allocation, only send data
    m_VAO->addBuffer(*m_VertexBuffer, *m_Layout);
}
