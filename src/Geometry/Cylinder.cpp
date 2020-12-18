//
// Created by 邱泽鸿 on 2020/12/17.
//

#include "Cylinder.h"
#include "Renderer.h"
#include <random>

Cylinder::Cylinder(const std::shared_ptr<Camera> &Camera, const std::shared_ptr<Shader> &Shader,
                   const glm::vec3 &Position, Rotation rotation, Scale Scale) : Geometry(Camera, Shader, Position,
                                                                                         rotation, Scale)
{
    //    m_VAO = std::make_unique<VertexArray>();
    //    m_Layout = std::make_unique<VertexBufferLayout>();
    m_Layout->Push<float>(3); // 点坐标
    m_Layout->Push<float>(3); // 法向量
    m_Layout->Push<float>(4); // 颜色

    updateSubdivision(30);
}

void Cylinder::updateSubdivision(int Steps)
{
// 和updateDrawData()一样，只不过需要重新为Buffer分配内存，并且更新Steps
    m_Steps = Steps;

    // 重新计算圆柱上的点，并放入buffer中
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;

    // 随机数
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-0.1f, 0.2f);//uniform distribution between 0 and 1

    // 第一部分，上面的圆形

    double Theta = 0;
    double deltaTheta = M_PI * 2 / m_Steps;

    // 第一部分，放入vertex
    for (int i = 0;i < m_Steps; ++i)
    {
        float upperX = m_Scale.w * m_Scale.z * cos(Theta);
        float lowerX = m_Scale.x * m_Scale.z * cos(Theta);
        float y = m_Scale.y;
        float upperZ = m_Scale.w * m_Scale.z * sin(Theta);
        float lowerZ = m_Scale.x * m_Scale.z * sin(Theta);

        // 上半部分对应于圆的vertex
        Vertices.push_back({glm::vec3(upperX, y, upperZ), glm::vec3(0.0f, 1.0f, 0.0f),
                            m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)});
        // 上半部分对应于侧面的vertex（法向量是(x, 0, z)）
        Vertices.push_back({glm::vec3(upperX, y, upperZ), glm::vec3(upperX, 0.0f, upperZ),
                            m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)});

        // 下半部分对应于圆的vertex
        Vertices.push_back({glm::vec3(lowerX, -y, lowerZ), glm::vec3(0.0f, -1.0f, 0.0f),
                            m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)});
        // 下半部分对应于侧面的vertex（法向量是(x, 0, z)）
        Vertices.push_back({glm::vec3(lowerX, -y, lowerZ), glm::vec3(lowerX, 0.0f, lowerZ),
                            m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)});
        Theta += deltaTheta;

        /* 总结来说，2i的点是圆的点，2i+1的点是侧面的点，0 <= i < 2 * m_Steps */
    }
    // 两个圆心
    int Center1 = Vertices.size();
    Vertices.push_back({glm::vec3(0.0f, m_Scale.y, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)});
    int Center2 = Vertices.size();
    Vertices.push_back({glm::vec3(0.0f, -m_Scale.y, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)});


    // 第二部分，放入index
    // 圆上的点
    for (int i = 0;i < 4 * m_Steps; i += 4)
    {
        if (i < 4 * m_Steps - 4)
        {
            Indices.push_back(Center1);
            Indices.push_back(i);
            Indices.push_back(i+4);

            Indices.push_back(i+2);
            Indices.push_back(Center2);
            Indices.push_back(i+6);
        }
        else
        {
            Indices.push_back(Center1);
            Indices.push_back(i);
            Indices.push_back(0);

            Indices.push_back(i+2);
            Indices.push_back(Center2);
            Indices.push_back(2);
        }
    }

    // 侧面的点
    for (int i = 1;i < 4 * m_Steps; i += 4)
    {
        if (i < 4 * m_Steps - 6)
        {
            Indices.push_back(i);
            Indices.push_back(i+2);
            Indices.push_back(i+4);

            Indices.push_back(i+4);
            Indices.push_back(i+2);
            Indices.push_back(i+6);
        }
        else
        {
            Indices.push_back(i);
            Indices.push_back(i+2);
            Indices.push_back(1);

            Indices.push_back(1);
            Indices.push_back(i+2);
            Indices.push_back(3);
        }
    }

    // 重新分配空间
    m_VertexBuffer = std::make_unique<VertexBuffer>(nullptr, Vertices.size() * sizeof(Vertex), false);
    m_IndexBuffer = std::make_unique<IndexBuffer>(nullptr, Indices.size(), false);
    // 绑定VAO
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, Indices.size() * sizeof(unsigned int), &Indices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, Vertices.size() * sizeof(Vertex), &Vertices[0]); // No allocation, only send data
    m_VAO->addBuffer(*m_VertexBuffer, *m_Layout);
}

