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

        struct GraphicsShaders
        {
            const Sierra::Shader& vertexShader;
            const Sierra::Shader& fragmentShader;
        };

        [[nodiscard]] std::optional<GraphicsShaders> GetGraphicsShaders(const EditorAssetManager& assetManager, const ShaderID ID, const std::string_view name)
        {
            // NOTE: Entry points are looked up by the names given to them within the shader sources
            const Sierra::Shader* vertexShader = assetManager.GetShader(ID, "vertexMain");
            const Sierra::Shader* fragmentShader = assetManager.GetShader(ID, "fragmentMain");

            if (vertexShader == nullptr || fragmentShader == nullptr)
            {
                APP_WARNING("Built-in shader [{0}] must have entry points [vertexMain] and [fragmentMain]", name);
                return std::nullopt;
            }

            return GraphicsShaders { .vertexShader = *vertexShader, .fragmentShader = *fragmentShader };
        }
    }

    /* --- CONSTRUCTORS --- */

    EditorApplication::EditorApplication(const EditorApplicationCreateInfo& createInfo)
        : Application({ .name = APPLICATION_NAME, .version = APPLICATION_VERSION, .settings = createInfo.settings }),
            frameLimiter({ .maxFrameRate = 60 * SR_PLATFORM_MOBILE }),
            project({ .directoryPath = createInfo.projectDirectoryPath }),
            renderingContext({ .renderingInstance = GetRenderingInstance() }),
            queue(renderingContext.GetDevice().CreateQueue({ .name = "General Queue" })),
            arenaAllocator({ .device = renderingContext.GetDevice() }),
            assetManager({ .device = renderingContext.GetDevice(), .fileManager = GetFileManager() })
    {
        SR_PROFILE_SCOPE();

        // The scene renderer is a mockup drawing a triangle, with a shader from the project's assets, which is compiled whenever present, and otherwise loaded from the Resources folder, as last compiled
        const Sierra::Path sceneShaderFilePath = project.GetAssetPath() / "Shaders" / "Triangle.slang";
        const std::optional<ShaderID> sceneShaderID = assetManager.LoadBuiltInShader({ .name = "Triangle", .sourceFilePath = sceneShaderFilePath });
        APP_THROW_IF(!sceneShaderID.has_value(), Sierra::InvalidOperationError(SR_FORMAT("Cannot create editor application, as scene shader [{0}] could not be loaded", sceneShaderFilePath.GetString())));

        const std::optional<GraphicsShaders> sceneShaders = GetGraphicsShaders(assetManager, *sceneShaderID, "Triangle");
        APP_THROW_IF(!sceneShaders.has_value(), Sierra::InvalidOperationError(SR_FORMAT("Cannot create editor application, as scene shader [{0}] is missing entry points", sceneShaderFilePath.GetString())));

        const std::unique_ptr<Sierra::CommandBuffer> commandBuffer = queue->CreateCommandBuffer({ .name = "Staging Command Buffer" });
        commandBuffer->Begin();

        const DrawFrame drawFrame = renderingContext.Bind(*commandBuffer);
        surface.emplace(drawFrame, EditorSurfaceCreateInfo { .windowManager = GetWindowManager() });

        const ApplicationFrame applicationFrame = Bind();
        editor.emplace(applicationFrame, EditorCreateInfo { .window = surface->GetWindow() });

        assetManager.Update(drawFrame);

        commandBuffer->End();
        queue->SubmitCommandBuffer(*commandBuffer);

        sceneRenderer = std::make_shared<SceneRenderer>(SceneRendererCreateInfo { .renderingContext = renderingContext, .vertexShader = sceneShaders->vertexShader, .fragmentShader = sceneShaders->fragmentShader });

        for (size i = 0; i < VIEWPORT_COUNT; i++)
        {
            ViewportID viewportID;
            Viewport& viewport = editor->CreateViewport(viewportID, { .renderingContext = renderingContext, .renderer = sceneRenderer });

            viewport.GetTransform().SetPosition({ 0.0f, 4.0f, -10.0f * static_cast<float32>(viewportID.GetValue() + 1) / 1.5f });
            viewport.GetTransform().SetRotation({ 0.0f, -20.0f * static_cast<float32>(viewportID.GetValue() + 1) / 1.5f, 0.0f });
        }

        Scene& scene = editor->GetScene();
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

        const ApplicationFrame applicationFrame = Bind();
        const DrawFrame drawFrame = renderingContext.Bind(commandBuffer);

        assetManager.Update(drawFrame);

        arenaAllocator.Bind(drawFrame);
        surface->Render(applicationFrame, drawFrame, *editor);

        commandBuffer.End();
        queue->SubmitCommandBuffer(commandBuffer);

        surface->Present(drawFrame);
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

    /* --- PRIVATE METHODS --- */

    ApplicationFrame EditorApplication::Bind()
    {
        const Sierra::TimePoint bindTime = Sierra::TimePoint::Now();
        const Sierra::TimeStep timeStep = bindTime - lastBindTime;

        lastBindTime = bindTime;
        return
        {
            .timeStep = timeStep,
            .project = project,
            .platformContext = GetPlatformContext(),
            .inputManager = surface->GetWindow().GetInputManager(),
            .cursorManager = surface->GetWindow().GetCursorManager(),
            .touchManager = surface->GetWindow().GetTouchManager()
        };
    }

}
