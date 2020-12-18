//
// Created by 邱泽鸿 on 2020/12/13.
//

#pragma once

#include "Basic.h"

class IndexBuffer
{
private:
    unsigned int m_RendererID;
    unsigned int m_Count; // how many indices
public:
    // "size" means bytes, "count" means element counts
    IndexBuffer(const unsigned int* Data, unsigned int Count, const bool& isStatic)
        : m_Count(Count)
    { // isStatic表示数据是否不能通过bufferSubData重新写入vertex
        ASSERT(sizeof(unsigned int) == sizeof(GLuint));

        DebugCall(glGenBuffers(1, &m_RendererID));
        DebugCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
        DebugCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, Count * sizeof(unsigned int), Data, isStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW));
    }
    ~IndexBuffer()
    {
        DebugCall(glDeleteBuffers(1, &m_RendererID));
    }

    void bind() const
    {
        DebugCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
    }
    void unbind() const
    {
        DebugCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }

    inline unsigned int getCount() const
    {
        return m_Count;
    }
};