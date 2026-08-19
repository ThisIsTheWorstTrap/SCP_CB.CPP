#include "Renderer.hpp"
#include "RaylibRenderer.hpp"

std::unique_ptr<Renderer> CreateRenderer()
{
    return std::make_unique<RaylibRenderer>();
}