//
// Created by 邱泽鸿 on 2020/12/13.
//

#pragma once
#include "Basic.h"

class VertexBuffer
{
private:
    unsigned int m_RendererID;
public:
    inline VertexBuffer(const void* Data, unsigned  int Size, const bool &isStatic)
    {
        DebugCall(glGenBuffers(1, &m_RendererID)); // 分配一个buffer的内存
        DebugCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
        DebugCall(glBufferData(GL_ARRAY_BUFFER, Size, Data, isStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW));
    }
    inline ~VertexBuffer()
    {
        DebugCall(glDeleteBuffers(1, &m_RendererID));
    }

    inline unsigned int getRendererID() const
    {
        return m_RendererID;
    }

    inline void bind() const
    {
        DebugCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
    }
    inline void unbind() const
    {
        DebugCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }
};