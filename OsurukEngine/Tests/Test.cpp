// Created by Berkush on 19.03.2026.

#include "Test.h"

#include "imgui.h"

namespace Test
{
    TestMenu::TestMenu(Test*& currentTestPointer)
        : m_CurrentTest(currentTestPointer)
    {

    }

    TestMenu::~TestMenu()
    {

    }

    void TestMenu::OnImGuiRender()
    {
        for (auto& test : m_Tests)
        {
            if (ImGui::Button(test.first.c_str()))
            {
                m_CurrentTest = test.second();
            }
        }
    }
}
