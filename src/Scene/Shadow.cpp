#include "Shadow.h"

Shadow::Shadow(const std::shared_ptr<Shader> &shader, int Width, int Height)
    : m_Shader(shader), m_Width(Width), m_Height(Height)
{
    // 生成frame buffer
    glGenFramebuffers(1, &FrameBuffer);
    // 绑定使用Frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);


    // 为阴影贴图分配空间
    glGenTextures(1, &DepthMap);
    glBindTexture(GL_TEXTURE_2D, DepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16,
                 Width, Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    // 把阴影贴图绑定在frame buffer上
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, DepthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    // 取消frame buffer绑定
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

void Shadow::renderShadow(const std::set<std::shared_ptr<Geometry>> &GeometrySet,
                          const std::set<std::shared_ptr<Light>> &LightSet)
{
    auto LightProjection = glm::ortho(-1.0f*m_Width, 1.0f*m_Width, -1.0f*m_Height, 1.0f*m_Height, ZNEAR, ZFAR);
    // 保存Viewport
    int SavedViewPort[4];
    glGetIntegerv(GL_VIEWPORT, SavedViewPort);

    // 换成阴影渲染的Viewport
    glViewport(0, 0, m_Width, m_Height);
    // 绑定Frame Buffer
    glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
    // 清除深度缓存
    glClear(GL_DEPTH_BUFFER_BIT);
//    glActiveTexture(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, DepthMap);
    for (auto light : LightSet)
    {
        auto LightView = glm::lookAt(light->m_Position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));


        auto LightSpaceMatrix = LightProjection * LightView;
//        auto LightSpaceMatrix = glm::mat4(1.0f);
        m_Shader->bind();
        m_Shader->setUniformMat4f("u_LightSpaceMatrix", LightSpaceMatrix);
        for (auto geometry : GeometrySet)
        {
            // 暂时绑定阴影的Shader
            auto Temp = geometry->m_Shader;
            Temp->setUniformMat4f("u_LightSpaceMatrix", LightSpaceMatrix);
//            Temp->setUniformMat4f("u_LightSpaceMatrix", glm::mat4(1.0f));


            geometry->m_Shader = m_Shader;
            geometry->draw();
            // 恢复原状态
            geometry->m_Shader = Temp;
        }
    }
    // 解除Frame Buffer绑定
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 恢复Viewport
    glViewport(SavedViewPort[0], SavedViewPort[1], SavedViewPort[2], SavedViewPort[3]);
}
