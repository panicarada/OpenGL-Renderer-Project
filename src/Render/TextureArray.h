//
// Created by 邱泽鸿 on 2020/12/13.
//

#pragma once

#include "Basic.h"
#include "Shader.h"
#include <set>
#include "stb_image.h" // 读取图片


class TextureArray
{
private:
    unsigned int m_RendererID;
    std::set<unsigned int> availableSlots; // 所有材质公共可用的slot编号池
    std::shared_ptr<Shader> m_Shader;

    // 一个TextureArray所有的图像大小是一样的
    int m_Width;
    int m_Height;
    int m_ImageNum;
public:
    TextureArray(std::shared_ptr<Shader> &shader, int ImageNum = 15, int Width = 500, int Height = 500);

    int addTexture(const std::string& path);
    bool eraseTexture(int SlotID);
    inline int getImageNum() const
    {
        return m_ImageNum;
    }

    ~TextureArray()
    {

    }

    void bind() const
    {
        DebugCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID));
    }
    void unbind() const
    {
        DebugCall(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
    }

    inline unsigned int getRendererID() const
    {
        return m_RendererID;
    }
};



