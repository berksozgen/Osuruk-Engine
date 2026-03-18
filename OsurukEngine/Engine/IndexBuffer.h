//
// Created by Berkush on 17.03.2026.
//

#ifndef OSURUKENGINE_INDEXBUFFER_H
#define OSURUKENGINE_INDEXBUFFER_H


class IndexBuffer
{
private:
    unsigned int m_RendererID; //OpenGL tarafi ile ilgili, Engine tarafi ile degil.
    unsigned int m_Count; //ne kadar indicimiz olmasini bilmemiz lazim bunun icin vertexin aksine
public:
    IndexBuffer(const unsigned int* data, unsigned int count); //shortda kullanabiliriz -> Bu arada size yaziyorsak Byte cinsinden, count yaziyorsak adet cinsinden isimlendirme olarak
    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;

    inline unsigned int GetCount() const { return m_Count; }
};


#endif //OSURUKENGINE_INDEXBUFFER_H