#include "Input.hpp"
#include "RaylibInput.hpp"

std::unique_ptr<Input> CreateInput()
{
    return std::make_unique<RaylibInput>();
}