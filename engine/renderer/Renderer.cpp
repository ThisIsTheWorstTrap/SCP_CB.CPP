// engine/renderer/Renderer.cpp
#include "Renderer.hpp"
#include "RaylibRenderer.hpp"

Renderer* CreateRenderer()
{
    return new RaylibRenderer();
}