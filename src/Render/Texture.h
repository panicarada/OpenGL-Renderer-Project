//
// Created by 邱泽鸿 on 2020/12/13.
//

#pragma once

#include "Basic.h"
#include "Shader.h"
#include <set>
#include "stb_image.h" // 读取图片


class Texture
{
private:
    unsigned int m_RendererID;
    unsigned char* m_LocalBuffer;
    int m_Width, m_Height, m_BPP; // BPP: bytes per pixel
    unsigned int m_SlotID; // 这个材质占用的slot编号
    static std::set<unsigned int> availableSlots; // 所有材质公共可用的slot编号池
public:
    inline unsigned int getSlotID() const
    {
        return m_SlotID;
    }
    Texture(std::shared_ptr<Shader> &shader, const std::string& path);

    ~Texture()
    {
        // 释放自己的slotID
        availableSlots.insert(m_SlotID);
        DebugCall(glDeleteTextures(1, &m_RendererID));
    }

    void bind() const
    {
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



