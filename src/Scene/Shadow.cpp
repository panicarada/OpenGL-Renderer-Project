#include "Shadow.h"

#include <random> // 随机采样


// 注意！！！！长宽要一样
const unsigned int ShadowWidth =  (WINDOW_WIDTH << 1);
const unsigned int ShadowHeight = (WINDOW_WIDTH << 1);

Shadow::Shadow(const std::shared_ptr<Shader> &shader)
        : m_Shader(shader)
{
    // 生成一个frame buffer
    glGenFramebuffers(1, &FBO);
    // 生成depth map
    glGenTextures(1, &DepthMap);
    // 将立方体纹理绑定在depth map上
    glBindTexture(GL_TEXTURE_CUBE_MAP, DepthMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // 为Depth map每个面分配一个图像纹理的空间
        // 类型GL_DEPTH_COMPONENT表明只需要写入深度值
        // 千万注意，这个Width和Height要相等
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, ShadowWidth, ShadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                     nullptr);
        // nullptr表示先分配空间，还没写入
    }

    // 设置纹理的属性
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // 绑定Frame Buffer
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    // 把Depth map绑定到Frame buffer上
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, DepthMap, 0);
    // 生成阴影写入Depth map时不需要绘制
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // 检查Frame Buffer的完整性
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    // 取消Frame Buffer的绑定
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Shadow::render(const std::set<std::shared_ptr<Geometry>> &GeometrySet,
                          const std::set<std::shared_ptr<Light>> &LightSet)
{
    auto LightProjection = glm::perspective(glm::radians(90.0f), WINDOW_RATIO, ZNEAR, ZFAR);

    // 保存Viewport
    int SavedViewPort[4];
    glGetIntegerv(GL_VIEWPORT, SavedViewPort);

    // 换成阴影渲染的Viewport
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, ShadowWidth, ShadowHeight);
    // 绑定Frame Buffer
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    // 清除深度缓存
    glClear(GL_DEPTH_BUFFER_BIT);
    m_Shader->bind();
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    // 设置偏移，减少阴影计算时的失真
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.5f, 20.0f);

    m_Shader->setUniform1f("u_zFar", ZFAR);

    glm::mat4 LightProj = glm::perspective(glm::radians(90.0f), WINDOW_RATIO, ZNEAR, ZFAR);
    for (auto light : LightSet)
    {
        // 六个面的光空间矩阵（以光为视点的Project Matrix * View Matrix）
        std::array<glm::mat4, 6> LightSpaceMatrices;
        // view matrix依次是光源向立方体六个面观察
        // 不用for循环，减少流水线的stall
        LightSpaceMatrices[0] = (LightProj * glm::lookAt(light->m_Position, light->m_Position + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
        m_Shader->setUniformMat4f("u_LightSpaceMatrices[0]", LightSpaceMatrices[0]);
        LightSpaceMatrices[1] = (LightProj * glm::lookAt(light->m_Position, light->m_Position + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
        m_Shader->setUniformMat4f("u_LightSpaceMatrices[1]", LightSpaceMatrices[1]);
        LightSpaceMatrices[2] = (LightProj * glm::lookAt(light->m_Position, light->m_Position + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)));
        m_Shader->setUniformMat4f("u_LightSpaceMatrices[2]", LightSpaceMatrices[2]);
        LightSpaceMatrices[3] = (LightProj * glm::lookAt(light->m_Position, light->m_Position + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)));
        m_Shader->setUniformMat4f("u_LightSpaceMatrices[3]", LightSpaceMatrices[3]);
        LightSpaceMatrices[4] = (LightProj * glm::lookAt(light->m_Position, light->m_Position + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
        m_Shader->setUniformMat4f("u_LightSpaceMatrices[4]", LightSpaceMatrices[4]);
        LightSpaceMatrices[5] = (LightProj * glm::lookAt(light->m_Position, light->m_Position + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
        m_Shader->setUniformMat4f("u_LightSpaceMatrices[5]", LightSpaceMatrices[5]);

        m_Shader->setUniform3f("u_LightPosition", light->m_Position);

        for (auto geometry : GeometrySet)
        {
            // 暂时切换Shader，用来渲染阴影
            auto Temp = geometry->m_Shader;
            geometry->m_Shader = m_Shader;
            geometry->draw();
            geometry->m_Shader = Temp;
        }
    }

    // 解除Frame Buffer绑定
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // 关闭偏移
    glDisable(GL_POLYGON_OFFSET_FILL);
    // 恢复Viewport
    glViewport(SavedViewPort[0], SavedViewPort[1], SavedViewPort[2], SavedViewPort[3]);
}

// 为被采样的shader设置uniform量，用于阴影滤波的采样
void Shadow::setSamples(const std::shared_ptr<Shader> &sampledShader)
{
    const int sampleRange = 1; // 总采样数目是(2 * sampleRange + 1)^3
    float ImportanceSum = 0;
    int Counter = 0;
    const float Factor = glm::exp(M_PI/2.0f)/2.0f;
    const float EulerConstant = 0.577215664901532;
    // 随机数
    std::random_device rd;
    std::mt19937 gen(rd());
    // 正态分布
    std::normal_distribution<> dis(-M_PI * 2.0f, M_PI * 2.0f);
    for (int i = -sampleRange; i <= sampleRange; ++ i)
    {
        for (int j = -sampleRange; j <= sampleRange; ++j)
        {
            for (int k = -sampleRange; k <= sampleRange; ++k)
            {
                // 采样点在一个立方格内
                auto Point = glm::vec3(dis(gen), dis(gen), dis(gen));
//                std::cout << "( " << Point[0] << ", " << Point[1] << ", " << Point[2] << ")" << std::endl;
                sampledShader->setUniform3f("u_SampledPoints[" + std::to_string(Counter) + "]", Point);

                // 离得越远，重要度越低
                float Importance = 1.0f / (glm::pow(glm::length(Point),
                                                    glm::exp(EulerConstant + dis(gen)/20.0)) + Factor);
                sampledShader->setUniform1f("u_SampleImportance[" + std::to_string(Counter) + "]", Importance);
                ImportanceSum += Importance;
                Counter ++;
            }
        }
    }
//    std::cout << ImportanceSum << std::endl;
    sampledShader->setUniform1f("u_SampleImportanceSum", ImportanceSum);
}
