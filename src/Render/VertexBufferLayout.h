//
// Created by 邱泽鸿 on 2020/12/13.
//

#pragma once

#include "Basic.h"
#include <vector>

// 管理Vertex排布格式
struct VertexBufferElement
{
    unsigned int Type;
    unsigned int Count;
    unsigned char Normalized;
};


class VertexBufferLayout
{
private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride;
public:
    VertexBufferLayout()
            : m_Stride(0){}

    template<typename T>
    void Push(unsigned int Count)
    {

    }

    template<>
    void Push<float>(unsigned int Count)
    {
        m_Elements.push_back({GL_FLOAT, Count, GL_FALSE});
        m_Stride += Count * getSizeofType(GL_FLOAT);
    }

    template<>
    void Push<unsigned int>(unsigned int Count)
    {
        m_Elements.push_back({GL_UNSIGNED_INT, Count, GL_FALSE});
        m_Stride += Count * getSizeofType(GL_UNSIGNED_INT);
    }

    template<>
    void Push<unsigned char>(unsigned int Count)
    {
        m_Elements.push_back({GL_UNSIGNED_BYTE, Count, GL_TRUE});
        m_Stride += Count * getSizeofType(GL_UNSIGNED_BYTE);
    }

    inline const std::vector<VertexBufferElement>& getElements() const
    {
        return m_Elements;
    }

    inline unsigned int getStride() const
    { // 每一批数据的间隔
        return m_Stride;
    }
};


