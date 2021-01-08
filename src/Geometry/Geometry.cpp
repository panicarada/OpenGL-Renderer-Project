//
// Created by 邱泽鸿 on 2021/1/2.
//


#include "Geometry.h"

// 静态成员初始化
auto Geometry::ConstructorMap = std::unordered_map<std::string, std::function<std::shared_ptr<Geometry>
        (const std::shared_ptr<Camera>& camera, const std::shared_ptr<Shader>& shader)>>();

void Geometry::draw() const
{
    glm::mat4 Model = this->getModelMatrix();
    glm::mat4 Projection = m_Camera->getProjectionMatrix();
    glm::mat4 View = m_Camera->getViewMatrix();
    m_Shader->setUniformMat4f("u_Model", Model);
    m_Shader->setUniformMat4f("u_Projection", Projection);
    m_Shader->setUniformMat4f("u_View", View);

    // 设置材质
    m_Shader->setUniform4f("u_Material.Ambient", m_Material.Ambient);
    m_Shader->setUniform4f("u_Material.Diffuse", m_Material.Diffuse);
    m_Shader->setUniform4f("u_Material.Specular", m_Material.Specular);
    m_Shader->setUniform1f("u_Material.Highlight", m_Material.Highlight);

    if (m_TextureSlot >= 0)
    { // 如果有纹理，就设置纹理
        m_Shader->setUniform1i("u_TexIndex", m_TextureSlot);
    }
    else
    { // 负数下标表示没有纹理
        m_Shader->setUniform1i("u_TexIndex", -1);
    }
    Renderer::draw(m_VAO, m_IndexBuffer, m_Shader);
}

