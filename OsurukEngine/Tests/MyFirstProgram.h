//
// Created by Berkush on 18.03.2026.
//

#pragma once

#include "Test.h"

namespace Test
{
    class MyFirstProgram : public Test
    {
    public:
        MyFirstProgram();
        ~MyFirstProgram();

        void OnUpdate(float deltaTime) override;
        void OnRender() override;
        void OnImGuiRender() override;

    private:
        float parseDeltaTimeToImgui;
    };
}
