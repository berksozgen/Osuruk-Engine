//
// Created by Berkush on 18.03.2026.
//

#include "MyFirstProgram.h"

#include <cstdio>
#include <vector>

#include "CommonValues.h"
#include "imgui.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexBufferLayout.h"
#include "GLM/vec3.hpp"
#include "GLM/ext/matrix_clip_space.hpp"
#include "GLM/ext/matrix_transform.hpp"

namespace Test
{
    float positions[] =
    {
        //x      y  -  u     v
        -0.5f, -0.5f, 0.0f, 0.0f, //0
         0.5f, -0.5f, 1.0f, 0.0f, //1
         0.5f,  0.5f, 1.0f, 1.0f, //2
        -0.5f,  0.5f, 0.0f, 1.0f  //3
    };
    unsigned int indices[] = //Bunu int yapmak zorunda degiliz char falan da yapabiliyor, unsigned olmali ama
    {
        0, 1, 2,
        2, 3, 0
    };

    std::vector<glm::vec3> translations;
    VertexArray* va = nullptr;
    VertexBuffer* vb = nullptr;

    VertexBufferLayout layout;

    IndexBuffer* ib = nullptr;

    Shader* shader = nullptr;

    Texture* texture = nullptr;

    Renderer renderer;

    float red = 1.0f;
    float green = 0.0f;
    float blue = 0.0f;
    const float cycleTime = 1.0f;

    glm::mat4 proj = glm::ortho(-widthToHeightRatio, widthToHeightRatio, -1.0f, 1.0f, -1.0f, 1.0f); //zNear ile zFar'i belirtmeye gerek yokmus -> ortho'da genel olarak 2D icin
    glm::mat4 view = glm::mat4(1.0f); //bunun bir bugu vardi, bilerek matrixi construct ediyorum adma gibi yapmiyorum

    glm::mat4 model = glm::mat4(1.0f);
    //Location->Rotation->Scale sirasi Transformation Matrix'i

    //imgui degiskenleri
    int frameCountForOptimal = 0;
    float totalDeltaTime = 0;
    float cachedDeltaTime = 0;

    static int ObjectCount = 1;

    bool useRgb = true;
    bool showDemoWindow = false;

    ImVec4 clearColor = ImVec4(0.05f, 0.0f, 0.0f, 1.00f);
    //imgui degiskenleri

