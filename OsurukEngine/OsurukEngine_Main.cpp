//  Osuruk Engine
//
//  Started by R. Berk SÖZGEN on 16 March 2026.

#include <cstring>
#include <iostream>
#include <fstream>
#include <string> //get line string kutuphanesi diyoda eklemedim ben calsiiyo valla
#include <sstream> //string stream

#include "CommonValues.h"
#include "MyFirstProgram.h"

//GLEW -> Bunu kullanma sebebimiz ornek olarak Windows tarafinda OpenGL 1.1'e kadar destek var, kalani icin OpenGL'i ekran karti ureticileri kendi implemente ediyor fonksiyonlari ve o fonksiyonlari
//cagirmak icin bunlarin (surumune gore de degisiyor) dll'lerinden fonksiyon pointerlarinin yerini bulup compile edip durmamiz gerekiyor. acikcasi tam enayi isi
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>

//GLFW -> Bunu kullanma sebebimiz sadece pencere acmak, her platformun kendi API'i farkli, ugrasmaya gerek yok; bu arada kodda ilk bu calisiyor ama GLEW'i yukarida cagiriyoruz, baglama ile ilgili tamamen
#include <GLFW/glfw3.h>

//GLM -> Matrix matematik kutuphanesi
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

//imgui -> DearImgui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Engine/Renderer.h"

#include "Engine/VertexBuffer.h"
#include "Engine/VertexBufferLayout.h"
#include "Engine/IndexBuffer.h"
#include "Engine/VertexArray.h"
#include "Engine/Shader.h"
#include "Engine/Texture.h"



float deltaTime = 1.0f;
float lastTime = 0.0f;

Test::MyFirstProgram test = Test::MyFirstProgram();


void DrawImGUIHud()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    test.OnImGuiRender();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int CreateWindowOpenGL()
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    //Bunu acmadan Vertex Array Objesi'ni kendi olusturuyormus OpenGL (Compat Profil icin), aslinda saglamamiz gereken bir seymis
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //BUNU yazmadan GLFW_OPENGL_COMPAT_PROFILE'da calisiyormus bu da son opengl surumunda calisiyor
    //Bunu acmadan Vertex Array Objesi'ni kendi olusturuyormus OpenGL (Compat Profil icin), aslinda saglamamiz gereken bir seymis


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Hello World from Osuruk Engine", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(LIMIT_FPS);

    /* Contexten sonra GLEW init */
    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW init failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    GLCall(glEnable(GL_BLEND)); //bende bunlari yazmadan calisiyordu alpha kanalinin buglu olmamasi
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)); //bende bunlari yazmadan calisiyordu alpha kanalinin buglu olmamasi


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        const float now = glfwGetTime(); //sdl icin SDL_GetPerformanceCounter(); //UnixTime veya Tick mi ne donduruyor, saniye dondurmuyor
        deltaTime = now - lastTime; // (now - lastTime) * 1000 / SDL_GetPerformanceFrequency();
        lastTime = now;

        /* Render here */
        //renderer.Clear();

        test.OnUpdate(deltaTime);
        test.OnRender();

        DrawImGUIHud();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();

    //Cleanup imgui
    ImGui_ImplOpenGL3_Shutdown();
    //ImGui_ImplGlfw_Shutdown(); -> Bunu da koymadi
    ImGui::DestroyContext();

    return 0;
}

static enum ERenderer : unsigned char
{
    OPENGL = 0,
    VULKAN,
    DIRECTX,
    SOFTWARE,
}CurrentRenderer = OPENGL; //OpenGL is the default renderer for now.

static void SetRendererFromArgs(int argc, const char * argv[])
{
    for(int i = 0; i < argc; i++)
    {
        std::cout << "Argument "<< i << " = " << argv[i] << std::endl;

        if(strcmp(argv[i], "-opengl") == 0) CurrentRenderer = OPENGL;
        if(strcmp(argv[i], "-vulkan") == 0) CurrentRenderer = VULKAN;
        if(strcmp(argv[i], "-directx") == 0) CurrentRenderer = DIRECTX;
        if(strcmp(argv[i], "-software") == 0) CurrentRenderer = SOFTWARE;
    }

    std::cout << "No available renderer to current selection. OpenGL selected by default." << std::endl;
}

int main(int argc, const char * argv[])
{
    int ExitCode = 0;

    SetRendererFromArgs(argc, argv);
    switch(CurrentRenderer)
    {
        case OPENGL:
            ExitCode = CreateWindowOpenGL();
            break;
        default:
            CreateWindowOpenGL();
            ExitCode = -CurrentRenderer;
            break;
    }

    return ExitCode;
}
