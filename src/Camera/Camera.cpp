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
    if (!isOrbit)
    {
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
}

Camera::Camera(const float& AngleOfView, const glm::mat4 &Projection, const glm::vec3 &Position, const glm::vec3 &InitUp,
               const glm::vec3 &InitDirection, const float &Yaw, const float &Pitch)
   : isFPS(false), m_Projection(Projection), m_Tag("Perspective"), m_Position(Position), m_InitDirection(glm::normalize(InitDirection)),
     m_Yaw(Yaw), m_Pitch(Pitch), m_AngleOfView(AngleOfView), InitAngleOfView(AngleOfView), isOrbit(false)
{
    m_Right = m_InitRight = glm::normalize(glm::cross(InitDirection, InitUp));
    m_Up = m_InitUp = glm::normalize(glm::cross(m_InitRight, InitDirection));
    m_Direction = glm::normalize(InitDirection);

    updateCameraVectors();
}

void Camera::updateCameraVectors()
{
    if (!isOrbit)
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
    else
    {

    }
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

void Camera::OnScrollAction(const glm::vec2&& Offset)
{
    if (std::abs(Offset.x) > 0.1 && m_Tag == "Perspective")
    { // 敏感度过滤
        if (isOrbit)
        {
            const static float MoveSpeed = 0.01f;
            float deltaTheta = MoveSpeed * Offset.x;
            // 绕轨道移动
            glm::vec3 Position;
            glm::vec3 relPos = m_Position - TargetPosition;
            Position.x = relPos.x * glm::cos(deltaTheta) -relPos.z * sin(deltaTheta);
            Position.y = relPos.y;
            Position.z = relPos.z * cos(deltaTheta) + relPos.x * sin(deltaTheta);
            m_Position = Position + TargetPosition;

            m_ViewMatrix = glm::lookAt(m_Position, TargetPosition, glm::vec3(0.0f, 1.0f, 0.0f));
            // 偏航角改变
//            m_Yaw += deltaTheta;
//            updateCameraVectors();
        }
    }

    /* zoom */
    if (std::abs(Offset.y) > 0.1 && m_Tag == "Perspective")
    { // 敏感度过滤
        const static float MoveSpeed = 0.001f;
        m_AngleOfView = (m_AngleOfView + glm::degrees(atan(tan(glm::radians(m_AngleOfView)) / (1.0f + MoveSpeed * Offset.y))))/2.0f;
//        std::cout << m_AngleOfView << std::endl;

        // 对角度进行约束
        if (m_AngleOfView >= 88.0f) m_AngleOfView = 88.0f;
        else if (m_AngleOfView <= 2.0f) m_AngleOfView = 2.0f;
        else
        {
            m_Position -= (float)Offset.y * MoveSpeed * m_Direction;
        }
        m_Projection = glm::perspective(glm::radians(m_AngleOfView), WINDOW_RATIO, ZNEAR, ZFAR);

        updateCameraVectors();
    }
}

void Camera::save(std::ostream& Out) const
{
    Out << "Projection: ";
    for (int i = 0;i < 4; ++i)
    {
        for (int j = 0;j < 4; ++j)
        {
            Out << m_Projection[i][j] << " ";
        }
    }
    Out << std::endl;
    Out << "Tag: " << m_Tag << std::endl;
    Out << "Position: " << m_Position[0] << " " << m_Position[1] << " " << m_Position[2] << std::endl;
    Out << "InitDirection: " << m_InitDirection[0] << " " << m_InitDirection[1] << " " << m_InitDirection[2] << std::endl;
    Out << "InitRight: " << m_InitRight[0] << " " << m_InitRight[1] << " " << m_InitRight[2] << std::endl;
    Out << "InitUp: " << m_InitUp[0] << " " << m_InitUp[1] << " " << m_InitUp[2] << std::endl;

    Out << "Rotation: " << m_Yaw << " " << m_Pitch << std::endl;
    Out << "AngleOfView: " << m_AngleOfView << std::endl;
    Out << "InitAngleOfView: " << InitAngleOfView << std::endl;

    Out << "END_CAMERA" << std::endl;
}

void Camera::load(std::ifstream &In)
{
    std::string Line;
    while (std::getline(In, Line))
    {
        if (Line.length() == 0) continue;
        if (Line == "END_CAMERA") break;
        std::stringstream ss;
        ss << Line;
        std::string Property = "";
        ss >> Property;
        if (Property == "Projection:")
        {
            for (int i = 0;i < 4; ++i)
            {
                for (int j = 0;j < 4; ++j)
                {
                    ss >> m_Projection[i][j];
                }
            }
        }
        else if (Property == "Tag:") ss >> m_Tag;
        else if (Property == "Position:") ss >> m_Position[0] >> m_Position[1] >> m_Position[2];
        else if (Property == "InitDirection:") ss >> m_InitDirection[0] >> m_InitDirection[1] >> m_InitDirection[2];
        else if (Property == "InitRight:") ss >> m_InitRight[0] >> m_InitRight[1] >> m_InitRight[2];
        else if (Property == "InitUp:") ss >> m_InitUp[0] >> m_InitUp[1] >> m_InitUp[2];
        else if (Property == "Rotation:") ss >> m_Yaw >> m_Pitch;
        else if (Property == "AngleOfView:") ss >> m_AngleOfView;
        else if (Property == "InitAngleOfView:") ss >> InitAngleOfView;
    }
    updateCameraVectors();
}

void Camera::OnKeyAction(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_SPACE)
        { // 相机漫游状态
            isFPS = !isFPS;
            if (isFPS)
            { // 禁用鼠标
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            else // 开启鼠标
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }
        if (key == GLFW_KEY_TAB)
        { // zooming状态恢复
            resetZooming();
        }
        if (key == GLFW_KEY_O && mods == GLFW_MOD_SHIFT)
        { // shift + O切换Orbit模式
            isOrbit = !isOrbit;
        }
    }
}
