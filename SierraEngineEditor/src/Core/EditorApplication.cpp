//
// Created by Nikolay Kanchevski on 13.05.24.
//

#include "EditorApplication.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    EditorApplication::EditorApplication(const ApplicationCreateInfo& createInfo)
        : Application(createInfo),
          frameLimiter({ .maxFrameRate = 60 * SR_PLATFORM_MOBILE }),
          threadPool({ .threadCount = std::thread::hardware_concurrency() }),
          surface({ .title = "Sierra Engine Editor", .windowManager = GetWindowManager(), .renderingContext = GetRenderingContext() }),
          resourceTable(GetRenderingContext().CreateResourceTable({ .name = "General Resource Table" })),
          assetManager({ .renderingContext = GetRenderingContext(), .threadPool = threadPool }),
          scene({ .name = "Scene" }),
          editor({ .scene = scene }),
          triangleRenderer({ .renderingContext = GetRenderingContext(), .templateOutputImage = surface.GetSwapchain().GetCurrentImage() })
    {
         commandBuffers.resize(surface.GetSwapchain().GetConcurrentFrameCount());
         for (size i = 0; i < commandBuffers.size(); i++)
         {
             commandBuffers[i] = GetRenderingContext().CreateCommandBuffer({ .name = fmt::format("General Command Buffer [{0}]", i) });
         }

        // Create an example scene hierarchy
        const EntityID Entity1 = scene.CreateEntity("Entity1");
        const EntityID Entity2 = scene.CreateEntity("Entity2");

        const EntityID Entity3 = scene.CreateEntity("Entity3");
            const EntityID Entity31 = scene.CreateEntity("Entity31"); scene.SetEntityParent(Entity31, Entity3);
                const EntityID Entity311 = scene.CreateEntity("Entity311"); scene.SetEntityParent(Entity311, Entity31);
                const EntityID Entity312 = scene.CreateEntity("Entity312"); scene.SetEntityParent(Entity312, Entity31);
            const EntityID Entity32 = scene.CreateEntity("Entity32"); scene.SetEntityParent(Entity32, Entity3);
            const EntityID Entity33 = scene.CreateEntity("Entity33"); scene.SetEntityParent(Entity33, Entity3);
                const EntityID Entity331 = scene.CreateEntity("Entity331"); scene.SetEntityParent(Entity331, Entity33);

        const EntityID Entity4 = scene.CreateEntity("Entity4");
        const EntityID Entity5 = scene.CreateEntity("Entity5");
        const EntityID Entity6 = scene.CreateEntity("Entity6");
    }

    /* --- POLLING METHODS --- */

    bool EditorApplication::Update()
    {
        frameLimiter.BeginFrame();

        Sierra::CommandBuffer& commandBuffer = *commandBuffers[surface.GetSwapchain().GetCurrentFrameIndex()];
        GetRenderingContext().GetDevice().WaitForCommandBuffer(commandBuffer);

        surface.Update();
        commandBuffer.Begin();

        static bool firstTime = true;
        if (firstTime)
        {
            const EditorRendererCreateInfo createInfo
            {
                .theme = EditorTheme::Dark,
                .concurrentFrameCount = surface.GetSwapchain().GetConcurrentFrameCount(),
                .scaling = surface.GetSwapchain().GetScaling(),
                .renderingContext = GetRenderingContext(),
                .commandBuffer = commandBuffer,
                .templateOutputImage = surface.GetSwapchain().GetCurrentImage(),
                .resourceTable = *resourceTable
            };

            editorRenderer = std::make_unique<EditorRenderer>(createInfo);
            firstTime = false;
        }

        // Update assets
        assetManager.Update(commandBuffer);

        // Update editor
        editorRenderer->Update(editor, surface.GetWindow().GetInputManager(), surface.GetWindow().GetCursorManager(), surface.GetWindow().GetTouchManager());

        // Bind scene resources
        commandBuffer.BindResourceTable(*resourceTable);
//        commandBuffer.BindVertexBuffer(scene.GetArenaAllocator().GetVertexBuffer());
//        commandBuffer.BindIndexBuffer(scene.GetArenaAllocator().GetIndexBuffer());

        // Render triangle
//        resourceTable->BindSampledImage(10, surface.GetCurrentRenderTarget());
//
//        commandBuffer.SynchronizeImageUsage(surface.GetCurrentRenderTarget(), Sierra::ImageCommandUsage::None, Sierra::ImageCommandUsage::ColorWrite);
//        triangleRenderer.Render(commandBuffer, surface.GetCurrentRenderTarget());
//        commandBuffer.SynchronizeImageUsage(surface.GetCurrentRenderTarget(), Sierra::ImageCommandUsage::ColorWrite, Sierra::ImageCommandUsage::ColorRead);

        // Render editor overlay
        commandBuffer.SynchronizeImageUsage(surface.GetSwapchain().GetCurrentImage(), Sierra::ImageCommandUsage::None, Sierra::ImageCommandUsage::ColorWrite);
        editorRenderer->Render(commandBuffer, surface.GetSwapchain().GetCurrentImage());
        commandBuffer.SynchronizeImageUsage(surface.GetSwapchain().GetCurrentImage(), Sierra::ImageCommandUsage::ColorWrite, Sierra::ImageCommandUsage::Present);

        // Submit work to GPU
        commandBuffer.End();
        GetRenderingContext().GetDevice().SubmitCommandBuffer(commandBuffer);

        surface.Present(commandBuffer);
        frameLimiter.EndFrame();

        return surface.GetWindow().IsClosed();
    }

    /* --- DESTRUCTOR --- */

    EditorApplication::~EditorApplication()
    {
        GetRenderingContext().GetDevice().WaitForCommandBuffer(*commandBuffers[surface.GetSwapchain().GetCurrentImageIndex()]);
    }

}
