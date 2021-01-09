//
// Created by 邱泽鸿 on 2020/12/14.
//

#include "Sphere.h"

#include <random>


Sphere::Sphere(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Shader> &shader,
               const glm::vec3 &position, const Material& material, const Rotation& rotation, const Scale& Scale)
   : Geometry(camera, shader, position, material, rotation, Scale)
{
    m_VerticalSteps = 50;
    m_HorizontalSteps = 50;
    updateDrawData();

    // 函数指针的加载
    // 球类需要额外储存细分度的信息
    SupplementarySave = [&](std::ofstream& Out) -> bool{
        Out << m_HorizontalSteps << " " << m_VerticalSteps << std::endl;
        return true;
    };
    SupplementaryLoad = [&](std::ifstream& In)
    {
        std::string Line;
        std::stringstream ss;
        while (std::getline(In, Line))
        {
            if (Line.length() == 0) continue;
            ss.clear();
            ss.str(Line);
            ss >> m_HorizontalSteps >> m_VerticalSteps;
            break;
        }
    };
}

void Sphere::updateDrawData()
{
    // 重新计算球面上的点，并放入buffer中
    //    std::vector<Vertex> m_Vertices;
    // 改为并行
    m_Vertices.resize(m_VerticalSteps * m_HorizontalSteps);

    // 每遍历到球上某一点，我们就绘制两个相接的三角形，需要6个m_Indices
    m_Indices.resize(6 * (m_VerticalSteps-1) * m_HorizontalSteps);

    {
        #pragma omp parallel for schedule(guided,2)
        for (int i = 0; i < m_VerticalSteps; ++i)
        {
            for (int j = 0; j < m_HorizontalSteps; ++j)
            {
                double Theta = 2.0 * M_PI * j / (m_HorizontalSteps - 1);
                double Phi = M_PI * i / (m_VerticalSteps - 1);
                double sinPhi = sin(Phi);
                double cosPhi = cos(Phi);

                glm::vec3 Position = glm::vec3(cos(Theta) * sinPhi, cosPhi,sin(Theta) * sinPhi);

                m_Vertices[i * m_HorizontalSteps + j].Position = Position;
                m_Vertices[i * m_HorizontalSteps + j].Normal = Position;
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
    m_VertexBuffer = std::make_unique<VertexBuffer>(&m_Vertices[0], m_Vertices.size() * sizeof(Vertex), false);
    m_IndexBuffer = std::make_unique<IndexBuffer>(&m_Indices[0], m_Indices.size(), false);
    m_VAO->addBuffer(m_VertexBuffer, m_Layout);
}

void Sphere::updateSubdivision(int VerticalSteps, int HorizontalSteps)
{
    m_VerticalSteps = VerticalSteps;
    m_HorizontalSteps = HorizontalSteps;
    updateDrawData();
}
