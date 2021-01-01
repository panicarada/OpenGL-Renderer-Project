//
// Created by 邱泽鸿 on 2020/12/14.
//

#pragma once
#include "Basic.h"
#include "Renderer.h"
#include "Camera.h"
#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "TextureArray.h"
#include "VertexBufferLayout.h"

// 四元数所用库
#include "glm/gtx/transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


#include <fstream>

class Geometry
{
public:
    inline static void exportObj(const std::string& OutFile, const std::set<std::shared_ptr<Geometry>>& GeometrySet)
    {
        std::string Extension = OutFile.substr(OutFile.find_last_of('.'));
        if (Extension != ".obj")
        { // 只支持obj文件
            std::cout << "The type of the file '" << OutFile << "' is not supported yet!" << std::endl;
            return ;
        }
        // 以写方式打开文件
        std::ofstream Out("../Export/" + OutFile);
        if (!Out.is_open())
        { // 打开文件失败
            std::cout << "Fail to write to the file '" << OutFile << "'" << std::endl;
            return ;
        }

        int Offset = 1; // 多个几何物体都写入该文件，索引号会叠加上去
        // 因为obj索引从1开始计数，所以初始化为1

        for (auto geometry : GeometrySet)
        {
            for (auto vertex : geometry->m_Vertices)
            { // 写入Vertices
                // 坐标
                Out << "v " << vertex.Position[0] << " " << vertex.Position[1] << " " << vertex.Position[2] << std::endl;
                // 纹理坐标
                Out << "vt " << vertex.TexCoord[0] << " " << vertex.TexCoord[1] << std::endl;
                // 法向量
                Out << "vn " << vertex.Normal[0] << " " << vertex.Normal[1] << " " << vertex.Normal[2] << std::endl;
            }

            // 这样一来，v、vt、vn的索引都是一样的，写入face三个分量都是重复的
            // 每行只写一个三角形
            for (int i = 0;i < geometry->m_Indices.size(); i += 3)
            {
                Out << "f " << geometry->m_Indices[i]+Offset << "/" << geometry->m_Indices[i]+Offset<< "/" << geometry->m_Indices[i]+Offset << " ";
                Out << geometry->m_Indices[i+1]+Offset << "/" << geometry->m_Indices[i+1]+Offset<< "/" << geometry->m_Indices[i+1]+Offset << " ";
                Out << geometry->m_Indices[i+2]+Offset << "/" << geometry->m_Indices[i+2]+Offset<< "/" << geometry->m_Indices[i+2]+Offset << std::endl;
            }
            Offset += geometry->m_Vertices.size();
        }

        Out.close();
    }

    inline void detachTexture(const std::shared_ptr<TextureArray>& TA)
    { // 取消纹理的绑定
        TA->eraseTexture(m_TextureSlot);
        m_TextureSlot = -1;
    }
    inline void draw() const
    {
        Renderer renderer;
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
        renderer.draw(*m_VAO, *m_IndexBuffer, m_Shader);
    }
    // 根据旋转角度重建旋转矩阵
    inline void updateRotation()
    {
        m_qPitch = glm::angleAxis(glm::radians(m_Rotation.Pitch), glm::vec3(1.0f, 0.0f, 0.0f));
        m_qYaw = glm::angleAxis(glm::radians(m_Rotation.Yaw), glm::vec3(0.0f, 1.0f, 0.0f));
        m_qRoll = glm::angleAxis(glm::radians(m_Rotation.Roll), glm::vec3(0.0f, 0.0f, 1.0f));

        glm::quat Orientation = glm::normalize(m_qPitch * m_qYaw * m_qRoll);
        m_RotateMatrix = glm::mat4_cast(Orientation);
    }

    virtual void updateDrawData() = 0; // 更新用于绘制的数据

    inline const glm::mat4 getModelMatrix() const
    {
        auto ScaleMat = glm::diagonal4x4(glm::vec4(m_Scale.x, m_Scale.y, m_Scale.z, 1.0f));
        auto TransMat = glm::translate(glm::mat4(1.0f), (m_Position));

        return TransMat * m_RotateMatrix * ScaleMat;
    }
    explicit Geometry(const std::shared_ptr<Camera>& Camera, const std::shared_ptr<Shader>& Shader, const glm::vec3 &Position = glm::vec3(0.0f, 0.0f, 0.0f),
             const Material& material = {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec4(1.0f), 1.0f},
             const Rotation& rotation = {0.0f, 0.0f, 0.0f},
             const Scale& Scale = {1.0f, 1.0f, 1.0f, 1.0f})
     : m_Camera(Camera), m_Shader(Shader), m_Position(Position), m_Rotation(rotation), m_Scale(Scale), m_Material(material), m_Color(0.48f, 0.75f, 0.81f, 1.0f)
     , m_TextureSlot(-1)
    {
        m_VAO = std::make_unique<VertexArray>();
        m_Layout = std::make_unique<VertexBufferLayout>();
        updateRotation();
    }
    virtual std::string getClassName() = 0; // 返回几何物体名字
protected:
    /* 渲染要用的数据结构 */
    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBuffer> m_VertexBuffer; // 存放Vertex的结构
    std::unique_ptr<IndexBuffer> m_IndexBuffer; // 描述绘制三角形所用Vertex的顺序
    std::unique_ptr<VertexBufferLayout> m_Layout;
    // 为了导出obj文件，还是要把Vertices和Indices存起来
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;

    std::shared_ptr<Camera> m_Camera;
public:
    std::shared_ptr<Shader> m_Shader;

private:
    glm::quat m_qPitch; // 俯仰角对应四元数
    glm::quat m_qYaw; // 偏航角对应四元数
    glm::quat m_qRoll; // 滚转角对应四元数
public:
    std::string Tag; // 关于该几何体的一些描述

    Material m_Material; // 材质
    glm::vec4 m_Color; // 颜色
    int m_TextureSlot; // 纹理

    /* 几何参数 */
    // 三个方向的拉伸，相对于几何物体的坐标
    Scale m_Scale;
    glm::vec3 m_Position; // 物体位置
    glm::mat4 m_RotateMatrix; // 旋转矩阵
    /* 以下角度类似于Camera，
     * 但是为了方便几何图形都默认InitDirection = z轴正方向，InitUp = y轴正方向 */
    Rotation m_Rotation;
};

