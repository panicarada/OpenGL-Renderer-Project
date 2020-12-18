//
// Created by 邱泽鸿 on 2020/12/14.
//

#include "Sphere.h"

#include <random>


Sphere::Sphere(const std::shared_ptr<Camera>& Camera, const std::shared_ptr<Shader> &shader,
               const glm::vec3 &position, Rotation rotation, Scale Scale)
   : Geometry(Camera, shader, position, rotation, Scale)
{
//    m_VAO = std::make_unique<VertexArray>();
//    m_Layout = std::make_unique<VertexBufferLayout>();
    m_Layout->Push<float>(3); // 点坐标
    m_Layout->Push<float>(3); // 法向量
    m_Layout->Push<float>(4); // 颜色
    updateSubdivision(50, 50);
}

void Sphere::updateSubdivision(int VerticalSteps, int HorizontalSteps)
{   // 和updateDrawData()一样，只不过需要重新为Buffer分配内存，并且更新Steps
    m_VerticalSteps = VerticalSteps;
    m_HorizontalSteps = HorizontalSteps;


    // 重新计算球面上的点，并放入buffer中
    std::vector<Vertex> Vertices;

    // 每遍历到球上某一点，我们就绘制两个相接的三角形，需要6个indices
    std::vector<unsigned int> Indices;

    double Phi = 0.0;
    double deltaPhi = M_PI / (m_VerticalSteps-1);
    double deltaTheta = 2.0 * M_PI / (m_HorizontalSteps-1);
    // 随机数
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<> distribution(0.0f, 0.0f);//uniform distribution between 0 and 1

    for (int i = 0;i < m_VerticalSteps; ++i)
    {
//        double Phi = M_PI * i / (m_VerticalSteps-1);
        double Theta = 0;
        double sinPhi = sin(Phi);
        double cosPhi = cos(Phi);
        for (int j = 0; j < m_HorizontalSteps; ++j)
        {
//            double Theta = 2.0 * M_PI * j / (m_HorizontalSteps-1);

            glm::vec3 Position = glm::vec3(m_Scale.x * cos(Theta) * sinPhi, -m_Scale.y * cosPhi,
                                           m_Scale.z * sin(Theta) * sinPhi);
            double rand1 = distribution(generator);
            double rand2 = distribution(generator);
            double rand3 = distribution(generator);

            Vertices.push_back({Position, glm::normalize(Position),
                                m_Color + glm::vec4(rand1, rand2, rand3, 0.0f)
                               }); // 点坐标

//            Theta += deltaTheta;

            if (i < m_VerticalSteps - 1)
            {
                Indices.push_back((i) * m_HorizontalSteps + j);
                Indices.push_back((i + 1) * m_HorizontalSteps + (j + 1) % m_HorizontalSteps);
                Indices.push_back((i + 1) * m_HorizontalSteps + j);

                Indices.push_back((i) * m_HorizontalSteps + j);
                Indices.push_back((i) * m_HorizontalSteps + (j + 1) % m_HorizontalSteps);
                Indices.push_back((i + 1) * m_HorizontalSteps + (j + 1) % m_HorizontalSteps);
            }

            Theta += deltaTheta;
        }
        Phi += deltaPhi;
    }

    // 重新分配空间
    m_VertexBuffer = std::make_unique<VertexBuffer>(nullptr, Vertices.size() * sizeof(Vertex), false);
    m_IndexBuffer = std::make_unique<IndexBuffer>(nullptr, Indices.size(), false);
    // 绑定VAO
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, Indices.size() * sizeof(unsigned int), &Indices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, Vertices.size() * sizeof(Vertex), &Vertices[0]); // No allocation, only send data
    m_VAO->addBuffer(*m_VertexBuffer, *m_Layout);
}

void Sphere::updateDrawData()
{
    // 重新计算球面上的点，并放入buffer中
    std::vector<Vertex> Vertices;

    // 每遍历到球上某一点，我们就绘制两个相接的三角形，需要6个indices
    std::vector<unsigned int> Indices;

    double Phi = 0.0;
    double deltaPhi = M_PI / (m_VerticalSteps-1);
    double deltaTheta = 2.0 * M_PI / (m_HorizontalSteps-1);
    // 随机数
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<> distribution(0.0f, 0.0f);//uniform distribution between 0 and 1

    for (int i = 0;i < m_VerticalSteps; ++i)
    {
//        double Phi = M_PI * i / (m_VerticalSteps-1);
        double Theta = 0;
        double sinPhi = sin(Phi);
        double cosPhi = cos(Phi);
        for (int j = 0; j < m_HorizontalSteps; ++j)
        {
//            double Theta = 2.0 * M_PI * j / (m_HorizontalSteps-1);

            glm::vec3 Position = glm::vec3(m_Scale.x * cos(Theta) * sinPhi, -m_Scale.y * cosPhi,
                                           m_Scale.z * sin(Theta) * sinPhi);
            double rand1 = distribution(generator);
            double rand2 = distribution(generator);
            double rand3 = distribution(generator);

            Vertices.push_back({Position, glm::normalize(Position),
                                m_Color + glm::vec4(rand1, rand2, rand3, 0.0f)
                               }); // 点坐标

//            Theta += deltaTheta;

            if (i < m_VerticalSteps - 1)
            {
                Indices.push_back((i) * m_HorizontalSteps + j);
                Indices.push_back((i + 1) * m_HorizontalSteps + (j + 1) % m_HorizontalSteps);
                Indices.push_back((i + 1) * m_HorizontalSteps + j);

                Indices.push_back((i) * m_HorizontalSteps + j);
                Indices.push_back((i) * m_HorizontalSteps + (j + 1) % m_HorizontalSteps);
                Indices.push_back((i + 1) * m_HorizontalSteps + (j + 1) % m_HorizontalSteps);
            }
            

            Theta += deltaTheta;
        }
        Phi += deltaPhi;
    }

    // 重新分配空间
    m_VertexBuffer = std::make_unique<VertexBuffer>(nullptr, Vertices.size() * sizeof(Vertex), false);
    m_IndexBuffer = std::make_unique<IndexBuffer>(nullptr, Indices.size(), false);
    // 绑定VAO
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, Indices.size() * sizeof(unsigned int), &Indices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, Vertices.size() * sizeof(Vertex), &Vertices[0]); // No allocation, only send data
    m_VAO->addBuffer(*m_VertexBuffer, *m_Layout);
}
