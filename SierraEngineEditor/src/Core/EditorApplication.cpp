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
            surface({ .windowManager = GetWindowManager(), .renderingContext = GetRenderingContext() }),
            scene({ .renderingContext = GetRenderingContext() })
    {
         commandBuffers.resize(surface.GetConcurrentFrameCount());
         for (size i = 0; i < commandBuffers.size(); i++)
         {
             commandBuffers[i] = GetRenderingContext().CreateCommandBuffer({ .name = fmt::format("General Command Buffer [{0}]", i) });
         }

        // Create an example scene hierarchy
        Entity Entity1 = scene.CreateEntity("Entity1");
        Entity Entity2 = scene.CreateEntity("Entity2");

        Entity Entity3 = scene.CreateEntity("Entity3");
            Entity Entity31 = scene.CreateEntity("Entity31"); Entity31.SetParent(Entity3);
                Entity Entity311 = scene.CreateEntity("Entity311"); Entity311.SetParent(Entity31);
                Entity Entity312 = scene.CreateEntity("Entity312"); Entity312.SetParent(Entity31);
            Entity Entity32 = scene.CreateEntity("Entity32"); Entity32.SetParent(Entity3);
            Entity Entity33 = scene.CreateEntity("Entity33"); Entity33.SetParent(Entity3);
                Entity Entity331 = scene.CreateEntity("Entity331"); Entity331.SetParent(Entity33);

        Entity Entity4 = scene.CreateEntity("Entity4");
        Entity Entity5 = scene.CreateEntity("Entity5");
        Entity Entity6 = scene.CreateEntity("Entity6");
    }

    /* --- POLLING METHODS --- */

    bool EditorApplication::Update()
    {
        frameLimiter.BeginFrame();

        Sierra::CommandBuffer &commandBuffer = *commandBuffers[surface.GetCurrentFrameIndex()];
        GetRenderingContext().GetDevice().WaitForCommandBuffer(commandBuffer);

        surface.Update();
        commandBuffer.Begin();

        static bool firstTime = true;
        if (firstTime)
        {
            const EditorCreateInfo createInfo
            {
                .theme = EditorTheme::Dark,
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

        // Update editor
        editor->Update(scene, &surface.GetInputManager(), &surface.GetCursorManager(), &surface.GetTouchManager());

        // Bind scene resources
        commandBuffer.BindResourceTable(scene.GetResourceTable());
        commandBuffer.BindVertexBuffer(scene.GetArenaAllocator().GetVertexBuffer());
        commandBuffer.BindIndexBuffer(scene.GetArenaAllocator().GetIndexBuffer());

        // Render editor overlay
        commandBuffer.SynchronizeImageUsage(surface.GetCurrentSwapchainImage(), Sierra::ImageCommandUsage::None, Sierra::ImageCommandUsage::ColorWrite);
        editor->Render(commandBuffer, surface.GetCurrentSwapchainImage());
        commandBuffer.SynchronizeImageUsage(surface.GetCurrentSwapchainImage(), Sierra::ImageCommandUsage::ColorWrite, Sierra::ImageCommandUsage::Present);

        // Submit work to GPU
        commandBuffer.End();
        GetRenderingContext().GetDevice().SubmitCommandBuffer(commandBuffer);

        surface.Present(commandBuffer);
        frameLimiter.EndFrame();

        return !surface.IsActive();
    }

    /* --- DESTRUCTOR --- */

    EditorApplication::~EditorApplication()
    {
        GetRenderingContext().GetDevice().WaitForCommandBuffer(*commandBuffers[surface.GetCurrentImageIndex()]);
    }

}
