// Created by Berkush on 18.03.2026.

#include "TestClearColor.h"

#include "Renderer.h" //->Bu glewi cagiriyor zaten
#include "imgui.h"

namespace Test
{
    TestClearColor::TestClearColor()
        : m_ClearColor {0.05f, 0.0f, 0.0f, 1.0f }
    {
    }

    TestClearColor::~TestClearColor()
    {
    }

    void TestClearColor::OnUpdate(float deltaTime)
    {
        //Test::OnUpdate(deltaTime);
    }

    void TestClearColor::OnRender()
    {
        GLCall(glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));
    }

    void TestClearColor::OnImGuiRender()
    {
        ImGui::ColorEdit4("Clear Color", m_ClearColor);
    }
}


