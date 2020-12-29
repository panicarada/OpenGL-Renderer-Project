//
// Created by 邱泽鸿 on 2020/12/29.
//

#include "SoftShadow.h"

// 注意！！！！长宽高要一样
const unsigned int ShadowSize =  (WINDOW_WIDTH << 1);


SoftShadow::SoftShadow(const std::shared_ptr<Shader> &shader)
{
    glEnable(GL_TEXTURE_3D);

    // 生成一个frame buffer
    glGenFramebuffers(1, &FBO);
    // 绑定frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // 生成Shadow map
    glGenTextures(1, &SoftShadowMap);
    // 将3D纹理绑定在Shadow map上
    glBindTexture(GL_TEXTURE_3D, SoftShadowMap);

    // 设置纹理的属性
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    /*
    glTexImage3D(GL_TEXTURE_3D, 0,
                 GL_DEPTH_COMPONENT, // 只需要深度值（即阴影值）
                 ShadowSize, ShadowSize, ShadowSize, 0, GL_DEPTH_COMPONENT,
                 GL_FLOAT, nullptr); // 只是分配空间，暂时没有数据
*/
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, ShadowSize, ShadowSize, ShadowSize, 0, GL_RGBA, GL_FLOAT, NULL);

    // 把Shadow map绑定到Frame Buffer上
//    glFramebufferTexture3D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_3D, SoftShadowMap, 0, 0);
    glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, SoftShadowMap, 0, 0);

    // 生成阴影写入Depth map时不需要绘制
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // 检查Frame Buffer的完整性
    std::cout << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    // 取消Frame Buffer的绑定
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SoftShadow::render(const std::set<std::shared_ptr<Geometry>> &GeometrySet,
                        const std::set<std::shared_ptr<Light>> &LightSet) const
{

}
