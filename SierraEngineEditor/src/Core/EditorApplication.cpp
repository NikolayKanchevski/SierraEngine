//
// Created by Nikolay Kanchevski on 13.05.24.
//

#include "EditorApplication.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    EditorApplication::EditorApplication(const ApplicationCreateInfo &createInfo)
        : Application(createInfo),
            frameLimiter({ .maxFrameRate = 60 * SR_PLATFORM_MOBILE }),
            threadPool({ .threadCount = std::thread::hardware_concurrency() }),
            surface({ .windowTitle = "Sierra Engine Editor", .windowManager = GetWindowManager(), .renderingContext = GetRenderingContext() }),
            scene({ .renderingContext = GetRenderingContext() }),
            triangleRenderer({ .renderingContext = GetRenderingContext(), .templateOutputImage = surface.GetCurrentRenderTarget() })
    {
         commandBuffers.resize(surface.GetSwapchain().GetConcurrentFrameCount());
         for (size i = 0; i < commandBuffers.size(); i++)
         {
             commandBuffers[i] = GetRenderingContext().CreateCommandBuffer({ .name = fmt::format("General Command Buffer [{0}]", i) });
         }

        // Create an example scene hierarchy
        const Entity Entity1 = scene.CreateEntity("Entity1");
        const Entity Entity2 = scene.CreateEntity("Entity2");

        const Entity Entity3 = scene.CreateEntity("Entity3");
            const Entity Entity31 = scene.CreateEntity("Entity31"); Entity31.SetParent(Entity3);
                const Entity Entity311 = scene.CreateEntity("Entity311"); Entity311.SetParent(Entity31);
                const Entity Entity312 = scene.CreateEntity("Entity312"); Entity312.SetParent(Entity31);
            const Entity Entity32 = scene.CreateEntity("Entity32"); Entity32.SetParent(Entity3);
            const Entity Entity33 = scene.CreateEntity("Entity33"); Entity33.SetParent(Entity3);
                const Entity Entity331 = scene.CreateEntity("Entity331"); Entity331.SetParent(Entity33);

        const Entity Entity4 = scene.CreateEntity("Entity4");
        const Entity Entity5 = scene.CreateEntity("Entity5");
        const Entity Entity6 = scene.CreateEntity("Entity6");
    }

    /* --- POLLING METHODS --- */

    bool EditorApplication::Update()
    {
        frameLimiter.BeginFrame();

        Sierra::CommandBuffer &commandBuffer = *commandBuffers[surface.GetSwapchain().GetCurrentFrameIndex()];
        GetRenderingContext().GetDevice().WaitForCommandBuffer(commandBuffer);

        surface.Update();
        commandBuffer.Begin();

        static bool firstTime = true;
        if (firstTime)
        {
            const EditorCreateInfo createInfo
            {
                .theme = EditorTheme::Dark,
                .concurrentFrameCount = surface.GetSwapchain().GetConcurrentFrameCount(),
                .scaling = surface.GetSwapchain().GetScaling(),
                .renderingContext = GetRenderingContext(),
                .commandBuffer = commandBuffer,
                .templateOutputImage = surface.GetCurrentSwapchainImage(),
                .resourceTable = scene.GetResourceTable()
            };

            editorRenderer = std::make_unique<EditorRenderer>(createInfo);
            firstTime = false;
        }

        // Update editor
        editorRenderer->Update(scene, surface.GetCurrentRenderTarget(), &surface.GetWindow().GetInputManager(), &surface.GetWindow().GetCursorManager(), &surface.GetWindow().GetTouchManager());

        // Bind scene resources
        commandBuffer.BindResourceTable(scene.GetResourceTable());
        commandBuffer.BindVertexBuffer(scene.GetArenaAllocator().GetVertexBuffer());
        commandBuffer.BindIndexBuffer(scene.GetArenaAllocator().GetIndexBuffer());

        // Render triangle
        scene.GetResourceTable().BindSampledImage(10, surface.GetCurrentRenderTarget());

        commandBuffer.SynchronizeImageUsage(surface.GetCurrentRenderTarget(), Sierra::ImageCommandUsage::None, Sierra::ImageCommandUsage::ColorWrite);
        triangleRenderer.Render(commandBuffer, surface.GetCurrentRenderTarget());
        commandBuffer.SynchronizeImageUsage(surface.GetCurrentRenderTarget(), Sierra::ImageCommandUsage::ColorWrite, Sierra::ImageCommandUsage::ColorRead);

        // Render editor overlay
        commandBuffer.SynchronizeImageUsage(surface.GetCurrentSwapchainImage(), Sierra::ImageCommandUsage::None, Sierra::ImageCommandUsage::ColorWrite);
        editorRenderer->Render(commandBuffer, surface.GetCurrentSwapchainImage());
        commandBuffer.SynchronizeImageUsage(surface.GetCurrentSwapchainImage(), Sierra::ImageCommandUsage::ColorWrite, Sierra::ImageCommandUsage::Present);

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
