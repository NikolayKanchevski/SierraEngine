//
// Created by Nikolay Kanchevski on 13.05.24.
//

#include "EditorApplication.h"

namespace SierraEngine
{

    namespace
    {
        constexpr std::string_view APPLICATION_NAME = "Sierra Editor";
        const Sierra::Version APPLICATION_VERSION = Sierra::Version({ 1, 0, 0 });
    }

    /* --- CONSTRUCTORS --- */

    EditorApplication::EditorApplication(const EditorApplicationCreateInfo& createInfo)
        : Application({ .name = APPLICATION_NAME, .version = APPLICATION_VERSION, .settings = createInfo.settings }),
            frameLimiter({ .maxFrameRate = 60 * SR_PLATFORM_MOBILE }),
            threadPool({ .maxThreadCount = std::thread::hardware_concurrency() }),
            project({ .fileManager = GetFileManager(), .projectDirectoryPath = createInfo.projectDirectoryPath }),
            device(GetRenderingContext().CreateDevice({ .name = "General Device" })),
            resourceTable(device->CreateResourceTable({ .name = "General Resource Table" })),
            queue(device->CreateQueue({ .name = "General Queue", .operations = Sierra::QueueOperations::All })),
            arenaAllocator({ .device = *device }),
            assetManager({ .device = *device }),
            sceneRenderer({ .device = *device }),
            editorSurface({ .platformContext = GetPlatformContext(), .device = *device, .resourceTable = *resourceTable }),
            scene({ .name = "Scene" }),
            editor({ .device = *device, .platformContext = GetPlatformContext(), .scene = scene, .resourceTable = *resourceTable })
    {
        const std::unique_ptr<Sierra::CommandBuffer> commandBuffer = queue->CreateCommandBuffer({ .name = "Staging Command Buffer" });
        commandBuffer->Begin();

        assetManager.LoadProjectAssets(GetFileManager(), project);
        assetManager.Update(*commandBuffer);

        commandBuffer->End();
        queue->SubmitCommandBuffer(*commandBuffer);

        constexpr size VIEWPORT_COUNT = 2;
        for (size i = 0; i < VIEWPORT_COUNT; i++)
        {
            const ViewportID viewportID = editor.CreateViewport({
                .title = SR_FORMAT("Viewport [{0}]", i),
                .device = *device,
                .renderer = sceneRenderer,
                .resourceTable = *resourceTable
            });

            ViewportPanel* viewport = editor.GetViewport(viewportID);
            viewport->GetTransform().SetPosition({ 0.0f, 4.0f, -10.0f * (viewportID + 1) / 1.5f });
            viewport->GetTransform().SetRotation({ 0.0f, -20.0f * (viewportID + 1) / 1.5f, 0.0f });
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

        queue->WaitForCommandBuffer(*commandBuffer);

        commandBuffers.resize(editorSurface.GetConcurrentFrameCount());
        for (size i = 0; i < commandBuffers.size(); i++)
        {
            commandBuffers[i] = queue->CreateCommandBuffer({ .name = SR_FORMAT("General Command Buffer [{0}]", i) });
        }
    }

    /* --- POLLING METHODS --- */

    bool EditorApplication::Update()
    {
        Application::Update();

        // Begin frame
        frameLimiter.BeginFrame();

        // Retrieve current command buffer and wait until it is free
        Sierra::CommandBuffer& commandBuffer = *commandBuffers[editorSurface.GetCurrentFrameIndex()];
        queue->WaitForCommandBuffer(commandBuffer);

        // Begin rendering
        if (editorSurface.Update()) return true;
        commandBuffer.Begin();

        // Bind scene resources
        arenaAllocator.Bind(commandBuffer);
        commandBuffer.BindResourceTable(*resourceTable);

        editorSurface.Render(commandBuffer, editor);

        // Submit work to GPU
        commandBuffer.End();
        queue->SubmitCommandBuffer(commandBuffer);

        editorSurface.Present(commandBuffer);
        frameLimiter.EndFrame();

        return false;
    }

    /* --- DESTRUCTOR --- */

    EditorApplication::~EditorApplication() noexcept
    {
        for (const std::unique_ptr<Sierra::CommandBuffer>& commandBuffer : commandBuffers)
        {
            queue->WaitForCommandBuffer(*commandBuffer);
        }
    }

}
