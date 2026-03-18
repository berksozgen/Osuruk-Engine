//
// Created by Berkush on 18.03.2026.
//

#ifndef OSURUKENGINE_TEXTURE_H
#define OSURUKENGINE_TEXTURE_H

#include "Renderer.h"


class Texture
{
private:
    unsigned int m_RendererID;
    std::string m_FilePath;
    unsigned char* m_LocalBuffer;
    int m_Width, m_Height, m_BPP;
public:
    Texture(const std::string& filepath);
    ~Texture();

    void Bind(unsigned int slot = 0) const; //Kabaca modern GPU'larda 32, Mobilde 8 slotumuz varmis ayni anda o kadar Texture kullanmak icin, bunu OpenGL'e sorabiliyormusuz.
    void Unbind() const;

    inline int GetWidth() const { return m_Width; }
    inline int GetHeight() const { return m_Height; }

};

#endif //OSURUKENGINE_TEXTURE_H
