//
// Created by 邱泽鸿 on 2021/1/1.
//

#include "ObjLoader.h"

ObjLoader::ObjLoader(const std::shared_ptr<Camera> &camera, const std::shared_ptr<Shader> &shader,
                     const glm::vec3 &Position, const Material &material, const Rotation &rotation, const Scale &Scale)
        : Geometry(camera, shader, Position, material, rotation, Scale)
{
    m_Layout->Push<float>(3); // 点坐标
    m_Layout->Push<float>(3); // 法向量
    m_Layout->Push<float>(4); // 颜色
    m_Layout->Push<float>(2); // 纹理坐标


    SupplementarySave = [&](std::ofstream& Out) -> bool{
        Out << m_FileName << std::endl;
        return true;
    };

    SupplementaryLoad = [&](std::ifstream& In){
        std::string Line;
        std::stringstream ss;
        while (std::getline(In, Line))
        {
            if (Line.length() == 0) continue;
            ss.clear();
            ss.str(Line);
            ss >> m_FileName;
            loadOBJ(m_FileName);
            break;
        }
    };
}

void ObjLoader::loadOBJ(const std::string &FileName)
{
    std::stringstream ss;
//    std::ifstream inFile("../resource/Obj/" + FileName + ".obj");
    std::ifstream inFile("../resource/Obj/" + FileName);

    m_FileName = FileName;
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
    
    std::vector<unsigned int> PositionIndices;
    std::vector<unsigned int> TexIndices;
    std::vector<unsigned int> NormalIndices;

    // Indices，声明绘制三角形的方式
    m_Indices.clear();
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
            std::vector<unsigned int> Points;
            while (ss >> Index)
            {
                if (Counter == 0)
                {
                    Points.push_back(PositionIndices.size());
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
            // 读完了一行所有的顶点，现在将其三角化
            triangulate(Points, m_Indices);
        }
    }

    // Vertex Array
    m_Vertices.resize(PositionIndices.size());

    bool m_hasTexture = (TexIndices.size() == PositionIndices.size()); // 是否具有纹理坐标
    for (int i = 0;i < m_Vertices.size(); ++i)
    {
        // Obj文件从1开始计数，所以要-1
        m_Vertices[i].Position = Positions[PositionIndices[i] - 1];
        if (m_hasTexture)
        {
            m_Vertices[i].TexCoord = TexCoords[TexIndices[i] - 1];
        }
        else
        {
            m_Vertices[i].TexCoord = glm::vec2(0.0f);
        }
        m_Vertices[i].Normal = Normals[NormalIndices[i] - 1];
        m_Vertices[i].Color = glm::vec4(1.0f);
    }

    // DEBUG
    std::cout << "Number of Vertices: " << m_Vertices.size() << std::endl;
    std::cout << "Load Obj successfully!" << std::endl;

    // Buffer分配空间
    m_VertexBuffer = std::make_unique<VertexBuffer>(&m_Vertices[0], m_Vertices.size() * sizeof(Vertex), true);
    m_IndexBuffer = std::make_unique<IndexBuffer>( &m_Indices[0], m_Indices.size(), true);
    // 绑定VAO
//    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, Indices.size() * sizeof(unsigned int), &Indices[0]);
//    glBufferSubData(GL_ARRAY_BUFFER, 0, Vertices.size() * sizeof(Vertex), &Vertices[0]); // No allocation, only send data
    m_VAO->addBuffer(*m_VertexBuffer, *m_Layout);
}

void ObjLoader::updateDrawData()
{

}
