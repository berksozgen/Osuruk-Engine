//
// Created by Berkush on 17.03.2026.
//

#ifndef OSURUKENGINE_VERTEXARRAY_H
#define OSURUKENGINE_VERTEXARRAY_H

#include "VertexBuffer.h"

class VertexBufferLayout;

class VertexArray
{
private:
    unsigned int m_RendererID;

public:
    VertexArray();
    ~VertexArray();

    void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

    void Bind() const;
    void Unbind() const;
};


#endif //OSURUKENGINE_VERTEXARRAY_H