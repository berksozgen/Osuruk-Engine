//
// Created by Berkush on 17.03.2026.
//

#ifndef OSURUKENGINE_RENDERER_H //pragma once ile ayni mantik
#define OSURUKENGINE_RENDERER_H

#include <GL/glew.h>

#include "IndexBuffer.h"
#include "Shader.h"
#include "VertexArray.h"

#define ASSERT(x) if(!(x)) __debugbreak(); //__debugbreak compilera a ozguymus -> burada da calisiyor bu arada
#define GLCall(x) GLClearError();\
                  x;\
                  ASSERT(GLLogCall(#x, __FILE__, __LINE__))//__FILE__ ve __LINE__ platforma ozel degilmis, __debugbreak() cagrisinin aksine
//Bu arada adam her OpenGL fonksiyonunu bu makroyla sardi, asiri luzumu var mi bilmiyorum ne kadar performans etkiliyor

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer //Statik veya Singleton yapanlar da varmis, tartismaya acik diyo adam, 1 den fazla pencere acabilirsek tatli olur ama.
{
public:
    void Clear() const;
    void ClearColor(float r, float g, float b, float a) const;
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
};

#endif //OSURUKENGINE_RENDERER_H