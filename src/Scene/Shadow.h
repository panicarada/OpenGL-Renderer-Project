//
// Created by 邱泽鸿 on 2020/12/20.
//

#pragma once
#include "Basic.h"
#include "Light.h"
#include "Shader.h"
#include "FrameBuffer.h"
#include <array>

class Shadow
{
public:
    void updateShadowData(); // 光源位置发生变化后，需要更新
private:
    const glm::mat4 ShadowProjection;
    unsigned int m_RendererID;
public:
    Shadow(std::shared_ptr<Light> light, const unsigned int& ShadowWidth = 1024, const unsigned int& ShadowHeight = 1024);
    std::shared_ptr<Light> m_Light;
    void bind() const;
    void unbind() const;

private:
    inline void setUniformMat4fv()
    {
        for (unsigned int i = 0;i < 6; ++i)
        {
            m_Shader->setUniformMat4f("u_ShadowMatrices["+std::to_string(i)+"]", ShadowTransforms[i]);
        }
        m_Shader->setUniform3f("u_LightPosition", m_Light->m_Position[0], m_Light->m_Position[1], m_Light->m_Position[2]);
    }

private:
    std::shared_ptr<Shader> m_Shader;
    std::unique_ptr<FrameBuffer> m_FrameBuffer;
    std::array<glm::mat4, 6> ShadowTransforms; // 光空间的变换矩阵，对应于六个面
};
