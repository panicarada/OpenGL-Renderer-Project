//
// Created by 邱泽鸿 on 2020/12/13.
//

#pragma once

#include "Basic.h"


class Texture
{
private:
    unsigned int m_RendererID;
    unsigned char* m_LocalBuffer;
    int m_Width, m_Height, m_BPP; // BPP: bytes per pixel
public:
    inline Texture(const std::string& path)
    {
        // image的左上角是(0, 0)，而OpenGL左下角是(0, 0)
        // 所以要上下翻转
        stbi_set_flip_vertically_on_load(1);
        /*
         * stbi_load：把路径的图像load到OpenGL的buffer中，在输入获得图像宽度、高度以及bytes per pixel
         *            desired_channels表示希望从图像得到的通道数，我们需要R, G, G, Alpha
         */
        m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

        DebugCall(glGenTextures(1, &m_RendererID)); // 获取一个texture
        DebugCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

        DebugCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        DebugCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        DebugCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        DebugCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

        // 把buffer发送给OpenGL
        DebugCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
        DebugCall(glBindTexture(GL_TEXTURE_2D, 0));

        if (m_LocalBuffer)
        { // 如果绑定成功
            stbi_image_free(m_LocalBuffer);
        }
    }
    ~Texture()
    {
        DebugCall(glDeleteTextures(1, &m_RendererID));
    }

    void bind(unsigned int slot = 0) const
    {
        // 激活slot位置的texture
        DebugCall(glActiveTexture(GL_TEXTURE0 + slot));
        DebugCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
    }
    void unbind() const
    {
        DebugCall(glBindTexture(GL_TEXTURE_2D, 0));
    }

    inline unsigned int getRendererID() const
    {
        return m_RendererID;
    }
    inline int getWidth() const { return m_Width; }
    inline int getHeight() const { return m_Height; }
};