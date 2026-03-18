// Created by Berkush on 18.03.2026.

#pragma once

namespace Test
{
    class Test
    {
    public:
        Test() {}
        virtual ~Test() {}

        virtual void OnUpdate(float deltaTime);
        virtual void OnRender();
        virtual void OnImGuiRender();
    };
}