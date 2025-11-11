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

        constexpr uint32 VIEWPORT_COUNT = 4;
    }

    /* --- CONSTRUCTORS --- */

    EditorApplication::EditorApplication(const EditorApplicationCreateInfo& createInfo)
        : Application({ .name = APPLICATION_NAME, .version = APPLICATION_VERSION, .settings = createInfo.settings }),
            frameLimiter({ .maxFrameRate = 60 * SR_PLATFORM_MOBILE }),
            // threadPool({ .maxThreadCount = std::thread::hardware_concurrency() }),
            project({ .fileManager = GetFileManager(), .projectDirectoryPath = createInfo.projectDirectoryPath }),
            renderingContext({ .renderingInstance = GetRenderingInstance() }),
            queue(renderingContext.GetDevice().CreateQueue({ .name = "General Queue" })),
            arenaAllocator({ .renderingContext = renderingContext }),
            assetManager({ .renderingContext = renderingContext }),
            editor({ .platformContext = GetPlatformContext(), .renderingContext = renderingContext })
    {
        assetManager.LoadProjectAssets(GetFileManager(), project);

        const std::unique_ptr<Sierra::CommandBuffer> commandBuffer = queue->CreateCommandBuffer({ .name = "Staging Command Buffer" });

        commandBuffer->Begin();
        renderingContext.Bind(*commandBuffer);

        // std::optional
        surface.emplace(EditorSurfaceCreateInfo { .platformContext = GetPlatformContext(), .renderingContext = renderingContext, .commandBuffer = *commandBuffer });
        assetManager.Update(*commandBuffer);

        commandBuffer->End();
        queue->SubmitCommandBuffer(*commandBuffer);

        sceneRenderer = std::make_shared<SceneRenderer>(SceneRendererCreateInfo { .renderingContext = renderingContext });
        for (size i = 0; i < VIEWPORT_COUNT; i++)
        {
            ViewportID viewportID;
            Viewport& viewport = editor.CreateViewport(viewportID, { .renderingContext = renderingContext, .renderer = sceneRenderer });

            viewport.GetTransform().SetPosition({ 0.0f, 4.0f, -10.0f * (viewportID + 1) / 1.5f });
            viewport.GetTransform().SetRotation({ 0.0f, -20.0f * (viewportID + 1) / 1.5f, 0.0f });
        }

        Scene& scene = editor.GetScene();
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

        commandBuffers.resize(surface->GetConcurrentFrameCount());
        for (size i = 0; i < commandBuffers.size(); i++)
        {
            commandBuffers[i] = queue->CreateCommandBuffer({ .name = SR_FORMAT("General Command Buffer [{0}]", i) });
        }

        queue->WaitForCommandBuffer(*commandBuffer);
    }

    /* --- POLLING METHODS --- */

    bool EditorApplication::Update()
    {
        Application::Update();
        frameLimiter.BeginFrame();

        Sierra::CommandBuffer& commandBuffer = *commandBuffers[surface->GetCurrentFrameIndex()];
        queue->WaitForCommandBuffer(commandBuffer);

        if (surface->Update()) return true;

        commandBuffer.Begin();
        renderingContext.Bind(commandBuffer);

        assetManager.Update(commandBuffer);

        arenaAllocator.Bind(commandBuffer);
        surface->Render(commandBuffer, editor);

        commandBuffer.End();
        queue->SubmitCommandBuffer(commandBuffer);

        surface->Present(commandBuffer);
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
