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

#include "Light.h"


class Geometry
{
protected:
    std::function<bool(std::ofstream&)> SupplementarySave = [](std::ofstream&) -> bool{
        return true; // 如果子类需要额外储存其他信息，就更改这个函数指针
    };
    std::function<void(std::ifstream&)> SupplementaryLoad = [](std::ifstream&)
    { // 如果子类需要额外读取其他信息，就更改这个函数指针

    };
public:
    // 子类名字和对应构造函数的对应表
    // 在Basic中初始化这张表，这样就能在基类函数中初始化子类了
    static std::unordered_map<std::string, std::function<std::shared_ptr<Geometry>
            (const std::shared_ptr<Camera>& camera, const std::shared_ptr<Shader>& shader)>> ConstructorMap;
public:
    bool save(std::ofstream& Out);
    static std::shared_ptr<Geometry> load(std::ifstream& In, const std::shared_ptr<Camera>& camera,
                                                 const std::shared_ptr<Shader>& shader);

    // 导出obj文件
    void exportObj(const std::string& OutFile) const;

    inline void detachTexture(const std::shared_ptr<TextureArray>& TA)
    { // 取消纹理的绑定
        TA->eraseTexture(m_TextureSlot);
        m_TexturePath = "NONE";
        m_TextureSlot = -1;
    }
    void draw() const;
    // 根据旋转角度重建旋转矩阵
    void updateRotation();

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
     , m_TextureSlot(-1), m_TexturePath("NONE")
    {
        m_VAO = std::make_unique<VertexArray>();
        m_Layout = std::make_unique<VertexBufferLayout>();
        updateRotation();
    }
    virtual std::string getClassName() const = 0; // 返回几何物体名字
protected:
    /* 渲染要用的数据结构 */
    std::shared_ptr<VertexArray> m_VAO;
    std::shared_ptr<VertexBuffer> m_VertexBuffer; // 存放Vertex的结构
    std::shared_ptr<IndexBuffer> m_IndexBuffer; // 描述绘制三角形所用Vertex的顺序
    std::shared_ptr<VertexBufferLayout> m_Layout;
    std::vector<unsigned int> m_Indices;

    std::shared_ptr<Camera> m_Camera;
// 为了导出obj文件，还是要把Vertices和Indices存起来
std::vector<Vertex> m_Vertices;
public:
    std::shared_ptr<Shader> m_Shader;

private:
    glm::quat m_qPitch; // 俯仰角对应四元数
    glm::quat m_qYaw; // 偏航角对应四元数
    glm::quat m_qRoll; // 滚转角对应四元数
public:
    std::string Tag = "NONE"; // 关于该几何体的一些描述

    Material m_Material; // 材质
    glm::vec4 m_Color; // 颜色
    int m_TextureSlot; // 纹理
    std::string m_TexturePath; // 纹理素材路径

    /* 几何参数 */
    // 三个方向的拉伸，相对于几何物体的坐标
    Scale m_Scale;
    glm::vec3 m_Position; // 物体位置
    glm::mat4 m_RotateMatrix; // 旋转矩阵
    /* 以下角度类似于Camera，
     * 但是为了方便几何图形都默认InitDirection = z轴正方向，InitUp = y轴正方向 */
    Rotation m_Rotation;
};