// 根据旋转角度重建旋转矩阵
void Geometry::updateRotation()
{
    m_qPitch = glm::angleAxis(glm::radians(m_Rotation.Pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    m_qYaw = glm::angleAxis(glm::radians(m_Rotation.Yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    m_qRoll = glm::angleAxis(glm::radians(m_Rotation.Roll), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::quat Orientation = glm::normalize(m_qPitch * m_qYaw * m_qRoll);
    m_RotateMatrix = glm::mat4_cast(Orientation);
}

void Geometry::exportObj(const std::string &OutFile) const
{
    // 以写方式打开文件
    std::ofstream Out("../Export/Objects/" + OutFile + ".obj");
    if (!Out.is_open())
    { // 打开文件失败
        std::cout << "Fail to write to the file '" << OutFile << "'" << std::endl;
        return ;
    }

    int Offset = 1; // 多个几何物体都写入该文件，索引号会叠加上去
    // 因为obj索引从1开始计数，所以初始化为1

    /* 写入Vertices */
    for (auto vertex : m_Vertices)
    {
        auto ModelMatrix = getModelMatrix();
        auto Position = ModelMatrix * glm::vec4(vertex.Position, 1.0f);
        auto Normal = glm::mat3(glm::transpose(glm::inverse(ModelMatrix))) * vertex.Normal;
        // 坐标
        Out << "v " << Position[0] << " " << Position[1] << " " << Position[2] << std::endl;
        // 法向量
        Out << "vn " << Normal[0] << " " << Normal[1] << " " << Normal[2] << std::endl;
        // 纹理坐标
        Out << "vt " << vertex.TexCoord[0] << " " << vertex.TexCoord[1] << std::endl;
    }

    // 这样一来，v、vt、vn的索引都是一样的，写入face三个分量都是重复的
    // 每行只写一个三角形
    for (int i = 0;i < m_Indices.size(); i += 3)
    {
        Out << "f " << m_Indices[i]+Offset << "/" << m_Indices[i]+Offset<< "/" << m_Indices[i]+Offset << " ";
        Out << m_Indices[i+1]+Offset << "/" << m_Indices[i+1]+Offset<< "/" << m_Indices[i+1]+Offset << " ";
        Out << m_Indices[i+2]+Offset << "/" << m_Indices[i+2]+Offset<< "/" << m_Indices[i+2]+Offset << std::endl;
    }
    Offset += m_Vertices.size();

    Out.close();
    std::cout << "Write file '" << OutFile << "' successfully!" << std::endl;
}


bool Geometry::save(std::ofstream& Out)
{ // 把当前几何物体的数据写入Out
    // 几何物体的类型
    Out << getClassName() << " " << Tag << std::endl;
    // 基础几何信息
    Out << "Position: " << m_Position.x << " " << m_Position.y << " " << m_Position.z << std::endl;
    Out << "Rotation: " << m_Rotation.Pitch << " " << m_Rotation.Roll << " " << m_Rotation.Yaw << std::endl;
    Out << "Color: " << m_Color[0] << " " << m_Color[1] << " " << m_Color[2] << " " << m_Color[3] << std::endl;
    Out << "Scale: " << m_Scale.w << " " << m_Scale.x << " " << m_Scale.y << " " << m_Scale.z << std::endl;
    Out << "Material::Ambient: " << m_Material.Ambient[0] << " " << m_Material.Ambient[1] << " " << m_Material.Ambient[2] << " " << m_Material.Ambient[3] << std::endl;
    Out << "Material::Diffuse: " << m_Material.Diffuse[0] << " " << m_Material.Diffuse[1] << " " << m_Material.Diffuse[2] << " " << m_Material.Diffuse[3] << std::endl;
    Out << "Material::Specular: " << m_Material.Specular[0] << " " << m_Material.Specular[1] << " " << m_Material.Specular[2] << " " << m_Material.Specular[3] << std::endl;
    Out << "Material::Highlight: " << m_Material.Highlight << std::endl;
    Out << "TextureSlot: " << m_TextureSlot << std::endl;
    Out << "TexturePath: " << m_TexturePath << std::endl;
    SupplementarySave(Out); // 如果子类需要额外储存其他信息，就更改这个函数指针 */
    return true;
}


std::shared_ptr<Geometry> Geometry::load(std::ifstream& In, const std::shared_ptr<Camera>& camera,
                                      const std::shared_ptr<Shader>& shader)
{
    std::string Line;
    int LineCounter = 0;
    std::shared_ptr<Geometry> geometry = nullptr;
    std::stringstream ss;
    std::cout << "-----" << std::endl;
    while (std::getline(In, Line))
    {
        ss.clear();
        ss.str(Line);
        if (Line.length() == 0)
        { // 跳过空行
            std::cout << "***" << std::endl;
            continue;
        }
        if (Line == "END_GEOMETRY")
        {
            break;
        }
        std::string Property = "";
        if (LineCounter == 0)
        {
            std::string ClassName = "";
            ss >> ClassName;
            ClassName = ClassName.substr(ClassName.find_last_of(':')+1);
            geometry = ConstructorMap[ClassName](camera, shader);
            ss >> geometry->Tag;
            LineCounter ++;
        }
        else
        {
            ss >> Property;
            if (Property == "Position:")
            {
                // 位置
                ss >> geometry->m_Position.x >> geometry->m_Position.y >> geometry->m_Position.z;
            }
            else if (Property == "Rotation:")
            {
                // 旋转
                ss >> Property >> geometry->m_Rotation.Pitch >> geometry->m_Rotation.Roll >> geometry->m_Rotation.Yaw;
            }
            else if (Property == "Color:")
            {
                // 颜色
                ss >> geometry->m_Color[0] >> geometry->m_Color[1] >> geometry->m_Color[2] >> geometry->m_Color[3];
            }
            else if (Property == "Scale:")
            {
                // 缩放
                ss >> geometry->m_Scale.w >> geometry->m_Scale.x>> geometry->m_Scale.y >> geometry->m_Scale.z;
            }
            else if (Property == "Material::Ambient:")
            {
                // 材质
                ss >> geometry->m_Material.Ambient[0] >> geometry->m_Material.Ambient[1] >> geometry->m_Material.Ambient[2] >> geometry->m_Material.Ambient[3];
            }
            else if (Property == "Material::Diffuse:")
            {
                ss >> geometry->m_Material.Diffuse[0] >> geometry->m_Material.Diffuse[1] >> geometry->m_Material.Diffuse[2] >> geometry->m_Material.Diffuse[3];
            }
            else if (Property == "Material::Specular:")
            {
                ss >> geometry->m_Material.Specular[0] >> geometry->m_Material.Specular[1] >> geometry->m_Material.Specular[2] >> geometry->m_Material.Specular[3];
            }
            else if (Property == "Material::Highlight:")
            {
                ss >> geometry->m_Material.Highlight;
            }
            else if (Property == "TextureSlot:")
            {
                ss >> geometry->m_TextureSlot;
            }
            else if (Property == "TexturePath:")
            {
                ss >> geometry->m_TexturePath;
                break;
            }
        }
    }
    if (geometry)
    {
        geometry->SupplementaryLoad(In);
        geometry->updateDrawData();
    }

    #undef __DEBUG
    #ifdef __DEBUG
    std::cout << "Position: " << geometry->m_Position.x << " " << geometry->m_Position.y << " " << geometry->m_Position.z << std::endl;
        std::cout << "Rotation: " << geometry->m_Rotation.Pitch << " " << geometry->m_Rotation.Roll << " " << geometry->m_Rotation.Yaw << std::endl;
        std::cout << "Color: " << geometry->m_Color[0] << " " << geometry->m_Color[1] << " " << geometry->m_Color[2] << " " << geometry->m_Color[3] << std::endl;
        std::cout << "Scale: " << geometry->m_Scale.w << " " << geometry->m_Scale.x << " " << geometry->m_Scale.y << " " << geometry->m_Scale.z << std::endl;
        std::cout << "Material::Ambient: " << geometry->m_Material.Ambient[0] << " " << geometry->m_Material.Ambient[1] << " " << geometry->m_Material.Ambient[2] << " " << geometry->m_Material.Ambient[3] << std::endl;
        std::cout << "Material::Diffuse: " << geometry->m_Material.Diffuse[0] << " " << geometry->m_Material.Diffuse[1] << " " << geometry->m_Material.Diffuse[2] << " " << geometry->m_Material.Diffuse[3] << std::endl;
        std::cout << "Material::Specular: " << geometry->m_Material.Specular[0] << " " << geometry->m_Material.Specular[1] << " " << geometry->m_Material.Specular[2] << " " << geometry->m_Material.Specular[3] << std::endl;
        std::cout << "Material::Highlight: " << geometry->m_Material.Highlight << std::endl;
        std::cout << "TextureSlot: " << geometry->m_TextureSlot << std::endl;
    #endif
    return geometry;
}