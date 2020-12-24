#include "Shadow.h"

Shadow::Shadow(const std::shared_ptr<Shader> &shader, int Width, int Height)
    : m_Shader(shader), m_Width(Width), m_Height(Height)
{
    glGenFramebuffers(1, &FrameBuffer);
    glGenTextures(1, &DepthMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, DepthMap); // 为DepthMap分配包围盒六个面的空间
    for (unsigned int i = 0;i < 6; ++i)
    {
        // 为深度图包围盒每个面分配空间，nullptr表示数据待定，只是分配空间
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, Width, Height, 0,
                     GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }

    // 设置属性，routine而已
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // 把DepthMap作为Frame Buffer的颜色缓冲
    bind(); // 将包围盒六个面的深度图绑定在Frame Buffer上
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, DepthMap, 0);
    // 检查FrameBuffer是否绑定完成
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    // 取消Frame Buffer的绑定
    unbind();
}

void Shadow::renderShadow(const std::set<std::shared_ptr<Geometry>> &GeometrySet,
                          const std::set<std::shared_ptr<Light>> &LightSet)
{
//    glViewport(0, 0, m_Width, m_Height);
    this->bind(); // 绑定Frame Buffer
    glClear(GL_DEPTH_BUFFER_BIT);
    m_Shader->bind();
    m_Shader->setUniform1f("u_zFar", ZFAR);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    std::set<int> LightIDs;
    for (int i = 0;i < MAX_LIGHT_NUM; ++i) LightIDs.insert(i);
    for (auto light : LightSet)
    { // 设置光源
        m_Shader->setUniform3f("Lights[" + std::to_string(light->m_ID) + "].Position", light->m_Position);
        m_Shader->setUniform1i("Lights[" + std::to_string(light->m_ID) + "].isOpen", 1);
        // 这个光源是开启的，我们把它从集合取出
        LightIDs.erase(light->m_ID);
    }
    // LightIDs中剩余的光源都是关闭的
    for (int offLightID : LightIDs)
    {
        m_Shader->setUniform1i("Lights[" + std::to_string(offLightID) + "].isOpen", 0);
    }

    for (auto light : LightSet)
    { // 对每个光源都做一次深度图，最终效果叠加
        glClear(GL_DEPTH_BUFFER_BIT);
        // 渲染深度图时的Projection Matrix，90度保证整个面都被渲染到
        glm::mat4 ShadowProj = glm::perspective(glm::radians(90.0f), WINDOW_RATIO, 0.1f, 100.0f);
        std::array<glm::mat4, 6> ShadowMatrices; // 六个面的Model Matrix
        ShadowMatrices[0] = ShadowProj * glm::lookAt(light->m_Position, light->m_Position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        ShadowMatrices[1] = ShadowProj * glm::lookAt(light->m_Position, light->m_Position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        ShadowMatrices[2] = ShadowProj * glm::lookAt(light->m_Position, light->m_Position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ShadowMatrices[3] = ShadowProj * glm::lookAt(light->m_Position, light->m_Position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        ShadowMatrices[4] = ShadowProj * glm::lookAt(light->m_Position, light->m_Position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        ShadowMatrices[5] = ShadowProj * glm::lookAt(light->m_Position, light->m_Position + glm::vec3(.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

        for (int i = 0;i < 6; ++i)
        {
            m_Shader->setUniformMat4f("u_ShadowMatrices[" + std::to_string(i) + "]", ShadowMatrices[i]);
        }
        // 绘制深度图
        for (auto geometry : GeometrySet)
        { // 不能clear buffer，因为我们希望深度图叠加起来
            auto OriginalShader = geometry->m_Shader; // 先与原来的shader解绑
            geometry->m_Shader = m_Shader;
            geometry->draw();
            geometry->m_Shader = OriginalShader; // 重回绑定
        }
    }
    // Frame Buffer写完毕，取消绑定
    this->unbind();
    // 恢复正常视图
//    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    glDrawBuffer(GL_FRONT);
    glReadBuffer(GL_FRONT);
    m_Shader->unbind();
}
