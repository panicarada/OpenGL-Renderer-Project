//
// Created by 邱泽鸿 on 2020/12/20.
//

#include "Shadow.h"

Shadow::Shadow(std::shared_ptr<Light> light, const unsigned int &ShadowWidth, const unsigned int &ShadowHeight)
    : m_Light(light),
      ShadowProjection(glm::perspective(glm::radians(90.0f), // 光空间的投影矩阵，只需初始化一次，换成其他立方体的面时旋转即可
                (float)ShadowWidth/(float)ShadowHeight, 1.0f, 25.0f))
{
    m_Shader = std::make_shared<Shader>("../resource/Shadow.shader");
    m_Shader->setUniform1f("u_zFar", 25.0f);

    // 首先，创建一个立方体贴图
    glGenTextures(1, &m_RendererID);
    // 然后生成立方体贴图的每个面，将它们作为2D深度值纹理图像
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID); // 分配六个面的内存

    glActiveTexture(GL_TEXTURE0);

    for (unsigned int i = 0;i < 6; ++i)
    { // 立方体六个面都加上贴图
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                     ShadowWidth, ShadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // 绑定Frame Buffer
    m_FrameBuffer = std::make_unique<FrameBuffer>(m_RendererID);

    // 光空间的变换
    updateShadowData();
}

void Shadow::updateShadowData()
{
    // 对立方体六个面分别做一个投影矩阵，glm::lookAt的Up参数其实并不重要
    ShadowTransforms[0] = ShadowProjection * glm::lookAt(m_Light->m_Position,
                                                         m_Light->m_Position + glm::vec3(1.0f, 0.0f, 0.0f),
                                                         glm::vec3(0.0f, -1.0f, 0.0f));
    ShadowTransforms[1] = ShadowProjection * glm::lookAt(m_Light->m_Position,
                                                         m_Light->m_Position + glm::vec3(-1.0f, 0.0f, 0.0f),
                                                         glm::vec3(0.0f, -1.0f, 0.0f));
    ShadowTransforms[2] = ShadowProjection * glm::lookAt(m_Light->m_Position,
                                                         m_Light->m_Position + glm::vec3(0.0f, 1.0f, 0.0f),
                                                         glm::vec3(0.0f, 0.0f, 1.0f));
    ShadowTransforms[3] = ShadowProjection * glm::lookAt(m_Light->m_Position,
                                                         m_Light->m_Position + glm::vec3(0.0f, -1.0f, 0.0f),
                                                         glm::vec3(0.0f, 0.0f, -1.0f));
    ShadowTransforms[4] = ShadowProjection * glm::lookAt(m_Light->m_Position,
                                                         m_Light->m_Position + glm::vec3(0.0f, 0.0f, 1.0f),
                                                         glm::vec3(0.0f, -1.0f, 0.0f));
    ShadowTransforms[5] = ShadowProjection * glm::lookAt(m_Light->m_Position,
                                                         m_Light->m_Position + glm::vec3(1.0f, 0.0f, -1.0f),
                                                         glm::vec3(0.0f, -1.0f, 0.0f));
}

void Shadow::bind() const
{
    DebugCall(glBindFramebuffer(GL_TEXTURE_CUBE_MAP, m_RendererID));
}


void Shadow::unbind() const
{
    DebugCall(glBindFramebuffer(GL_TEXTURE_CUBE_MAP, 0));
}
