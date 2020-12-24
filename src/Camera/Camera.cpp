//
// Created by 邱泽鸿 on 2020/12/13.
//

#include "Camera.h"

#include "glm/gtx/transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


void Camera::OnKeyAction(GLFWwindow *Window, float deltaTime)
{
    const static float MoveSpeed = 2.0f;
    float Velocity = MoveSpeed * deltaTime;
    if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
    {
        m_Position += m_Direction * Velocity;
    }
    if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
    {
        m_Position -= m_Direction * Velocity;
    }
    if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
    {
        m_Position -= m_Right * Velocity;
    }
    if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
    {
        m_Position += m_Right * Velocity;
    }
    updateCameraVectors();
}

Camera::Camera(const float& AngleOfView, const glm::mat4 &Projection, const glm::vec3 &Position, const glm::vec3 &InitUp,
               const glm::vec3 &InitDirection, const float &Yaw, const float &Pitch)
   : isFPS(false), m_Projection(Projection), m_Tag("Perspective"), m_Position(Position), m_InitDirection(glm::normalize(InitDirection)),
     m_Yaw(Yaw), m_Pitch(Pitch), m_AngleOfView(AngleOfView), InitAngleOfView(AngleOfView)
{
    m_Right = m_InitRight = glm::normalize(glm::cross(InitDirection, InitUp));
    m_Up = m_InitUp = glm::normalize(glm::cross(m_InitRight, InitDirection));
    m_Direction = glm::normalize(InitDirection);

    updateCameraVectors();
}

void Camera::updateCameraVectors()
{

    m_qPitch = glm::angleAxis(m_Pitch, -m_InitRight);
    m_qYaw = glm::angleAxis(m_Yaw, m_InitUp);

    glm::quat Orientation = glm::normalize(m_qYaw * m_qPitch);// * m_qRoll);
    glm::mat4 Rotate = glm::mat4_cast(Orientation);

    m_Direction = glm::normalize(Rotate * glm::vec4(m_InitDirection, 1.0f));
    m_Right = glm::normalize(Rotate * glm::vec4(m_InitRight, 1.0f));
    m_Up = glm::normalize(Rotate * glm::vec4(m_InitUp, 1.0f));

    m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Direction, m_Up);
}

void Camera::OnMouseAction(GLFWwindow *window, glm::vec2 Position)
{
    // 鼠标的敏感度
    const static float MouseSensitivity = 0.001f;
    static glm::vec2 lastPos = Position; // 上一次鼠标位置
    if (isFPS)
    {
        //相机在漫游模式
        glm::vec2 offset = MouseSensitivity * (Position - lastPos);
        m_Yaw -= offset.x;
        m_Pitch += offset.y;

        if (m_Pitch > 85.0f) m_Pitch = 85.0f;
        if (m_Pitch < -85.0f) m_Pitch = -85.0f;

        if (m_Yaw > 85.0f) m_Yaw = 85.0f;
        if (m_Yaw < -85.0f) m_Yaw = -85.0f;
        updateCameraVectors();
    }
    lastPos = Position; // 鼠标位置还是要实时更新的
}

void Camera::OnScrollAction(const double &Offset)
{ // zoom
    if (std::abs(Offset) > 0.1 && m_Tag == "Perspective")
    { // 敏感度过滤
        const static float MoveSpeed = 0.001f;
        m_AngleOfView = (m_AngleOfView + glm::degrees(atan(tan(glm::radians(m_AngleOfView)) / (1.0f + MoveSpeed * Offset))))/2.0f;
//        std::cout << m_AngleOfView << std::endl;

        // 对角度进行约束
        if (m_AngleOfView >= 88.0f) m_AngleOfView = 88.0f;
        else if (m_AngleOfView <= 2.0f) m_AngleOfView = 2.0f;
        else
        {
            m_Position -= (float)Offset * MoveSpeed * m_Direction;
        }
        m_Projection = glm::perspective(glm::radians(m_AngleOfView), 1.0f, 0.1f, 100.0f);

        updateCameraVectors();
    }
}
