//
// Created by 邱泽鸿 on 2020/12/20.
//

#pragma once

#include "Basic.h"

class FrameBuffer
{
private:
    unsigned int m_RendererID;
    unsigned int m_TextureMap;
public:
    inline FrameBuffer(const unsigned int& TextureMap)
        : m_TextureMap(TextureMap)
    { // Frame buffer主要用于depth map的生成
        DebugCall(glGenFramebuffers(1, &m_RendererID)); // 分配一个buffer的内存
        // 把材质图绑定到Frame Buffer上
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_TextureMap, 0);

        // 当生成一个深度立方体贴图时我们只关心深度值
        // 所以我们必须显式告诉OpenGL这个帧缓冲对象不会渲染到一个颜色缓冲里。
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        bind();
        DebugCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
    }
    inline ~FrameBuffer()
    {
        DebugCall(glDeleteBuffers(1, &m_RendererID));
    }

    inline unsigned int getRendererID() const
    {
        return m_RendererID;
    }

    inline void bind() const
    {
        DebugCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_TextureMap, 0));
    }
    inline void unbind() const
    {
//        DebugCall(glFramebufferTexture(GL_ARRAY_BUFFER, 0));
    }
};
