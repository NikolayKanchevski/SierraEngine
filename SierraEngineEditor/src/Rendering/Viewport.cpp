//
// Created by Nikolay Kanchevski on 28.10.25.
//

#include "Viewport.h"

namespace SierraEngine
{
    /* --- CONSTRUCTORS --- */

    Viewport::Viewport(const ViewportCreateInfo& createInfo)
        : renderingContext(&createInfo.renderingContext), renderer(createInfo.renderer)
    {
        renderTarget = renderer->CreateRenderTarget();
        imageID = renderingContext->GetResourceTable().ReserveSampledImage();
    }

    /* --- SETTER METHODS --- */

    void Viewport::SetRenderer(const std::shared_ptr<SceneRenderer>& givenRenderer)
    {
        renderer->DestroyRenderTarget(renderTarget);

        renderer = givenRenderer;
        renderTarget = renderer->CreateRenderTarget();
    }

    /* --- SETTER METHODS --- */

    void Viewport::SetWidth(const uint32 givenWidth) noexcept
    {
        width = givenWidth;
    }

    void Viewport::SetHeight(const uint32 givenHeight) noexcept
    {
        height = givenHeight;
    }

    /* --- DESTRUCTOR --- */

    Viewport::~Viewport() noexcept
    {
        if (renderer == nullptr) return;

        renderingContext->GetResourceTable().FreeSampledImage(imageID);
        renderer->DestroyRenderTarget(renderTarget);
    }

}