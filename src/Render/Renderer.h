//
// Created by 邱泽鸿 on 2020/12/13.
//

#pragma once

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Basic.h"


class Renderer
{
public:
    void draw(const VertexArray& va,
              const IndexBuffer& ib,
              const std::shared_ptr<Shader>& shader) const
    {
        shader->bind();
        va.bind();
        ib.bind();
        DebugCall(glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr));
    }
    void clear()
    {
        DebugCall(glClear(GL_COLOR_BUFFER_BIT));
    }
};

