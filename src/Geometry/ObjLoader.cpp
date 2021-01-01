//
// Created by 邱泽鸿 on 2021/1/1.
//

#include "ObjLoader.h"

ObjLoader::ObjLoader(const std::shared_ptr<Camera> &Camera, const std::shared_ptr<Shader> &Shader,
                     const glm::vec3 &Position, const Material &material, const Rotation &rotation, const Scale &Scale)
        : Geometry(Camera, Shader, Position, material, rotation, Scale)
{
    m_Layout->Push<float>(3); // 点坐标
    m_Layout->Push<float>(3); // 法向量
    m_Layout->Push<float>(4); // 颜色
    m_Layout->Push<float>(2); // 纹理坐标
}

void ObjLoader::loadOBJ(const std::string &FileName, bool isTriangle)
{
    std::stringstream ss;
    std::ifstream inFile(FileName);
    std::string Line = "";

    // 打开文件
    if (!inFile.is_open())
    {
        std::cout << "ERROR::OBJLOADER::Could not open file '" << FileName << "'!" << std::endl;
    }

    // Vertices
    std::vector<glm::vec3> Positions;
    std::vector<glm::vec2> TexCoords;
    std::vector<glm::vec3> Normals;
    // Face Vectors，一个face3*3个数组为一组，表明一个三角形三个顶点格子的Position、TexCoord、Normal的索引（文件中的位置）
    // 有些文件一行face是4*3，表示(1, 2, 3), (2, 3, 4)两个三角形所组成的quad，我们在输入处说明
    std::vector<unsigned int> PositionIndices;
    std::vector<unsigned int> TexIndices;
    std::vector<unsigned int> NormalIndices;


    // 每次读取一行
    while (std::getline(inFile, Line))
    {
        ss.clear();
        ss.str(Line);
        // 1. 获得第一个前缀
        std::string Prefix = "";
        ss >> Prefix;

        if (Prefix == "v")
        { // 坐标
            glm::vec3 Temp;
            ss >> Temp.x >> Temp.y >> Temp.z;
            Positions.push_back(Temp);
        }
        else if (Prefix == "vt")
        { // 纹理坐标
            glm::vec2 Temp;
            ss >> Temp.x >> Temp.y;
            TexCoords.push_back(Temp);
        }
        else if (Prefix == "vn")
        { // 法向量
            glm::vec3 Temp;
            ss >> Temp.x >> Temp.y >> Temp.z;
            Normals.push_back(Temp);
        }
        else if (Prefix == "f")
        {
            int Counter = 0; // 0: Position, 1: TexCoord, 2: Normal
            unsigned int Index;
            while (ss >> Index)
            {
                if (Counter == 0)
                {
                    PositionIndices.push_back(Index);
                }
                else if (Counter == 1)
                {
                    TexIndices.push_back(Index);
                }
                else if (Counter == 2)
                {
                    NormalIndices.push_back(Index);
                }

                int ignoreCount = 1;
                while (ss.peek() == '/')
                { // 有些obj没有材质，所以会连着两个'/'
                    Counter ++;
                    ss.ignore(ignoreCount, '/');
                    ignoreCount++;
                }
                if (ss.peek() == ' ')
                {
                    Counter ++;
                    ss.ignore(1, ' ');
                }

                if (Counter > 2)
                { // 读完了一个Vertex
                    Counter = 0;
                }
            }
        }
    }

    // Vertex Array
    std::vector<Vertex> Vertices(PositionIndices.size());
    // Indices
    std::vector<unsigned int> Indices;

    bool m_hasTexture = (TexIndices.size() == PositionIndices.size()); // 是否具有纹理坐标
    for (int i = 0;i < Vertices.size(); ++i)
    {
        // Obj文件从1开始计数，所以要-1
        Vertices[i].Position = Positions[PositionIndices[i] - 1];
        if (m_hasTexture)
        {
            Vertices[i].TexCoord = TexCoords[TexIndices[i] - 1];
        }
        else
        {
            Vertices[i].TexCoord = glm::vec2(0.0f);
        }
        Vertices[i].Normal = Normals[NormalIndices[i] - 1];
        Vertices[i].Color = glm::vec4(1.0f);

        if (isTriangle && i % 3 == 0)
        {
            Indices.push_back(i);
            Indices.push_back(i+1);
            Indices.push_back(i+2);
        }
        else if (!isTriangle && i % 4 == 0)
        { // 一行face是一个quad
            Indices.push_back(i);
            Indices.push_back(i+2);
            Indices.push_back(i+1);

            Indices.push_back(i+2);
            Indices.push_back(i);
            Indices.push_back(i+3);
        }
    }

    // DEBUG
    std::cout << "Number of Vertices: " << Vertices.size() << std::endl;
    std::cout << "Load Obj successfully!" << std::endl;



    // Buffer分配空间
    m_VertexBuffer = std::make_unique<VertexBuffer>(&Vertices[0], Vertices.size() * sizeof(Vertex), true);
    m_IndexBuffer = std::make_unique<IndexBuffer>( &Indices[0], Indices.size(), true);
    // 绑定VAO
//    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, Indices.size() * sizeof(unsigned int), &Indices[0]);
//    glBufferSubData(GL_ARRAY_BUFFER, 0, Vertices.size() * sizeof(Vertex), &Vertices[0]); // No allocation, only send data
    m_VAO->addBuffer(*m_VertexBuffer, *m_Layout);
}
