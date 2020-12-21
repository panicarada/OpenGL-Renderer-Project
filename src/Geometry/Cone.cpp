//
// Created by 邱泽鸿 on 2020/12/17.
//

#include "Cone.h"
#include "Renderer.h"
#include <random>

Cone::Cone(const std::shared_ptr<Camera>& Camera, const std::shared_ptr<Shader> &shader,
           const glm::vec3 &position, const Material& material, const Rotation& rotation, const Scale& Scale)
        : Geometry(Camera, shader, position, material, rotation, Scale)
{
    //    m_VAO = std::make_unique<VertexArray>();
    //    m_Layout = std::make_unique<VertexBufferLayout>();
    m_Layout->Push<float>(3); // 点坐标
    m_Layout->Push<float>(3); // 法向量
    m_Layout->Push<float>(4); // 颜色

    updateSubdivision(30);
}

void Cone::updateSubdivision(int Steps)
{
// 和updateDrawData()一样，只不过需要重新为Buffer分配内存，并且更新Steps
    m_Steps = Steps;

    // 重新计算圆锥上的点，并放入buffer中
    std::vector<Vertex> Vertices((m_Steps << 1) + 2);
    // 随机数
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-0.1f, 0.2f);//uniform distribution between 0 and 1

    // 第一部分，放入点
    {
        #pragma omp parallel for num_thread(CORE_NUM)
        for (int i = 0;i < m_Steps; ++i)
        {
            double Theta = M_PI * 2.0 * i / (m_Steps - 1);
            float x = m_Scale.x * cos(Theta);
            float y = 0.0f;
            float z = m_Scale.z * sin(Theta);

            Vertices[(i << 1) + 0] = {glm::vec3(x, y, z), glm::vec3(0.0f, -1.0f, 0.0f),
                                      m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)};

            Vertices[(i << 1) + 1] = {glm::vec3(x, y, z), glm::vec3(x, 0.0f, z),
                                      m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)};
        }
    }
    // 圆锥顶点
    int Center2 = Vertices.size();
    Vertices.push_back({glm::vec3(0.0f, m_Scale.y, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)});

    // 圆心
    int Center1 = Vertices.size();
    Vertices.push_back({glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)});


    // 第二部分，放入indices

    std::vector<unsigned int> Indices(6 * m_Steps);
    // 圆
    {
        #pragma omp parallel num_thread(CORE_NUM)
        for (int i = 0;i < m_Steps-1; ++i)
        {
            Indices[3*i + 0] = (i << 1);
            Indices[3*i + 1] = Center1;
            Indices[3*i + 2] = (i << 1) + 2;
        }
        Indices[3*m_Steps - 3] = (m_Steps - 1) << 1;
        Indices[3*m_Steps - 2] = Center1;
        Indices[3*m_Steps - 1] = 0;
    }

    int Offset = 3 * m_Steps;
    // 侧面
    {
        #pragma omp parallel num_thread(CORE_NUM)
        for (int i = 0; i < m_Steps - 1; ++i)
        {
            Indices[Offset + 3*i + 0] = Center2;
            Indices[Offset + 3*i + 1] = (i << 1) + 1;
            Indices[Offset + 3*i + 2] = (i << 1) + 3;
        }

        Indices[Offset + 3*m_Steps - 3] = Center2;
        Indices[Offset + 3*m_Steps - 2] = ((m_Steps - 1) << 1) + 1;
        Indices[Offset + 3*m_Steps - 1] = 1;
    }

    // 重新分配空间
    m_VertexBuffer = std::make_unique<VertexBuffer>(nullptr, Vertices.size() * sizeof(Vertex), false);
    m_IndexBuffer = std::make_unique<IndexBuffer>(nullptr, Indices.size(), false);
    // 绑定VAO
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, Indices.size() * sizeof(unsigned int), &Indices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, Vertices.size() * sizeof(Vertex), &Vertices[0]); // No allocation, only send data
    m_VAO->addBuffer(*m_VertexBuffer, *m_Layout);
}

void Cone::updateDrawData()
{
    // 重新计算圆锥上的点，并放入buffer中
    std::vector<Vertex> Vertices((m_Steps << 1) + 2);

    // 随机数
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-0.1f, 0.2f);//uniform distribution between 0 and 1

    // 第一部分，放入点
    {
        #pragma omp parallel for num_thread(CORE_NUM)
        for (int i = 0;i < m_Steps; ++i)
        {
            double Theta = M_PI * 2.0 * i / (m_Steps - 1);
            float x = m_Scale.x * cos(Theta);
            float y = 0.0f;
            float z = m_Scale.z * sin(Theta);

            Vertices[(i << 1) + 0] = {glm::vec3(x, y, z), glm::vec3(0.0f, -1.0f, 0.0f),
                                      m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)};

            Vertices[(i << 1) + 1] = {glm::vec3(x, y, z), glm::vec3(x, 0.0f, z),
                                      m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)};
        }
    }
    // 圆锥顶点
    int Center2 = Vertices.size();
    Vertices.push_back({glm::vec3(0.0f, m_Scale.y, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)});

    // 圆心
    int Center1 = Vertices.size();
    Vertices.push_back({glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)});


    // 第二部分，放入indices

    std::vector<unsigned int> Indices(6 * m_Steps);
    // 圆
    {
        #pragma omp parallel num_thread(CORE_NUM)
        for (int i = 0;i < m_Steps-1; ++i)
        {
            Indices[3*i + 0] = (i << 1);
            Indices[3*i + 1] = Center1;
            Indices[3*i + 2] = (i << 1) + 2;
        }
        Indices[3*m_Steps - 3] = (m_Steps - 1) << 1;
        Indices[3*m_Steps - 2] = Center1;
        Indices[3*m_Steps - 1] = 0;
    }

    int Offset = 3 * m_Steps;
    // 侧面
    {
        #pragma omp parallel num_thread(CORE_NUM)
        for (int i = 0; i < m_Steps - 1; ++i)
        {
            Indices[Offset + 3*i + 0] = Center2;
            Indices[Offset + 3*i + 1] = (i << 1) + 1;
            Indices[Offset + 3*i + 2] = (i << 1) + 3;
        }

        Indices[Offset + 3*m_Steps - 3] = Center2;
        Indices[Offset + 3*m_Steps - 2] = ((m_Steps - 1) << 1) + 1;
        Indices[Offset + 3*m_Steps - 1] = 1;
    }

    // 重新分配空间
    m_VertexBuffer = std::make_unique<VertexBuffer>(nullptr, Vertices.size() * sizeof(Vertex), false);
    m_IndexBuffer = std::make_unique<IndexBuffer>(nullptr, Indices.size(), false);

    // 绑定VAO
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, Indices.size() * sizeof(unsigned int), &Indices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, Vertices.size() * sizeof(Vertex), &Vertices[0]); // No allocation, only send data
    m_VAO->addBuffer(*m_VertexBuffer, *m_Layout);
}