    MyFirstProgram::MyFirstProgram()
    {
        va = new VertexArray();
        vb = new VertexBuffer(positions, 4 * 4 * sizeof(float));
        ib = new IndexBuffer(indices, 6);

        shader = new Shader(ASSET_DIR "/Shaders/Basic.shader");
        texture = new Texture(ASSET_DIR "/Textures/berkush_256.png");

        layout.Push<float>(2);
        layout.Push<float>(2);
        va->AddBuffer(*vb, layout);


        view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.0f));

        shader->Bind();
        shader->SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

        model = glm::translate(model, glm::vec3(1.5f, -0.5f, 0.0f));
        glm::mat4 m_ModelViewProjection = proj * view * model; //DirectX ile OpenGL birbirine transpose matrix kullaniyor, orada siralama tam tersi.
        shader->SetUniformMat4f("u_MVP", m_ModelViewProjection);

        texture->Bind();
        shader->SetUniform1i("u_Texture", 0);

        //herseye unbind olma
        va->Unbind();
        shader->Unbind();
        vb->Unbind();
        ib->Unbind();
        //herseye unbind olma

        //ImGui::CreateContext(); //bunlar adamin implementi, ben kendim yapicam valla
        //ImGui_ImplGlfw_InitForOpenGL(window, true); //ImGui_ImplGlfwGL3_Init(); ->Bu legacy olmus galiba
        //ImGui_ImplOpenGL3_Init("#version 330 core"); //bunu yazmadiydi adam
        //ImGui::StyleColorsDark();

        translations.push_back(glm::vec3(0.0f, 0.0f, 0.0f)); //default obje hep ortada
    }

    MyFirstProgram::~MyFirstProgram()
    {
        delete va;
        delete vb;
        delete ib;
        delete shader;
        delete texture;
    }

    void MyFirstProgram::OnUpdate(float deltaTime)
    {
        parseDeltaTimeToImgui = deltaTime;

        //renderer.Clear();
        renderer.ClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        //ImGui_ImplOpenGL3_NewFrame(); //bunlar da adamin imgui kodu
        //ImGui_ImplGlfw_NewFrame();
        //ImGui::NewFrame(); //bunlari da yazmadiydi

        //->Renderer su an handle etmiyor bunu
        shader->Bind();

        if (useRgb)
            shader->SetUniform4f("u_Color", red, green, blue, 1.0f); //-> Not: Shader'a bind olduktan sonra uniforma eris!
        else
            shader->SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f); //-> Not: Shader'a bind olduktan sonra uniforma eris!

            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translations[0]);
                //Location->Rotation->Scale sirasi Transformation Matrix'i
                glm::mat4 m_ModelViewProjection = proj * view * model; //DirectX ile OpenGL birbirine transpose matrix kullaniyor, orada siralama tam tersi.

                shader->SetUniformMat4f("u_MVP", m_ModelViewProjection); //Burada Bind'li Shader, yukarida cagirdik
                //->Renderer su an handle etmiyor bunu

                renderer.Draw(*va, *ib, *shader);
            }

            for (unsigned int i = 1; i < ObjectCount; i++)
            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translations[i]);
                glm::mat4 m_ModelViewProjection = proj * view * model;
                //shader.Bind();
                shader->SetUniformMat4f("u_MVP", m_ModelViewProjection); //Burada da Bind'li, Draw call'in bind ettigini biliyoruz, gereksiz cagri yapmamiza gerek yok.

                renderer.Draw(*va, *ib, *shader);
            }

            if(red>=1.f)
            {
                blue -= deltaTime/cycleTime;
                green += deltaTime/cycleTime;
            }
            if (green>=1.f)
            {
                red -= deltaTime/cycleTime;
                blue += deltaTime/cycleTime;
            }
            if (blue>=1.f)
            {
                green -= deltaTime/cycleTime;
                red += deltaTime/cycleTime;
            }
    }

    void MyFirstProgram::OnRender()
    {
    }

    void MyFirstProgram::OnImGuiRender()
    {
        frameCountForOptimal++;
        totalDeltaTime += parseDeltaTimeToImgui;
        if (frameCountForOptimal >= 500)
        {
            cachedDeltaTime = totalDeltaTime / 500.0f;
            frameCountForOptimal = 0;
            totalDeltaTime = 0;
        }

        ImGui::Begin("My name is window, ImGUI window");
        {
            static float f = 0.0f;
            //ImGui::Begin("Hello, world!");
            ImGui::Text("This is some useful text.");
            //ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            //ImGui::SliderFloat("Horizontal", &translationA.x, -widthToHeightRatio, widthToHeightRatio);
            //ImGui::SliderFloat("Vertical", &translationA.y, -1.0f, 1.0f);
            for (unsigned int i = 0; i < ObjectCount; i++)
            {
                char name[32];
                sprintf(name, "Translation %d", i);
                ImGui::SliderFloat3(name, &translations[i].x, -1.0f, 1.0f);
            }

            ImGui::ColorEdit3("clear color", (float*)&clearColor);

            ImGui::Checkbox("Use RGB", &useRgb);
            ImGui::Checkbox("Demo Window", &showDemoWindow);

            if (ImGui::Button("Add"))
            {
                ObjectCount++;
                translations.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
            }

            ImGui::SameLine();
            ImGui::Text("Count = %d", ObjectCount);

            ImGui::Text("FPS:%.0f\n", 1 / cachedDeltaTime);
            //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            //ImGui::End();
        }
        ImGui::End();
    }
}
