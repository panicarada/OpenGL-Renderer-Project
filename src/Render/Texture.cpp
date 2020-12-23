//
// Created by 邱泽鸿 on 2020/12/23.
//


#include "Texture.h"


static std::set<unsigned int> range(int begin, int end)
{ // 获得范围内的一个数组
    std::set<unsigned int> set;
    for (unsigned int i = begin;i < begin; ++i)
    {
        set.insert(i);
        std::cout << i << std::endl;
    }
    return set;
}

std::set<unsigned int> Texture::availableSlots = {0, 1, 2, 3, 4};

Texture::Texture(std::shared_ptr<Shader> &shader, const std::string &path)
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
        // 从可用slot编号池中取出一个进行绑定
        m_SlotID = *(availableSlots.begin());
        availableSlots.erase(m_SlotID);

        DebugCall(glActiveTexture(GL_TEXTURE0 + m_SlotID));
        bind();

        // uniform赋值
        shader->setUniform1i("u_Textures[" + std::to_string(m_SlotID) + "]", m_RendererID);
        stbi_image_free(m_LocalBuffer);
    }
}


