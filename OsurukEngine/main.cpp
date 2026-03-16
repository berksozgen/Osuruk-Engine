//  Osuruk Engine
//
//  Started by R. Berk SÖZGEN on 16 March 2026.

#define STB_IMAGE_IMPLEMENTATION //Texture handling icin, tek bir headerdan olusuyor, calismasi icin bu define zorunluymus

#include <cstring>
#include <iostream>

//GLEW -> Bunu kullanma sebebimiz ornek olarak Windows tarafinda OpenGL 1.1'e kadar destek var, kalani icin OpenGL'i ekran karti ureticileri kendi implemente ediyor fonksiyonlari ve o fonksiyonlari
//cagirmak icin bunlarin (surumune gore de degisiyor) dll'lerinden fonksiyon pointerlarinin yerini bulup compile edip durmamiz gerekiyor. acikcasi tam enayi isi
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>

//GLFW -> Bunu kullanma sebebimiz sadece pencere acmak, her platformun kendi API'i farkli, ugrasmaya gerek yok; bu arada kodda ilk bu calisiyor ama GLEW'i yukarida cagiriyoruz, baglama ile ilgili tamamen
#include <GLFW/glfw3.h>

//GLM -> Matrix matematik kutuphanesi
//#include <GLM/glm.hpp>
//#include <GLM/gtc/matrix_transform.hpp>
//#include <GLM/gtc/type_ptr.hpp>

//imgui -> DearImgui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

static unsigned int CompileShader(unsigned int shaderType, const std::string& source) //const char* source clion boyle dolduruyo, kursu yapan adam yapti, aslinda karsilik gelen sey GLenum.
{
    unsigned int id = glCreateShader(shaderType);
    const char* src = source.c_str(); //&source[0] da yapabilirsiniz diyo //btw stringin valid olduguna bakmak isteyebilirsiniz diyo, L value falan filan diyo
    glShaderSource(id, 1, &src, nullptr); //pointerin pointerimis src //sondaki bi kisim da shaderin bir kismini kullanmak istiyorsak diye, NULL da diyebilirmisiz
    glCompileShader(id);

    //Error handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); //iv -> i integer, v vector
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char)); //stackte allocote ediyormus alloca
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0; //assert ekleyebilirsin dedi onlari ogrenmek lazim
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) //ikisini de opengl linklesin istiyoruz
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    //Link error handling =>claude agent ekledi bunu btw
    int linkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE)
    {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetProgramInfoLog(program, length, &length, message);
        std::cout << "Failed to link shader program!" << std::endl;
        std::cout << message << std::endl;
        glDeleteProgram(program);
        return 0;
    }

    glDeleteShader(vs); //zaten linkledik, intermediate dosyalari gibi dusun
    glDeleteShader(fs); //btw glDetachShader cagrasi linklendikten sonra yapiliyor, manuel cagirmaya gerek yok dedi -> bi de debug yapmaniz zorlasir dedi

    return program;
}

float deltaTime = 1.0f;
float lastTime = 0.0f;

int frameCountForOptimal = 0;
float totalDeltaTime = 0;
float cachedDeltaTime = 0;
void DrawImGUIHud()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    frameCountForOptimal++;
    totalDeltaTime += deltaTime;
    if (frameCountForOptimal >= 500)
    {
        cachedDeltaTime = totalDeltaTime / 500.0f;
        frameCountForOptimal = 0;
        totalDeltaTime = 0;
    }

    ImGui::Begin("My name is window, ImGUI window");
    ImGui::Text("FPS:%.0f\n", 1 / cachedDeltaTime);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void CreateBufferObject(unsigned int& buffer, int arraysize, float* datas)
{
    glGenBuffers(1, &buffer); //her obje icin buffen atamak mantikli
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, arraysize * sizeof(float), datas, GL_STATIC_DRAW);
}

int RenderThisFrame(GLFWwindow* window, int bufferCount, unsigned int* buffers)
{
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < bufferCount; i++)
    {
        glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
        glEnableVertexAttribArray(0); //bunu baska yere yazabiliyor muyum denemem lazim, her obje icin cagrilcakmi
        glVertexAttribPointer(0, 2/*2d uzaydayiz, float dizisinin genisligi, vektoru iste*/, GL_FLOAT, GL_FALSE,
            8/*her vertex arasindaki bosluk, uv normal faln da verirsek kayma oluyor onun toplam byte boyutu, size * sizeof(float) da diyebiliriz*/,
            0/*bu da bizim girdimizin ne kaydigi, UV falansa 2. elaman olacak offset vercez*/); //const void* verisiyle sarmak gerekiyor pointeri

    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    DrawImGUIHud();

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    return 1;
}

int CreateWindowOpenGL()
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1280, 720, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); //0 fps limiti yok, 1 refresh rate

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


    float positions[6] =
    {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };


    unsigned int buffer;
    //CreateBufferObject(buffer, 6, positions);
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(float) * 2,0); //index 0 dedigi shader codedaki layout location kismi

    std::string vertexShader = //cpp da "" "" ya da alt satira gecmesi bir hataya sebep olmuyormus, arti koymadan baglaniyormus bunlar
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec4 position;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   gl_Position = position;\n"
        "}\n";

    std::string fragmentShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) out vec4 color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";

    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        const float now = glfwGetTime(); //sdl icin SDL_GetPerformanceCounter(); //UnixTime veya Tick mi ne donduruyor, saniye dondurmuyor
        deltaTime = now - lastTime; // (now - lastTime) * 1000 / SDL_GetPerformanceFrequency();
        lastTime = now;

        /* Render here */
        ////RenderThisFrame(window, 1, &buffer);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glDrawArrays(GL_TRIANGLES, 0, 3);


        DrawImGUIHud();
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader); //bunu yapmak gerekiyormus->vertex bufferlar falan icin de

    glfwTerminate();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}

enum Renderer
{
    RENDERER_OPENGL,
    RENDERER_VULKAN,
    RENDERER_DIRECTX
};

int main(int argc, const char * argv[])
{
    Renderer CurrentRenderer = RENDERER_OPENGL;
    for(int i = 0; i < argc; i++)
    {
        std::cout << "Argument "<< i << " = " << argv[i] << std::endl;

        if(strcmp(argv[i], "-opengl") == 0)
        {
            CurrentRenderer = RENDERER_OPENGL;
            break;
        }
        else if(strcmp(argv[i], "-vulkan") == 0)
        {
            CurrentRenderer = RENDERER_VULKAN;
            break;
        }
        else if(strcmp(argv[i], "-directx") == 0)
        {
            CurrentRenderer = RENDERER_DIRECTX;
            break;
        }
    }

    int ExitCode = 0;

    switch(CurrentRenderer)
    {
        case RENDERER_OPENGL:
            ExitCode = CreateWindowOpenGL();
            break;
        default:
            std::cout << "No available renderer to current selection. OpenGL selected by default." << std::endl;
            ExitCode = CreateWindowOpenGL();
            break;
    }

    return ExitCode;
}

/*
 //Legacy OpenGL versiyonu, ucgen ciziyor
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.5f, -0.5f);
        glEnd();


//basit bir draw call ornegi
//glDrawArrays(GL_TRIANGLES, 0, 3); //first offset, count ta indici, yani kac tane bu elemandan var demek
->btw use shader kullanmadan bununla birlikte ekrana cizim yapip bir sey gorabiliyorsak; gpunun standart shaderi vardir demektir ve bu aslinda opengl standartlarina karsi bisi
 */
