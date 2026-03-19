// Created by Berkush on 19.03.2026.

#include "TestTexture2D.h"

#include "CommonValues.h"
#include "Renderer.h" //->Bu glewi cagiriyor zaten
#include "imgui.h"

#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"

namespace Test //#TODO adam bu test icin yaptigim organizasyonun test icin onceki mainimizden alip burada olusturdu, ayni yapiyi MyFirstTest'e de yapilandirsam iyi olur
{
    TestTexture2D::TestTexture2D()
        : m_TranslationA(-1.0f, -1.0f, 0.0f), m_TranslationB(1.0f, 1.0f, 0.0f),
        m_Proj(glm::ortho(-widthToHeightRatio, widthToHeightRatio, -1.0f, 1.0f, -1.0f, 1.0f)),
        m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)))
    {
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        float positions[] =
        {
            //x      y  -  u     v
            -0.5f, -0.5f, 0.0f, 0.0f, //0
             0.5f, -0.5f, 1.0f, 0.0f, //1
             0.5f,  0.5f, 1.0f, 1.0f, //2
            -0.5f,  0.5f, 0.0f, 1.0f  //3
        };
        m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float)); //bunu unique ptr yapma nedenimiz, scopetan cikinci gpudan da veriyi silsin diye ayarlamistik
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);

        m_VAO = std::make_unique<VertexArray>();
        m_VAO->AddBuffer(*m_VertexBuffer, layout);

        unsigned int indices[] = //Bunu int yapmak zorunda degiliz char falan da yapabiliyor, unsigned olmali ama
        {
            0, 1, 2,
            2, 3, 0
        };
        m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);

        m_Shader = std::make_unique<Shader>(ASSET_DIR "/Shaders/Basic.shader");
        m_Shader->Bind();
        m_Shader->SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);
        m_Texture = std::make_unique<Texture>(ASSET_DIR "/Textures/berkush_256.png");
        m_Texture_Brick = std::make_unique<Texture>(ASSET_DIR "/Textures/brick.png");
        m_Shader->SetUniform1i("u_Texture", 0);
    }

    TestTexture2D::~TestTexture2D()
    {
        //unique ptr kullandik, scope sonunda onlar kendiliginden yok olacak
    }

    void TestTexture2D::OnUpdate(float deltaTime)
    {
    }

    void TestTexture2D::OnRender()
    {
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));


        Renderer renderer; //su anki nesne yapimizla bunun maliyeti cok dusuk

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
            glm::mat4 m_ModelViewProjection = m_Proj * m_View * model;

            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP", m_ModelViewProjection);
            m_Texture->Bind();

            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        }

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);
            glm::mat4 m_ModelViewProjection = m_Proj * m_View * model;

            m_Shader->SetUniformMat4f("u_MVP", m_ModelViewProjection);
            m_Texture_Brick->Bind();

            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        }
    }

    void TestTexture2D::OnImGuiRender()
    {
        ImGui::SliderFloat3("Translation A", &m_TranslationA.x, -1.0f, 1.0f);
        ImGui::SliderFloat3("Translation B", &m_TranslationB.x, -1.0f, 1.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
}