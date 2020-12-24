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
    Shadow(const std::shared_ptr<Shader>& shader, int Width, int Height);
    void renderShadow(const std::set<std::shared_ptr<Geometry>>& GeometrySet,
                      const std::set<std::shared_ptr<Light>>& LightSet); // 渲染深度图
    inline void bind() const
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, FrameBuffer);
    }
    inline void unbind() const
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
    inline unsigned int getDepthMap() const
    {
        return DepthMap;
    }
private:
    unsigned int FrameBuffer;
    unsigned int DepthMap; // 深度图
    std::shared_ptr<Shader> m_Shader;
    int m_Width;
    int m_Height;
};