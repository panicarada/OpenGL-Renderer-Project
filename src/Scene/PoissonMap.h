//
// Created by 邱泽鸿 on 2020/12/29.
//

#pragma once
#include "Basic.h"
#include <vector>
#include <random>

// 利用Poisson分布生成噪声图，每个点在平面上
class PoissonMap
{
public:
    unsigned int genRandomMap(const int& SampleNum);
    inline void bind()
    {
        glBindTexture(GL_TEXTURE_1D, m_RendererID);
    }
private:
    unsigned int m_RendererID; // 纹理的ID

private:
    std::shared_ptr<glm::vec2> popRandom(std::vector<std::shared_ptr<glm::vec2>>& Points)
    { // 在列表中随机取出一个点
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0, Points.size());//uniform distribution between 0 and 1
        int RandomIndex = glm::floor(dis(gen));
        auto res = Points[RandomIndex];
        Points.erase(Points.begin() + RandomIndex);
        return res;
    }
    std::shared_ptr<glm::vec2> genRandomPointAround(const std::shared_ptr<glm::vec2>& Point, float Bound);

    class Grid
    {
    public:
        Grid(int Width, int Height, float CellSize)
        : m_Width(Width), m_Height(Height), m_CellSize(CellSize)
        {
            // 初始化分配空间
            m_Cells.resize(Height); // 有Height行
            for (auto it = m_Cells.begin(); it != m_Cells.end(); ++it)
            {  // 每行有Width列
                it->resize(m_Width);
            }
        }
        inline void insert(const std::shared_ptr<glm::vec2>& Point)
        { // 现在坐标是[0, 1] x [0, 1]，需要转换成对应位置
            int Row = (int)Point->y / m_CellSize;
            int Col = (int)Point->x / m_CellSize;
            m_Cells[Row][Col] = Point;
        }
        inline bool isNeighbor(const std::shared_ptr<glm::vec2>& Point, const float& MinGap) const
        { // 看看这个点加入后，会不会有两点距离小于MinGap
            int Row = (int)Point->y / m_CellSize;
            int Col = (int)Point->x / m_CellSize;
            const int Delta = 4; // 搜索范围
            for (int i = Row - Delta; i < Row + Delta; ++i)
            {
                for (int j = Col - Delta; j < Col + Delta; ++j)
                {
                    if (i >= 0 && m_Width && j >= 0 && j < m_Height)
                    {
                        auto Temp = m_Cells[i][j];
                        if (Temp && glm::distance(*Temp, *Point) < MinGap)
                        {
                            return true;
                        }
                    }
                }
            }
            return false;
        }
    public:
        std::vector<std::vector<std::shared_ptr<glm::vec2>>> m_Cells;
        int m_Width;
        int m_Height;
        float m_CellSize;
    };
    std::vector<std::shared_ptr<glm::vec2>>
    genPoissonPoints(int NumPoints, int SearchThreshold = 30, bool isCircle = true,
                                            float MinGap = -1.0f);
};
