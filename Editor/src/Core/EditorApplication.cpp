//
// Created by Nikolay Kanchevski on 13.05.24.
//

#include "EditorApplication.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    EditorApplication::EditorApplication(const ApplicationCreateInfo &createInfo)
        : Application(createInfo), surface({ .windowManager = GetWindowManager(), .renderingContext = GetRenderingContext() }), threadPool({ .threadCount = std::thread::hardware_concurrency() }), scene({ .renderingContext = GetRenderingContext() })
    {
        commandBuffers.resize(surface.GetConcurrentFrameCount());
        for (uint32 i = 0; i < commandBuffers.size(); i++)
        {
            commandBuffers[i] = GetRenderingContext().CreateCommandBuffer({ .name = std::format("General Command Buffer [{0}]", i) });
        }
    }

    /* --- POLLING METHODS --- */

    bool EditorApplication::Update(const Sierra::TimeStep&)
    {
        std::unique_ptr<Sierra::CommandBuffer> &commandBuffer = commandBuffers[surface.GetCurrentFrameIndex()];
        GetRenderingContext().GetDevice().WaitForCommandBuffer(commandBuffer);

        surface.Update();
        commandBuffer->Begin();

        static bool firstTime = true;
        if (firstTime)
        {
            const EditorCreateInfo createInfo
            {
                .concurrentFrameCount = surface.GetConcurrentFrameCount(),
                .scaling = surface.GetScaling(),
                .renderingContext = GetRenderingContext(),
                .commandBuffer = commandBuffer,
                .templateOutputImage = surface.GetCurrentSwapchainImage(),
                .resourceTable = scene.GetResourceTable()
            };

            editor = std::make_unique<Editor>(createInfo);
            firstTime = false;
        }

        scene.Update(commandBuffer);
        editor->Update(scene, surface.GetInputManager(), surface.GetCursorManager(), surface.GetTouchManager());

        commandBuffer->BindResourceTable(scene.GetResourceTable());
        commandBuffer->BindVertexBuffer(scene.GetArenaAllocator().GetVertexBuffer());
        commandBuffer->BindIndexBuffer(scene.GetArenaAllocator().GetIndexBuffer());

        commandBuffer->SynchronizeImageUsage(surface.GetCurrentSwapchainImage(), Sierra::ImageCommandUsage::None, Sierra::ImageCommandUsage::ColorWrite);
        editor->Render(commandBuffer, surface.GetCurrentSwapchainImage());
        commandBuffer->SynchronizeImageUsage(surface.GetCurrentSwapchainImage(), Sierra::ImageCommandUsage::ColorWrite, Sierra::ImageCommandUsage::Present);

        commandBuffer->End();
        GetRenderingContext().GetDevice().SubmitCommandBuffer(commandBuffer);

        surface.Present(commandBuffer);
        return !surface.IsActive();
    }

    /* --- DESTRUCTOR --- */

    EditorApplication::~EditorApplication()
    {
        GetRenderingContext().GetDevice().WaitForCommandBuffer(commandBuffers[surface.GetCurrentImageIndex()]);
    }

}