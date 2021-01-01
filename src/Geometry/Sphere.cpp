//
// Created by 邱泽鸿 on 2020/12/14.
//

#include "Sphere.h"

#include <random>


Sphere::Sphere(const std::shared_ptr<Camera>& Camera, const std::shared_ptr<Shader> &shader,
               const glm::vec3 &position, const Material& material, const Rotation& rotation, const Scale& Scale)
   : Geometry(Camera, shader, position, material, rotation, Scale)
{
    m_Layout->Push<float>(3); // 点坐标
    m_Layout->Push<float>(3); // 法向量
    m_Layout->Push<float>(4); // 颜色
    m_Layout->Push<float>(2); // 纹理坐标

    m_VerticalSteps = 50;
    m_HorizontalSteps = 50;
    updateDrawData();
}

void Sphere::updateDrawData()
{
    // 重新计算球面上的点，并放入buffer中
    //    std::vector<Vertex> m_Vertices;
    // 改为并行
    m_Vertices.resize(m_VerticalSteps * m_HorizontalSteps);

    // 每遍历到球上某一点，我们就绘制两个相接的三角形，需要6个m_Indices
    m_Indices.resize(6 * (m_VerticalSteps-1) * m_HorizontalSteps);
    // 随机数
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<> distribution(0.0f, 0.0f);//uniform distribution between 0 and 1
    {// 电脑是八核的
        #pragma omp parallel for num_thread(CORE_NUM)
        for (int i = 0; i < m_VerticalSteps; ++i)
        {
            for (int j = 0; j < m_HorizontalSteps; ++j)
            {
                double Theta = 2.0 * M_PI * j / (m_HorizontalSteps - 1);
                double Phi = M_PI * i / (m_VerticalSteps - 1);
                double sinPhi = sin(Phi);
                double cosPhi = cos(Phi);

                glm::vec3 Position = glm::vec3(cos(Theta) * sinPhi, cosPhi,sin(Theta) * sinPhi);

                double rand1 = distribution(generator);
                double rand2 = distribution(generator);
                double rand3 = distribution(generator);

                m_Vertices[i * m_HorizontalSteps + j].Position = Position;
                m_Vertices[i * m_HorizontalSteps + j].Normal = Position;
                m_Vertices[i * m_HorizontalSteps + j].Color = m_Color + glm::vec4(rand1, rand2, rand3, 0.0f);
                m_Vertices[i * m_HorizontalSteps + j].TexCoord = glm::vec2(1.0*i/(m_VerticalSteps-1), 1.0*j/(m_HorizontalSteps-1));

                // m_Indices
                if (i < m_VerticalSteps - 1)
                {
                    m_Indices[6 * i * m_HorizontalSteps + 6*j + 0] = (i) * m_HorizontalSteps + j;
                    m_Indices[6 * i * m_HorizontalSteps + 6*j + 1] = (i + 1) * m_HorizontalSteps + (j + 1) % m_HorizontalSteps;
                    m_Indices[6 * i * m_HorizontalSteps + 6*j + 2] = (i + 1) * m_HorizontalSteps + j;

                    m_Indices[6 * i * m_HorizontalSteps + 6*j + 3] = (i) * m_HorizontalSteps + j;
                    m_Indices[6 * i * m_HorizontalSteps + 6*j + 4] = (i) * m_HorizontalSteps + (j + 1) % m_HorizontalSteps;
                    m_Indices[6 * i * m_HorizontalSteps + 6*j + 5] = (i + 1) * m_HorizontalSteps + (j + 1) % m_HorizontalSteps;
                }
            }
        }
    }

    // 重新分配空间
    m_VertexBuffer = std::make_unique<VertexBuffer>(nullptr, m_Vertices.size() * sizeof(Vertex), false);
    m_IndexBuffer = std::make_unique<IndexBuffer>(nullptr, m_Indices.size(), false);
    // 绑定VAO
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_Indices.size() * sizeof(unsigned int), &m_Indices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_Vertices.size() * sizeof(Vertex), &m_Vertices[0]); // No allocation, only send data
    m_VAO->addBuffer(*m_VertexBuffer, *m_Layout);
}

void Sphere::updateSubdivision(int VerticalSteps, int HorizontalSteps)
{
    m_VerticalSteps = VerticalSteps;
    m_HorizontalSteps = HorizontalSteps;
    updateDrawData();
}
