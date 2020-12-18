//
// Created by 邱泽鸿 on 2020/12/13.
//

#pragma once

#include "Basic.h"

class Camera
{
public:
    Camera(const glm::mat4 &Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f),
           const glm::vec3 &Position = glm::vec3(0.0f, 0.0f, 0.0f),
           const glm::vec3 &InitUp = glm::vec3(0.0f, 1.0f, 0.0f), // 相机初始上方是y轴正方向
           const glm::vec3 &InitDirection = glm::vec3(0.0f, 0.0f, -1.0f), // 相机屏幕z轴负方向
           const float &Yaw = 0.0f, const float &Pitch = 0.0f);
    virtual ~Camera() = default;
    void OnKeyAction(GLFWwindow *Window, float deltaTime); // 键盘输入，需要移动
    inline glm::mat4 getViewMatrix() const
    {
//        std::cout << m_Direction.x  << "  " << m_Direction.y << "  " << m_Direction.z<< std::endl;
        return m_ViewMatrix;
    }
    inline void setProjection(const glm::mat4 &Projection)
    {
        m_Projection = Projection;
    }
    inline glm::mat4 getProjectionMatrix() const
    {
        return m_Projection;
    }
    inline glm::vec3 getPosition() const
    {
        return m_Position;
    }
    inline glm::vec3 getDirection() const
    {
        return m_Direction;
    }
    void OnMouseAction(GLFWwindow* window, glm::vec2 Position);

private:
    void updateCameraVectors(); // 俯仰角或者偏航角改变后，更新相机向量
public:
    bool isFPS;
protected:
    glm::mat4 m_Projection; // 投影矩阵
    glm::mat4 m_ViewMatrix; // 视图矩阵
    glm::vec3 m_Position;
    glm::vec3 m_InitDirection;
    glm::vec3 m_Direction;
    glm::vec3 m_InitUp;
    glm::vec3 m_Up;
    glm::vec3 m_InitRight;
    glm::vec3 m_Right;

    glm::quat m_qPitch; // 俯仰角对应四元数
    glm::quat m_qYaw; // 偏航角对应四元数
    float m_Pitch; // 俯仰角，角度制，相当于绕InitRight的旋转角
    float m_Yaw; // 偏航角，角度制，相当于绕InitUp的旋转角
};
