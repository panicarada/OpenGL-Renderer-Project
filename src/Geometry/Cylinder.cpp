//
// Created by 邱泽鸿 on 2020/12/17.
//

#include "Cylinder.h"
#include "Renderer.h"
#include <random>

Cylinder::Cylinder(const std::shared_ptr<Camera>& Camera, const std::shared_ptr<Shader> &shader,
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

void Cylinder::updateSubdivision(int Steps)
{
// 和updateDrawData()一样，只不过需要重新为Buffer分配内存，并且更新Steps
    m_Steps = Steps;

    // 重新计算圆柱上的点，并放入buffer中
//    std::vector<Vertex> Vertices;
    // 改成并行
    std::vector<Vertex> Vertices(4 * m_Steps + 2);

    // 随机数
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-0.1f, 0.2f);//uniform distribution between 0 and 1


    // 第一部分，放入Vertex
    {
        #pragma omp parallel for num_thread(NUM_CORE)
        for (int i = 0;i < m_Steps; ++i)
        {
            double Theta = M_PI * 2.0 * i / (m_Steps - 1);
            float upperX = m_Scale.w * m_Scale.z * cos(Theta);
            float lowerX = m_Scale.x * m_Scale.z * cos(Theta);
            float y = m_Scale.y;
            float upperZ = m_Scale.w * m_Scale.z * sin(Theta);
            float lowerZ = m_Scale.x * m_Scale.z * sin(Theta);

            // 上半部分对应于圆的vertex
            Vertices[(i<<2) + 0] = {glm::vec3(upperX, y, upperZ), glm::vec3(0.0f, 1.0f, 0.0f),
                                    m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)};
            // 上半部分对应于侧面的vertex（法向量是(x, 0, z)）
            Vertices[(i<<2) + 1] = {glm::vec3(upperX, y, upperZ), glm::vec3(upperX, 0.0f, upperZ),
                                    m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)};

            // 下半部分对应于圆的vertex
            Vertices[(i<<2) + 2] = {glm::vec3(lowerX, -y, lowerZ), glm::vec3(0.0f, -1.0f, 0.0f),
                                    m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)};
            // 下半部分对应于侧面的vertex（法向量是(x, 0, z)）
            Vertices[(i<<2) + 3] = {glm::vec3(lowerX, -y, lowerZ), glm::vec3(lowerX, 0.0f, lowerZ),
                                    m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)};

            /* 总结来说，2i的点是圆的点，2i+1的点是侧面的点，0 <= i < 2 * m_Steps */
        }
    }
    // 两个圆心
    int Center1 = Vertices.size();
    Vertices.push_back({glm::vec3(0.0f, m_Scale.y, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)});
    int Center2 = Vertices.size();
    Vertices.push_back({glm::vec3(0.0f, -m_Scale.y, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)});


    std::vector<unsigned int> Indices(6*m_Steps * 2);
    // 第二部分，放入index
    // 圆上的点
    {
        #pragma omp parallel for num_thread(CORE_NUM)
        for (int i = 0;i < m_Steps-1; ++i)
        {
            Indices[i*6 + 0] = Center1;
            Indices[i*6 + 1] = (i << 2); // 即4 * i
            Indices[i*6 + 2] = (i << 2) + 4;

            Indices[i*6 + 3] = (i << 2) + 2;
            Indices[i*6 + 4] = Center2;
            Indices[i*6 + 5] = (i << 2) + 6;
        }
        Indices[6 * m_Steps - 6] = Center1;
        Indices[6 * m_Steps - 5] = ((m_Steps-1) << 2);
        Indices[6 * m_Steps - 4] = 0;
        Indices[6 * m_Steps - 3] = ((m_Steps-1) << 2) + 2;
        Indices[6 * m_Steps - 2] = Center2;
        Indices[6 * m_Steps - 1] = 2;
    }


    int Offset = 6 * m_Steps;
    // 侧面的点
    {
        #pragma omp parallel for num_thead(CORE_NUM)
        for (int i = 0; i < m_Steps-1; ++i)
        {
            Indices[6*i + Offset + 0] = (i << 2) + 1; // 4i + 1
            Indices[6*i + Offset + 1] = (i << 2) + 3; // 4i + 3
            Indices[6*i + Offset + 2] = (i << 2) + 5; // 4i + 5

            Indices[6*i + Offset + 3] = (i << 2) + 5; // 4i + 5
            Indices[6*i + Offset + 4] = (i << 2) + 3; // 4i + 3
            Indices[6*i + Offset + 5] = (i << 2) + 7; // 4i + 7
        }

        Indices[Offset + 6 * m_Steps - 6] = ((m_Steps-1) << 2) + 1;
        Indices[Offset + 6 * m_Steps - 5] = ((m_Steps-1) << 2) + 3;
        Indices[Offset + 6 * m_Steps - 4] = 1;

        Indices[Offset + 6 * m_Steps - 3] = 1;
        Indices[Offset + 6 * m_Steps - 2] = ((m_Steps-1) << 2) + 3;
        Indices[Offset + 6 * m_Steps - 1] = 3;
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
//    std::vector<Vertex> Vertices;
    // 改成并行
    // 改成并行
    std::vector<Vertex> Vertices(4 * m_Steps + 2);

    // 随机数
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-0.1f, 0.2f);//uniform distribution between 0 and 1


    // 第一部分，放入Vertex
    {
        #pragma omp parallel for num_thread(NUM_CORE)
        for (int i = 0;i < m_Steps; ++i)
        {
            double Theta = M_PI * 2.0 * i / (m_Steps - 1);
            float upperX = m_Scale.w * m_Scale.z * cos(Theta);
            float lowerX = m_Scale.x * m_Scale.z * cos(Theta);
            float y = m_Scale.y;
            float upperZ = m_Scale.w * m_Scale.z * sin(Theta);
            float lowerZ = m_Scale.x * m_Scale.z * sin(Theta);

            // 上半部分对应于圆的vertex
            Vertices[(i<<2) + 0] = {glm::vec3(upperX, y, upperZ), glm::vec3(0.0f, 1.0f, 0.0f),
                                    m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)};
            // 上半部分对应于侧面的vertex（法向量是(x, 0, z)）
            Vertices[(i<<2) + 1] = {glm::vec3(upperX, y, upperZ), glm::vec3(upperX, 0.0f, upperZ),
                                    m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)};

            // 下半部分对应于圆的vertex
            Vertices[(i<<2) + 2] = {glm::vec3(lowerX, -y, lowerZ), glm::vec3(0.0f, -1.0f, 0.0f),
                                    m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)};
            // 下半部分对应于侧面的vertex（法向量是(x, 0, z)）
            Vertices[(i<<2) + 3] = {glm::vec3(lowerX, -y, lowerZ), glm::vec3(lowerX, 0.0f, lowerZ),
                                    m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)};

            /* 总结来说，2i的点是圆的点，2i+1的点是侧面的点，0 <= i < 2 * m_Steps */
        }
    }
    // 两个圆心
    int Center1 = Vertices.size();
    Vertices.push_back({glm::vec3(0.0f, m_Scale.y, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)});
    int Center2 = Vertices.size();
    Vertices.push_back({glm::vec3(0.0f, -m_Scale.y, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), m_Color + glm::vec4(dis(gen), dis(gen), dis(gen), 0.0f)});


    std::vector<unsigned int> Indices(6*m_Steps * 2);
    // 第二部分，放入index
    // 圆上的点
    {
        #pragma omp parallel for num_thread(CORE_NUM)
        for (int i = 0;i < m_Steps-1; ++i)
        {
            Indices[i*6 + 0] = Center1;
            Indices[i*6 + 1] = (i << 2); // 即4 * i
            Indices[i*6 + 2] = (i << 2) + 4;

            Indices[i*6 + 3] = (i << 2) + 2;
            Indices[i*6 + 4] = Center2;
            Indices[i*6 + 5] = (i << 2) + 6;
        }
        Indices[6 * m_Steps - 6] = Center1;
        Indices[6 * m_Steps - 5] = ((m_Steps-1) << 2);
        Indices[6 * m_Steps - 4] = 0;
        Indices[6 * m_Steps - 3] = ((m_Steps-1) << 2) + 2;
        Indices[6 * m_Steps - 2] = Center2;
        Indices[6 * m_Steps - 1] = 2;
    }


    int Offset = 6 * m_Steps;
    // 侧面的点
    {
        #pragma omp parallel for num_thead(CORE_NUM)
        for (int i = 0; i < m_Steps-1; ++i)
        {
            Indices[6*i + Offset + 0] = (i << 2) + 1; // 4i + 1
            Indices[6*i + Offset + 1] = (i << 2) + 3; // 4i + 3
            Indices[6*i + Offset + 2] = (i << 2) + 5; // 4i + 5

            Indices[6*i + Offset + 3] = (i << 2) + 5; // 4i + 5
            Indices[6*i + Offset + 4] = (i << 2) + 3; // 4i + 3
            Indices[6*i + Offset + 5] = (i << 2) + 7; // 4i + 7
        }

        Indices[Offset + 6 * m_Steps - 6] = ((m_Steps-1) << 2) + 1;
        Indices[Offset + 6 * m_Steps - 5] = ((m_Steps-1) << 2) + 3;
        Indices[Offset + 6 * m_Steps - 4] = 1;

        Indices[Offset + 6 * m_Steps - 3] = 1;
        Indices[Offset + 6 * m_Steps - 2] = ((m_Steps-1) << 2) + 3;
        Indices[Offset + 6 * m_Steps - 1] = 3;
    }

    // 重新分配空间
    m_VertexBuffer = std::make_unique<VertexBuffer>(nullptr, Vertices.size() * sizeof(Vertex), false);
    m_IndexBuffer = std::make_unique<IndexBuffer>(nullptr, Indices.size(), false);

    // 绑定VAO
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, Indices.size() * sizeof(unsigned int), &Indices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, Vertices.size() * sizeof(Vertex), &Vertices[0]); // No allocation, only send data
    m_VAO->addBuffer(*m_VertexBuffer, *m_Layout);
}