void Cylinder::updateDrawData()
{
    // 重新计算圆柱上的点，并放入buffer中
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;

    // 随机数
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-0.1f, 0.2f);//uniform distribution between 0 and 1


    double Theta = 0;
    double deltaTheta = M_PI * 2 / m_Steps;

    // 第一部分，放入vertex
    for (int i = 0;i < m_Steps; ++i)
    {
        float upperX = m_Scale.w * m_Scale.z * cos(Theta);
        float lowerX = m_Scale.x * m_Scale.z * cos(Theta);
        float y = m_Scale.y;
        float upperZ = m_Scale.w * m_Scale.z * sin(Theta);
        float lowerZ = m_Scale.x * m_Scale.z * sin(Theta);

        // 上半部分对应于圆的vertex
        Vertices.push_back({glm::vec3(upperX, y, upperZ), glm::vec3(0.0f, 1.0f, 0.0f),
                            m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)});
        // 上半部分对应于侧面的vertex（法向量是(x, 0, z)）
        Vertices.push_back({glm::vec3(upperX, y, upperZ), glm::vec3(upperX, 0.0f, upperZ),
                            m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)});

        // 下半部分对应于圆的vertex
        Vertices.push_back({glm::vec3(lowerX, -y, lowerZ), glm::vec3(0.0f, -1.0f, 0.0f),
                            m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)});
        // 下半部分对应于侧面的vertex（法向量是(x, 0, z)）
        Vertices.push_back({glm::vec3(lowerX, -y, lowerZ), glm::vec3(lowerX, 0.0f, lowerZ),
                            m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)});
        Theta += deltaTheta;

        /* 总结来说，2i的点是圆的点，2i+1的点是侧面的点，0 <= i < 2 * m_Steps */
    }
    // 两个圆心
    int Center1 = Vertices.size();
    Vertices.push_back({glm::vec3(0.0f, m_Scale.y, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)});
    int Center2 = Vertices.size();
    Vertices.push_back({glm::vec3(0.0f, -m_Scale.y, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)});


    // 第二部分，放入index
    // 圆上的点
    for (int i = 0;i < 4 * m_Steps; i += 4)
    {
        if (i < 4 * m_Steps - 4)
        {
            Indices.push_back(Center1);
            Indices.push_back(i);
            Indices.push_back(i+4);

            Indices.push_back(i+2);
            Indices.push_back(Center2);
            Indices.push_back(i+6);
        }
        else
        {
            Indices.push_back(Center1);
            Indices.push_back(i);
            Indices.push_back(0);

            Indices.push_back(i+2);
            Indices.push_back(Center2);
            Indices.push_back(2);
        }
    }

    // 侧面的点
    for (int i = 1;i < 4 * m_Steps; i += 4)
    {
        if (i < 4 * m_Steps - 6)
        {
            Indices.push_back(i);
            Indices.push_back(i+2);
            Indices.push_back(i+4);

            Indices.push_back(i+2);
            Indices.push_back(i+4);
            Indices.push_back(i+6);
        }
        else
        {
            Indices.push_back(i);
            Indices.push_back(i+2);
            Indices.push_back(0);

            Indices.push_back(0);
            Indices.push_back(i+2);
            Indices.push_back(2);
        }
    }

    // 重新分配空间
    m_VertexBuffer = std::make_unique<VertexBuffer>(nullptr, Vertices.size() * sizeof(Vertex), false);
    m_IndexBuffer = std::make_unique<IndexBuffer>(nullptr, Indices.size(), false);

    // 绑定VAO
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, Indices.size() * sizeof(unsigned int), &Indices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, Vertices.size() * sizeof(Vertex), &Vertices[0]); // No allocation, only send data
    m_VAO->addBuffer(*m_VertexBuffer, *m_Layout);
}