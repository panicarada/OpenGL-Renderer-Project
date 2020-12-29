//
// Created by 邱泽鸿 on 2020/12/20.
//

#pragma once
#include "Basic.h"
#include "Light.h"
#include "Shader.h"
#include "Geometry.h"
#include <array>

class SoftShadow
{
public:
    SoftShadow(const std::shared_ptr<Shader>& shader);
    void render(const std::set<std::shared_ptr<Geometry>>& GeometrySet,
                      const std::set<std::shared_ptr<Light>>& LightSet) const; // 渲染阴影3D贴图
    inline unsigned int getSoftShadowMap() const
    {
        return SoftShadowMap;
    }
private:
    unsigned int FBO;
    unsigned int SoftShadowMap; // 3D阴影贴图
    std::shared_ptr<Shader> m_Shader;
};