//
// Created by 邱泽鸿 on 2020/12/20.
//

#pragma once
#include "Basic.h"
#include "Light.h"
#include "Shader.h"
#include "Geometry.h"
#include <array>

class Shadow
{
public:
    Shadow(const std::shared_ptr<Shader>& shader);
    void renderShadow(const std::set<std::shared_ptr<Geometry>>& GeometrySet,
                      const std::set<std::shared_ptr<Light>>& LightSet); // 渲染深度图
    inline unsigned int getDepthMap() const
    {
        return DepthMap;
    }
    void setSamples(const std::shared_ptr<Shader>& sampledShader) const;
private:
    unsigned int FBO;
    unsigned int DepthMap; // 深度图
    std::shared_ptr<Shader> m_Shader;
};