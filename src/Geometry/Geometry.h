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
#include "VertexBufferLayout.h"

// 四元数所用库
#include "glm/gtx/transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


class Geometry
{

public:
    inline void draw() const
    {
        Renderer renderer;
        glm::mat4 Model = this->getModelMatrix();
        glm::mat4 Projection = m_Camera->getProjectionMatrix();
        glm::mat4 View = m_Camera->getViewMatrix();
        m_Shader->setUniformMat4f("u_model", Model);
        m_Shader->setUniformMat4f("u_projection", Projection);
        m_Shader->setUniformMat4f("u_view", View);
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
//        updateDrawData();
    }

    virtual void updateDrawData() = 0; // 更新用于绘制的数据

    inline const glm::mat4 getModelMatrix() const
    {
        return glm::translate(glm::mat4(1.0f), (m_Position)) * m_RotateMatrix;
    }
    explicit Geometry(const std::shared_ptr<Camera>& Camera, const std::shared_ptr<Shader>& Shader, const glm::vec3 &Position = glm::vec3(0.0f, 0.0f, 0.0f),
             Rotation rotation = {0.0f, 0.0f, 0.0f},
             Scale Scale = {1.0f, 1.0f, 1.0f, 1.0f})
     : m_Camera(Camera), m_Shader(Shader), m_Position(Position), m_Rotation(rotation), m_Scale(Scale), m_Color(0.6f, 0.12f, 0.9f, 1.0f)
    {
        m_VAO = std::make_unique<VertexArray>();
        m_Layout = std::make_unique<VertexBufferLayout>();
        updateRotation();
    }

    virtual std::string getClassName() = 0; // 返回几何物体名字
//    inline void setPitch(const float& Pitch) {m_Pitch = Pitch; updateRotation();};
//    inline void setYaw(const float& Yaw) {m_Yaw = Yaw; updateRotation();};
//    inline void setRoll(const float& Roll) {m_Roll = Roll; updateRotation();};
protected:
    /* 渲染要用的数据结构 */
    std::shared_ptr<Shader> m_Shader;
    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBuffer> m_VertexBuffer; // 存放Vertex的结构
    std::unique_ptr<IndexBuffer> m_IndexBuffer; // 描述绘制三角形所用Vertex的顺序
    std::unique_ptr<VertexBufferLayout> m_Layout;
    std::shared_ptr<Camera> m_Camera;
private:
    glm::quat m_qPitch; // 俯仰角对应四元数
    glm::quat m_qYaw; // 偏航角对应四元数
    glm::quat m_qRoll; // 滚转角对应四元数
public:
    std::string Comment; // 关于该几何体的一些描述

    /* 几何参数 */
    // 三个方向的拉伸，相对于几何物体的坐标
    glm::vec4 m_Color;
    Scale m_Scale;
    glm::vec3 m_Position; // 物体位置
    glm::mat4 m_RotateMatrix; // 旋转矩阵
    /* 以下角度类似于Camera，
     * 但是为了方便几何图形都默认InitDirection = z轴正方向，InitUp = y轴正方向 */
    Rotation m_Rotation;
//    float m_Pitch; // 俯仰角
//    float m_Yaw; // 偏航角
//    float m_Roll; // 滚转角
};

