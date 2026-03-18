//
// Created by Berkush on 17.03.2026.
//

#include "Renderer.h"

#include <iostream>

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR/*Bunun sifira esit olmasi olmasi garanti edilmis, OpenGL dokumantosyanlarinda*/);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

void Renderer::Clear() const
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::ClearColor(float r, float g, float b, float a) const
{
    GLCall(glClearColor(r, g, b, a));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    //Draw'dan once tekrarden bind olma
    shader.Bind();
    va.Bind();
    ib.Bind();
    //Draw'dan once tekrarden bind olma

    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount()/*Indices sayisi, Vertices degil!*/, GL_UNSIGNED_INT/*Burada cok patliyormus insanlar*/,
    nullptr /*Halihazirda ElementArrayBuffer'a bind'liyiz diye birsey koymadik.*/));
    /*
     *bunu IndexBuffer sinifina tasimama nedenimiz bir VertexBuffer icin birden fazla index buffer olabilirmis ->
     *Ornegin araba yaptik, metal ve cam ayri draw call gerektiriyor -> ayrica kendi Renderer sinifimizin bunu halletmesini saglicaz
    */


    //unbind yapmiyoruz, nasil olsa baska bir draw calldan once bir seye bind yapilacak diye
}



/*
 //Legacy OpenGL versiyonu, ucgen ciziyor
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.5f, -0.5f);
        glEnd();


//Basit bir draw call ornegi
//glDrawArrays(GL_TRIANGLES, 0, 3); //first offset, count ta indici, yani kac tane bu elemandan var demek
->Bu arada use shader kullanmadan bununla birlikte ekrana cizim yapip bir sey gorabiliyorsak; gpunun standart shaderi vardir demektir ve bu aslinda OpenGL standartlarina karsi bisi


//glDebugMessageCallback() -> Surekli (dongu icinde) hata var mi diye kontrol etmek yerine, OpenGL API'ine bind atiyoruz. OpenGL 4.3 ustu icin gecerli.
 */