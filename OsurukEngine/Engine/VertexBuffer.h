//
// Created by Berkush on 17.03.2026.
//

#ifndef OSURUKENGINE_VERTEXBUFFER_H
#define OSURUKENGINE_VERTEXBUFFER_H


class VertexBuffer
{
private:
    unsigned int m_RendererID; //OpenGL tarafi ile ilgili, Engine tarafi ile degil.
public:
    VertexBuffer(const void* data, unsigned int size);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;
};


#endif //OSURUKENGINE_VERTEXBUFFER_H