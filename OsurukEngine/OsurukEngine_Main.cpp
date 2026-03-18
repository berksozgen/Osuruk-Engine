//  Osuruk Engine
//
//  Started by R. Berk SÖZGEN on 16 March 2026.

#include <cstring>
#include <iostream>
#include <fstream>
#include <string> //get line string kutuphanesi diyoda eklemedim ben calsiiyo valla
#include <sstream> //string stream

//GLEW -> Bunu kullanma sebebimiz ornek olarak Windows tarafinda OpenGL 1.1'e kadar destek var, kalani icin OpenGL'i ekran karti ureticileri kendi implemente ediyor fonksiyonlari ve o fonksiyonlari
//cagirmak icin bunlarin (surumune gore de degisiyor) dll'lerinden fonksiyon pointerlarinin yerini bulup compile edip durmamiz gerekiyor. acikcasi tam enayi isi
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>

//GLFW -> Bunu kullanma sebebimiz sadece pencere acmak, her platformun kendi API'i farkli, ugrasmaya gerek yok; bu arada kodda ilk bu calisiyor ama GLEW'i yukarida cagiriyoruz, baglama ile ilgili tamamen
#include <GLFW/glfw3.h>

//GLM -> Matrix matematik kutuphanesi -> Mainde ihtiyacimiz kalmadi

//imgui -> DearImgui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "CommonValues.h"

#include "Engine/Renderer.h"
#include "MyFirstProgram.h"
#include "TestClearColor.h"

Test::Test* currentTest;

void DrawImGUIHud()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (currentTest)
    {
        currentTest->OnImGuiRender();
    }
    else
    {
        ImGui::Begin("Test Object Not Valid!");
        //ImGui::Text("Test Object Not Valid!");
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

float deltaTime = 1.0f;
float lastTime = 0.0f;

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

    Test::TestMenu* testMenu = new Test::TestMenu(currentTest);
    currentTest = testMenu;
    //currentTest = new Test::MyFirstProgram();

    testMenu->RegisterTest<Test::TestClearColor>("Clear Color");
    testMenu->RegisterTest<Test::MyFirstProgram>("My First Program");

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        const float now = glfwGetTime(); //sdl icin SDL_GetPerformanceCounter(); //UnixTime veya Tick mi ne donduruyor, saniye dondurmuyor
        deltaTime = now - lastTime; // (now - lastTime) * 1000 / SDL_GetPerformanceFrequency();
        lastTime = now;

        /* Render here */
        //renderer.Clear();
        glClear(GL_COLOR_BUFFER_BIT); //default testte olmuyordu bu, ama bunu cagircagimiz seyini de cagristiriyor bir nevi bende

        //bunlari adamin koda sabit kalmak icin ekliyorum
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //bunlari adamin koda sabit kalmak icin ekliyorum

        if (currentTest)
        {
            currentTest->OnUpdate(deltaTime);
            currentTest->OnRender();
            ImGui::Begin("Test");
            if (currentTest != testMenu && ImGui::Button("<-"))
            {
                delete currentTest;
                currentTest = testMenu;
            }
            currentTest->OnImGuiRender();
            ImGui::End();
        }

        //DrawImGUIHud(); //bunu da ondan cikartiyorum
        //bunlari adamin koda sabit kalmak icin ekliyorum
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        //bunlari adamin koda sabit kalmak icin ekliyorum

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    // LOOP BİTER
    delete currentTest;  // ✅ OpenGL context HALA AKTİF

    if (currentTest != testMenu) //iki kere silmiyelim bunu diye
        delete testMenu;

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
