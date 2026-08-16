#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "engine/core/math/math.hpp"
#include "engine/core/handle/handle.hpp"
#include "color.hpp"

class Renderer
{
    public:
        virtual ~Renderer()=default;

        virtual void Init(int width, int height, const char* title)=0;
        virtual void Shutdown()=0;

        virtual bool ShouldClose()=0;
        virtual float GetDeltaTime()=0;

        virtual void BeginFrame()=0;
        virtual void EndFrame()=0;

        virtual void DrawCube(Coordinates position, float size, Color color)=0;

        virtual TextureHandle LoadTexture(const char* path)=0;
        virtual void DrawCubeTexture(TextureHandle handle, Coordinates position, float size, Color tint)=0;

        virtual ModelHandle LoadModel(const char* path)=0;
        virtual void DrawModel(ModelHandle handle, Coordinates position)=0;
};

Renderer* CreateRenderer();

#endif