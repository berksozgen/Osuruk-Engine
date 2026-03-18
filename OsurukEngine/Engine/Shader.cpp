//
// Created by Berkush on 17.03.2026.
//

#include "Shader.h"

#include <iostream>
#include <fstream>
//#include <string> //get line string kutuphanesi diyoda eklemedim ben calsiiyo valla
#include <sstream> //string stream

#include "Renderer.h"

Shader::Shader(const std::string& filepath)
    : m_FilePath(filepath), m_RendererID(0)
{
    ShaderProgramSource source = ParseShader(filepath);
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(m_RendererID));
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    std::cout << "Creating shaders from: " + filepath << std::endl;


    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos/*invalid screen position*/)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << '\n'; //otomasyon moruk
        }
    }

    return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompileShader(unsigned int shaderType, const std::string& source) //const char* source clion boyle dolduruyor; kursu yapan adam yapti, aslinda karsilik gelen sey GLenum.
{
    unsigned int id = glCreateShader(shaderType);
    const char* src = source.c_str(); //&source[0] da yapabilirsiniz diyor. Bu arada string'in valid olduguna bakmak isteyebilirsiniz diyor, L value falan filan diyor.
    glShaderSource(id, 1, &src, nullptr); //Pointerin pointeriymis src. Sondaki kisim ise shader'in bir kismini kullanmak istiyorsak diye (Uzunsa parcalamak icin), NULL da diyebilirmisiz.
    glCompileShader(id);

    //Error handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); //iv -> i integer, v vector
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char)); //Stack'te allocote ediyormus "alloca()"
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0; //ASSERT ekleyebilirsin dedi, onlari ogrenmek lazim.
    }

    return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) //Ikisini de OpenGL link'lesin istiyoruz.
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    //Link error handling => Claude Agent ekledi bunu bu.
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

    glDeleteShader(vs); //Zaten linkledik, intermediate dosyalari gibi dusun
    glDeleteShader(fs); //Bu arada glDetachShader cagrisi linklendikten sonra yapiliyor, manuel cagirmaya gerek yok dedi -> bi de debug yapmaniz zorlasir dedi

    return program;
}

void Shader::Bind() const
{
    GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
    GLCall(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string& name, int value)
{
    GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform1f(const std::string& name, float value)
{
    GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
    GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0])); //Baski ggrafik API'larinda transpose kismini isteyebilirmisiz.
}

int Shader::GetUniformLocation(const std::string& name)
{
    if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end()) //bu ne osuruktan bir syntx amk
        return m_uniformLocationCache[name];

    GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1) //-1 ise bulamamis demek oluyor
        std::cout << "WARNING: Uniform " << name << " not found!" << std::endl;

    m_uniformLocationCache[name] = location; //lokasyonun -1 olmasi bir seyi degistirmiyormus, shaderi yeniden compile etmeden gelmicekmis mape zaten ne de olsa

    return location;
}
