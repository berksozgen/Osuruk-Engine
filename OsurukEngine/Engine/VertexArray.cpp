//
// Created by Berkush on 17.03.2026.
//

#include "VertexArray.h"

#include "Renderer.h"
#include "VertexBufferLayout.h"

VertexArray::VertexArray()
{
    GLCall(glGenVertexArrays(1, &m_RendererID)); //Bu VertexArrayObject olayini test edin diyor acikladim asagida, OpenGL ama boyle yapmayi oneriyormus, NVIDIA 31 diyormus falan.
}

VertexArray::~VertexArray()
{
    GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
    Bind();
    vb.Bind();

    const auto& elements = layout.GetElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); i++)
    {
        const auto& element = elements[i];

        //Bunlar VertexArrayBuffer ile vao'yu bagliyorlar.
        GLCall(glEnableVertexAttribArray(i));
        GLCall(glVertexAttribPointer(i,element.count,element.type, element.normalized,
            layout.GetStride(), (const void*)offset)); //index dedigi shader codedaki layout location kismi.
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
        /*
         * Eger ki sadece global bir vao tanimlama yonunde bir secim yaparsak (her VertexArrayimize ozgu atamak yerine), her draw calldan once (aslinda index buffer objecte bind olmadan; ->
         * aslinda ekrana yazdirmak icin index buffer objecte ihtiyacimiz yok, kullandigimiz duzenlemede oyle yapmayi sectiysek ona bind olmadan, her halukarda draw calldan once yapmamiz lazim bunu) ->
         * glBindBuffer(GL_ARRAY_BUFFER, vertexArrayimiz iste) bind olacak, glEnableVertexAttribArray enable yapcaz ve glVertexAttribPointer enable yapcaz falan filan fistan.
         * Bu arada eleman diyor ki -> "performans olarak hangisi daha iyi kendi durumunuzda test edin".
         */
        //Bunlar VertexArrayBuffer ile vao'yu bagliyorlar.
    }
}

void VertexArray::Bind() const
{
    GLCall(glBindVertexArray(m_RendererID)); //GLFW_OPENGL_CORE_PROFILE icin gerek var, COMPAT Profile'da bu isi kendi olusturuyormus ID 0 verince.

    //Eski kisim -> GLCall(glBindVertexArray(vao)); /*Bunu yapma nedenimiz glBindBuffer ile vertex array'e baglanma,
    ////glEnableVertexAttribArray ile glVertexAttribPointer tekrardan kullanmamiza gerek birakmamasi, her Vertex grubuna atrib atiyor*/
}



void VertexArray::Unbind() const
{
    GLCall(glBindVertexArray(0));
}
