//
// Created by Berkush on 18.03.2026.
//

#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION //Texture handling icin, tek bir headerdan olusuyor, calismasi icin bu define zorunluymus
#include "stb_image.h"

Texture::Texture(const std::string& filepath)
    : m_RendererID(0), m_FilePath(filepath), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
{
    stbi_set_flip_vertically_on_load(1); //OpenGL'in sol alti 0-0, PNG'lerin sag ustu 0-0; bu arada Texture formatina gore degisebilirmis bu.
    m_LocalBuffer = stbi_load(filepath.c_str(), &m_Width, &m_Height, &m_BPP, 4/*RGBA istiyoruz desired channel bu*/);

    GLCall(glGenTextures(1, &m_RendererID));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)); //Resample down icin bu, GL_LINEAR yumusak, GL_NEAREST keskin retro
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)); //Magnification
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)); //S horizontal, clamp ise genisletmek, genisletilsin istemiyoruz
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)); //T vertical //GL_CLAMP enum'u legacy imis, yeni versiyonda TO_EDGE kismi gerekliymis

    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8/*Bu OpenGL nasil saklayacak, GL_RGBA de calisir da depreched oldu diyor, kanal basina biti belirt diyor*/,m_Width, m_Height,
        0, GL_RGBA/*Bu da bizim OpenGL'e sagladimiz dosya*/, GL_UNSIGNED_BYTE, m_LocalBuffer/*0 ya da nullptr da verebilir misiz, Data'yi OpenGL'e payla;maya hazir degil ama yer acmak istiyorsak*/));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0)); //Unbind
    //GLCall(glGenerateMipmap(GL_TEXTURE_2D));

    if (m_LocalBuffer)
    {
        stbi_image_free(m_LocalBuffer);
    }
}

Texture::~Texture()
{
    GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot /*= 0*/) const
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::Unbind() const
{
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}