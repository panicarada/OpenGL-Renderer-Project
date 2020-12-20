//
// Created by 邱泽鸿 on 2020/12/20.
//

#pragma once

#include "Geometry.h"
#include "Light.h"
class Scene
{
public:
    Scene();
    ~Scene() = default;

private:
    std::set<std::shared_ptr<Geometry>> m_GeometrySet; // 几何物体的集合
    std::shared_ptr<Geometry> selectedGeometry; // 当前选中物体
    std::shared_ptr<Geometry> Floor; // 地板
    std::set<std::shared_ptr<Light>> m_LightSet; // 光源
    std::shared_ptr<Light> selectedLight;

    std::shared_ptr<Shader> m_Shader;
    std::shared_ptr<Camera> m_Camera;
    glm::vec4 m_Color;
    glm::vec3 m_Position;
};