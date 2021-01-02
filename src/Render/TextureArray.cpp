//
// Created by 邱泽鸿 on 2020/12/23.
//


#include "TextureArray.h"
#include "stb_image/stb_image_resize.h"

TextureArray::TextureArray(std::shared_ptr<Shader> &shader, int ImageNum, int Width, int Height)
    : m_Shader(shader), m_Width(Width), m_Height(Height), m_ImageNum(ImageNum)
{
    for (int i = 0;i < ImageNum; ++i)
    { // 初始化编号池
        availableSlots.insert(i);
    }
//    glActiveTexture(GL_TEXTURE0);
    DebugCall(glGenTextures(1, &m_RendererID)); // 获取一个buffer位置
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID); // 绑定纹理
    //设置如何从数据缓冲区去读取图像数据
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //设置纹理过滤的参数
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //  要加载500 * 500的图片15张，先分配空间
    //  所有图片大小都要一样
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, m_Width, m_Height, m_ImageNum, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

int TextureArray::addTexture(const std::string& path)
{
    int Width;
    int Height;
    int BPP;
    // image的左上角是(0, 0)，而OpenGL左下角是(0, 0)
    // 所以要上下翻转
    stbi_set_flip_vertically_on_load(1);
    /*
     * stbi_load：把路径的图像load到OpenGL的buffer中，在输入获得图像宽度、高度以及bytes per pixel
     *            desired_channels表示希望从图像得到的通道数，我们需要R, G, G, Alpha
     */
    auto tempBuffer = stbi_load(path.c_str(), &Width, &Height, &BPP, STBI_rgb_alpha);

    //  所有图片大小都要一样，所以需要resize
    unsigned char* LocalBuffer = (unsigned char*) malloc(m_Width * m_Height * STBI_rgb_alpha * sizeof(unsigned char));
    if (stbir_resize_uint8(tempBuffer, Width, Height, 0, LocalBuffer, m_Width, m_Height, 0, STBI_rgb_alpha)
        && !availableSlots.empty())
    { // 如果读取并且改大小成功，并且还有可用的slot编号
        bind(); // 绑定纹理
        //设置如何从数据缓冲区去读取图像数据
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // 从可用slot编号池中取出一个进行绑定
        int SlotID = *(availableSlots.begin());
        availableSlots.erase(SlotID);
        // 添加材质到Buffer中
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, SlotID, m_Width, m_Height, 1, GL_RGBA, GL_UNSIGNED_BYTE, LocalBuffer);
        free(LocalBuffer);
        return SlotID;
    }
    return -1;
}

bool TextureArray::eraseTexture(int SlotID)
{
    if (SlotID >= 0 && SlotID < m_ImageNum)
    { // 编号重新生效
        availableSlots.insert(SlotID);
        return true;
    }
    return false;
}


