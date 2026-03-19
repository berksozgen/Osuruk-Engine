// Created by Berkush on 19.03.2026.

#include "TestRender3D.h"

#include "CommonValues.h"
#include "Renderer.h" //->Bu glewi cagiriyor zaten
#include "imgui.h"

#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"

namespace Test
{
    bool LoadOBJ(
        const char* path,
        std::vector<float>& out_vertices,      // interleaved (pos + uv)
        std::vector<unsigned int>& out_indices // index buffer
            )
    {
        //opengl sitesinden aliyorum
        //Ok, down with the actual code. We need some temporary variables in which we will store the contents of the .obj :
        std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
        std::vector<glm::vec3> temp_vertices;
        std::vector<glm::vec2> temp_uvs;
        std::vector<glm::vec3> temp_normals;

        //Since Tutorial 5 : A Textured Cube, you know how to open a file :
        FILE * file = fopen(path, "r");
        if( file == NULL ){
            printf("Impossible to open the file !\n");
            return false;
        }

        //Let’s read this file until the end :
        while( 1 )
        {
            char lineHeader[128];
            // read the first word of the line
            int res = fscanf(file, "%s", lineHeader);
            if (res == EOF)
                break; // EOF = End Of File. Quit the loop.

            // else : parse lineHeader


            //(notice that we assume that the first word of a line won’t be longer than 128, which is a very silly assumption. But for a toy parser, it’s all right)

            //Let’s deal with the vertices first :
            if ( strcmp( lineHeader, "v" ) == 0 ){
                glm::vec3 vertex;
                fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
                temp_vertices.push_back(vertex);

                //i.e : If the first word of the line is “v”, then the rest has to be 3 floats, so create a glm::vec3 out of them, and add it to the vector.
            }else if ( strcmp( lineHeader, "vt" ) == 0 ){
                glm::vec2 uv;
                fscanf(file, "%f %f\n", &uv.x, &uv.y );
                temp_uvs.push_back(uv);

                //i.e if it’s not a “v” but a “vt”, then the rest has to be 2 floats, so create a glm::vec2 and add it to the vector.
                //same thing for the normals :
            }else if ( strcmp( lineHeader, "vn" ) == 0 ){
                glm::vec3 normal;
                fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
                temp_normals.push_back(normal);
                //And now the “f”, which is more difficult :
            }else if ( strcmp( lineHeader, "f" ) == 0 ){
                std::string vertex1, vertex2, vertex3;
                unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
                int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
                if (matches != 9){
                    printf("File can't be read by our simple parser : ( Try exporting with other options\n");
                    return false;
                }
                vertexIndices.push_back(vertexIndex[0]);
                vertexIndices.push_back(vertexIndex[1]);
                vertexIndices.push_back(vertexIndex[2]);
                uvIndices    .push_back(uvIndex[0]);
                uvIndices    .push_back(uvIndex[1]);
                uvIndices    .push_back(uvIndex[2]);
                normalIndices.push_back(normalIndex[0]);
                normalIndices.push_back(normalIndex[1]);
                normalIndices.push_back(normalIndex[2]);
                //This code is in fact very similar to the previous one, except that there is more data to read.
            }
        }

        //Processing the data

        std::unordered_map<std::string, unsigned int> vertexMap;

        for (unsigned int i = 0; i < vertexIndices.size(); i++)
        {
            unsigned int vIndex = vertexIndices[i];
            unsigned int uvIndex = uvIndices[i];

            glm::vec3 pos = temp_vertices[vIndex - 1];
            glm::vec2 uv  = temp_uvs[uvIndex - 1];

            // unique key
            std::string key = std::to_string(vIndex) + "/" + std::to_string(uvIndex);

            if (vertexMap.find(key) == vertexMap.end())
            {
                // yeni vertex
                out_vertices.push_back(pos.x);
                out_vertices.push_back(pos.y);
                out_vertices.push_back(pos.z);

                out_vertices.push_back(uv.x);
                out_vertices.push_back(uv.y);

                unsigned int newIndex = (out_vertices.size() / 5) - 1;
                vertexMap[key] = newIndex;

                out_indices.push_back(newIndex);
            }
            else
            {
                // reuse index
                out_indices.push_back(vertexMap[key]);
            }
        }

        //The same is applied for UVs and normals, and we’re done !
        return true;
    }

    ////opemgl sitesi
    ////Using the loaded data

    ////Once we’ve got this, almost nothing changes. Instead of declaring our usual static const GLfloat g_vertex_buffer_data[] = {…}, you declare a std::vector vertices instead (same thing for UVS and normals). You call loadOBJ with the right parameters :

    //// Read our .obj file
    //std::vector< glm::vec3 > vertices;
    //std::vector< glm::vec2 > uvs;
    //std::vector< glm::vec3 > normals; // Won't be used at the moment.
    //bool res = LoadOBJ("cube.obj", vertices, uvs, normals);

    ////and give your vectors to OpenGL instead of your arrays :
    //glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    ////And that’s it !

    constexpr float projectionMultiplier = 2.f;

    TestRender3D::TestRender3D()
        : m_Translation(0.0f, 0.0f, -10.0f),
        m_Proj(glm::ortho(-widthToHeightRatio * projectionMultiplier, widthToHeightRatio * projectionMultiplier, -1.0f * projectionMultiplier, 1.0f * projectionMultiplier, -1.0f, 1.0f)),
        m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))),
        m_Rotation(0.0f, -90.0f, 0.0f)
    {
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        m_Proj = glm::perspective(glm::radians(45.0f), widthToHeightRatio, 0.1f, 100.0f);

        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        LoadOBJ(ASSET_DIR "/Models/Porsche_911_GT2.obj", vertices, indices);

        m_VertexBuffer = std::make_unique<VertexBuffer>(
            vertices.data(),
            vertices.size() * sizeof(float)
        );



        VertexBufferLayout layout;
        layout.Push<float>(3); //xyz
        layout.Push<float>(2); //uv

        m_VAO = std::make_unique<VertexArray>();
        m_VAO->AddBuffer(*m_VertexBuffer, layout);


        m_IndexBuffer = std::make_unique<IndexBuffer>(
        indices.data(),
        indices.size()
        );

        m_Shader = std::make_unique<Shader>(ASSET_DIR "/Shaders/Basic.shader");
        m_Shader->Bind();
        m_Shader->SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);
        m_Texture = std::make_unique<Texture>(ASSET_DIR "/Models/Porsche_911_GT2.BMP");
        m_Shader->SetUniform1i("u_Texture", 0);
    }

    TestRender3D::~TestRender3D()
    {
        //unique ptr kullandik, scope sonunda onlar kendiliginden yok olacak
    }

    void TestRender3D::OnUpdate(float deltaTime)
    {
    }

    void TestRender3D::OnRender()
    {
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));


        Renderer renderer; //su anki nesne yapimizla bunun maliyeti cok dusuk

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Translation);
            model = glm::rotate(model, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

            glm::mat4 m_ModelViewProjection = m_Proj * m_View * model;

            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP", m_ModelViewProjection);
            m_Texture->Bind();

            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        }
    }

    void TestRender3D::OnImGuiRender()
    {
        ImGui::SliderFloat3("Translation", &m_Translation.x, -10.0f, 10.0f);
        ImGui::SliderFloat3("Rotation", &m_Rotation.x, -180.0f, 180.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
}