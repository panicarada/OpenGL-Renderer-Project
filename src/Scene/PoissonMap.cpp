//fujin
// Created by 邱泽鸿 on 2020/12/29.
//

#include "PoissonMap.h"

/* 生成[0, 1] x [0, 1]范围内的随机点
 * @param:
 *      NumPoints: 预期采样点数目
 *      SearchThreshold: 在已经求得的采样点附近搜索新的采样点，搜索次数的阈值，即文章中的k
 *      isCircle: 采样点在圆形内还是在方形内
 *      MinGap: 相邻采样点之间最小的距离
 */
std::vector<std::shared_ptr<glm::vec2>>
PoissonMap::genPoissonPoints(int NumPoints, int SearchThreshold, bool isCircle, float MinGap)
{
    /* 随机数产生器 */
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double>dis(-0.5, 0.5);

    if (MinGap < 0.0f)
    {
        MinGap = 1.0f / glm::sqrt(float(NumPoints));
    }
    std::vector<std::shared_ptr<glm::vec2>> SamplePoints; // 真正返回的采样点
    std::vector<std::shared_ptr<glm::vec2>> ProcessList; // push的都是采样点，但是会不断弹出，找到采样点附近的点进行搜索

    float CellSize = MinGap / glm::sqrt(2.0f);

    // 矩形被分为方格，每个方格的宽度是CellSize
    int GridWidth;
    int GridHeight = GridWidth = glm::ceil(1.0f / CellSize);
    Grid grid(GridWidth, GridHeight, CellSize); // 随机点网格
    std::shared_ptr<glm::vec2> Start; // 迭代开始的随机点
    do
    {
        Start = std::make_shared<glm::vec2>(dis(gen)+0.5, dis(gen)+0.5);
        if (isCircle)
        { // 此时需要在以(0.5, 0.5)为圆心，0.5为半径的圆内
            if (glm::distance(*Start, glm::vec2(0.5f)) <= 0.5f)
            {
                break;
            }
        }
        else
        { // 此时生成的点一定在区域内
            break;
        }
    }
    while (true);

    // 初始化
    ProcessList.push_back(Start);
    SamplePoints.push_back(Start);
    grid.insert(Start);

    // 产生随机点
    while (!ProcessList.empty() && SamplePoints.size() < NumPoints)
    {
        // 在已经取出的点中随机取出一点，搜索它周围的点
        auto Point = popRandom(ProcessList);
        for (int i = 0;i < SearchThreshold; ++i)
        { // 搜索附近的点
            auto NewPoint = genRandomPointAround(Point, MinGap);
            if (isCircle)
            { // 此时需要在以(0.5, 0.5)为圆心，0.5为半径的圆内
                if (glm::distance(*Point, glm::vec2(0.5f)) > 0.5f)
                {
                    continue;
                }
            }
            else
            { // 此时需要在[0, 1] x [0, 1]的方形内
                if (Point->x < 0.0f || Point->x > 1.0f || Point->y < 0.0f || Point->y > 1.0f)
                {
                    continue;
                }
            }
            if (!grid.isNeighbor(NewPoint, MinGap))
            { // 是否是某点的邻居
                ProcessList.push_back(NewPoint);
                SamplePoints.push_back(NewPoint);
                grid.insert(NewPoint);
                continue; // 找到了新的点，重新搜索
            }
        }
    }
    return SamplePoints;
}

// 在Point附近（圆环邻域）随机取出一点
std::shared_ptr<glm::vec2> PoissonMap::genRandomPointAround(const std::shared_ptr<glm::vec2>& Point, float Bound)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(-M_PI, M_PI);
    float Radius = Bound + Bound * (dis(gen) + M_PI) / (2.0 * M_PI);
    float Angle = dis(gen);
    auto res = std::make_shared<glm::vec2>(Point->x + Radius * cos(Angle), Point->y + Radius * sin(Angle));
    return res;
}

unsigned int PoissonMap::genRandomMap(const int &SampleNum)
{
    // 生成纹理
    glGenTextures(1, &m_RendererID);
    auto Points = genPoissonPoints(SampleNum << 1);

    unsigned int Attempts = 0; // 尝试生成Random Map的此时
    #define MAX_ATTEMPTS (100)

    while (++Attempts < MAX_ATTEMPTS && Points.size() < SampleNum)
    { // 生成的采样点数目不符合要求，重新生成
        Points = genPoissonPoints(SampleNum<<1);
    }
    std::cout << Points.size() << std::endl;
    if (Attempts == MAX_ATTEMPTS)
    {
        std::cout << "Fail to generate Poisson Random Map with " << SampleNum << " sample points" << std::endl;
    }
    std::vector<float> Data(Points.size());
    for (int i = 0, j = 0; i < Points.size()/2; ++i, j += 2)
    {
        Data[j] = Points[i]->x;
        Data[j + 1] = Points[i]->y;
    }

    // 绑定纹理
    glBindTexture(GL_TEXTURE_1D, m_RendererID);
    // 分配空间，并写入纹理
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RG, Points.size(), 0, GL_RG, GL_FLOAT, &Data[0]);

    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return m_RendererID;
}