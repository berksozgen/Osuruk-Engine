//
// Created by Berkush on 17.03.2026.
//

#ifndef OSURUKENGINE_SHADER_H
#define OSURUKENGINE_SHADER_H
#include <string>
#include <unordered_map>

#include "GLM/glm.hpp"

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

class Shader
{
private:
    std::string m_FilePath; //Debug purpose
    unsigned int m_RendererID;
    std::unordered_map<std::string, int> m_uniformLocationCache; //Her seferinde OpenGL'e string yollayarak lokasyonu almak istemiyoruz
public:
    Shader(const std::string& filepath);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    //Set uniform
    void SetUniform1i(const std::string& name, int value);
    void SetUniform1f(const std::string& name, float value);
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

private:
    ShaderProgramSource ParseShader(const std::string& filepath);
    unsigned int CompileShader(unsigned int shaderType, const std::string& source);
    unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
    int GetUniformLocation(const std::string& name);
};

#endif //OSURUKENGINE_SHADER_H
