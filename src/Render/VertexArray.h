//
// Created by 邱泽鸿 on 2020/12/13.
//

#pragma once

#include "Basic.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray
{
private:
    unsigned int m_RendererID;
public:
    inline VertexArray()
    {
        DebugCall(glGenVertexArrays(1, &m_RendererID));
    }
    inline ~VertexArray()
    {
        DebugCall(glDeleteVertexArrays(1, &m_RendererID));
    }

    void addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
    {
        bind();
        vb.bind();
        const auto& elements = layout.getElements();
        unsigned int offset = 0;
        for (unsigned int i = 0; i < elements.size(); ++i)
        {
            const auto& element = elements[i];
            DebugCall(glEnableVertexAttribArray(i));
            // 下面这条指令把buffer和目前绑定的vao进行绑定
            DebugCall(glVertexAttribPointer(i, element.Count, element.Type,
                                         element.Normalized, layout.getStride(), (void*)(uintptr_t)offset)); // uint --> uint pointer --> void pointer

            offset += element.Count * getSizeofType(element.Type);
        }
    }

    void bind() const
    {
        DebugCall(glBindVertexArray(m_RendererID));
    }
    void unbind() const
    {
        DebugCall(glBindVertexArray(0));
    }
};