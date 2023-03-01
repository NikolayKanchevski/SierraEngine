//
// Created by Nikolay Kanchevski on 23.12.22.
//

#include "MainVulkanRenderer.h"

#include "../../Math/MatrixUtilities.h"
#include "../../../../Engine/Classes/Time.h"
#include "../../../../Engine/Components/Camera.h"
#include "../../../Engine/Components/WorldManager.h"
#include "../../../../Engine/Components/MeshRenderer.h"

#define USE_THREADED_FOR_EACH

#ifdef USE_THREADED_FOR_EACH
    #define FOR_EACH_LOOP tbb::parallel_for_each
#else
    #define FOR_EACH_LOOP std::for_each
#endif

using namespace Sierra::Engine::Components;

namespace Sierra::Core::Rendering::Vulkan::Renderers
{

    /* --- CONSTRUCTORS --- */

    MainVulkanRenderer::MainVulkanRenderer(const VulkanRendererCreateInfo &createInfo)
        : VulkanRenderer(createInfo)
    {
        InitializeOffscreenRendering();
    }

    UniquePtr<MainVulkanRenderer> MainVulkanRenderer::Create(const VulkanRendererCreateInfo createInfo)
    {
        return std::make_unique<MainVulkanRenderer>(createInfo);
    }

    /* --- POLLING METHODS --- */

    void MainVulkanRenderer::Update()
    {
        // Update camera
        Camera &camera = Camera::GetMainCamera();

        // Update uniform data
        auto &sceneUniformData = scenePipeline->GetUniformBufferData();
        sceneUniformData.view = camera.GetViewMatrix();
        sceneUniformData.projection = camera.GetProjectionMatrix();
        sceneUniformData.inverseView = camera.GetInverseViewMatrix();
        sceneUniformData.inverseProjection = camera.GetInverseProjectionMatrix();

        auto &skyboxUniformData = skyboxPipeline->GetUniformBufferData();
        skyboxUniformData.view = sceneUniformData.view;
        skyboxUniformData.projection = sceneUniformData.projection;

        float timeAngle = std::fmod(Time::GetUpTime(), 360.0f) * -0.8f;
        skyboxPipeline->GetPushConstantData().model = MatrixUtilities::CreateModel(Vector3(0.0f), {timeAngle, 0.0f, 0.0f });

        // Update storage data
        auto &storageData = scenePipeline->GetStorageBufferData();
        storageData.directionalLightCount = DirectionalLight::GetDirectionalLightCount();
        storageData.pointLightCount = PointLight::GetPointLightCount();

        auto enttMeshView = World::GetAllComponentsOfType<MeshRenderer>();
        FOR_EACH_LOOP(enttMeshView.begin(), enttMeshView.end(),
            [&enttMeshView, &storageData](auto &enttEntity)
            {
                MeshRenderer &meshRenderer = enttMeshView.get<MeshRenderer>(enttEntity);
                storageData.objectDatas[meshRenderer.GetMeshID()].model = meshRenderer.GetModelMatrix();
            }
        );

        auto enttDirectionalLightView = World::GetAllComponentsOfType<DirectionalLight>();
        FOR_EACH_LOOP(enttDirectionalLightView.begin(), enttDirectionalLightView.end(),
           [&enttDirectionalLightView, &storageData](auto &enttEntity)
           {
               DirectionalLight &directionalLight = enttDirectionalLightView.get<DirectionalLight>(enttEntity);
               storageData.directionalLights[directionalLight.GetID()] = directionalLight;
           }
        );

        auto enttPointLightView = World::GetAllComponentsOfType<PointLight>();
        FOR_EACH_LOOP(enttPointLightView.begin(), enttPointLightView.end(),
            [&enttPointLightView, &storageData](auto &enttEntity)
            {
                PointLight &pointLight = enttPointLightView.get<PointLight>(enttEntity);
                storageData.pointLights[pointLight.GetID()] = pointLight;
            }
        );
    }

    void MainVulkanRenderer::DrawUI()
    {
        ViewportPanel().DrawUI();

        GUI::BeginWindow("Renderer's Properties", nullptr, ImGuiWindowFlags_NoNav);

        // Shading type dropdown
        {
            static uint currentShadingTypeIndex = 0;
            static const char* shadingTypes[] = {"Shaded", "Wireframe" };

            GUI::CustomLabel("Renderer Shading:");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            if (GUI::Dropdown("##RENDERER_SHADING_DROPDOWN", currentShadingTypeIndex, shadingTypes, 2))
            {
                VK::GetDevice()->WaitUntilIdle();

                scenePipeline->GetCreateInfo().shadingType = (ShadingType) currentShadingTypeIndex;
                scenePipeline->Recreate();
            }
        }

        // Shaders to use
        {
            static uint currentShaderIndex = 2;

            static String shaderPaths[] { File::OUTPUT_FOLDER_PATH + "Shaders/StandardDiffuse.frag.spv", File::OUTPUT_FOLDER_PATH + "Shaders/StandardSpecular.frag.spv", File::OUTPUT_FOLDER_PATH + "Shaders/BlinnPhong.frag.spv" };
            static const char* shaderTypes[] = {"Diffuse", "Specular", "Blinn-Phong" };

            GUI::CustomLabel("Renderer's Fragment Shader:");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            if (GUI::Dropdown("##RENDERER_FRAGMENT_SHADER_DROPDOWN", currentShaderIndex, shaderTypes, 3))
            {
                    scenePipeline->OverloadShader(Shader::Create({
                        .filePath = shaderPaths[currentShaderIndex],
                        .shaderType = ShaderType::FRAGMENT
                    }));
            }
        }

        // Anti-Aliasing
        {
            static uint currentMSAATypeIndex = 0;
            static const char* msaaTypes[] = {"None", "MSAAx2", "MSAAx4", "MSAAx8", "MSAAx16", "MSAAx32", "MSAAx64" };

            uint maximumSampling = static_cast<uint32_t>(VK::GetDevice()->GetHighestMultisampling());
            static const bool deactivatedFlags[] = { 1 > maximumSampling, 2 > maximumSampling, 4 > maximumSampling, 8 > maximumSampling, 16 > maximumSampling, 32 > maximumSampling, 64 > maximumSampling };

            GUI::CustomLabel("Renderer Anti-Aliasing:");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            if (GUI::Dropdown("##RENDERER_ANTI_ALIASING_DROPDOWN", currentMSAATypeIndex, msaaTypes, 7, deactivatedFlags))
            {
                VK::GetDevice()->WaitUntilIdle();

                Sampling newSampling = (Sampling) glm::pow(2, currentMSAATypeIndex);
                sceneOffscreenRenderer->SetMultisampling(newSampling);

                scenePipeline->GetCreateInfo().sampling = newSampling;
                scenePipeline->Recreate();

                skyboxPipeline->GetCreateInfo().sampling = newSampling;
                skyboxPipeline->Recreate();

                CreateOffscreenDescriptorSets();
            }
        }

        GUI::EndWindow();

        VulkanRenderer::DrawUI();
    }

    void MainVulkanRenderer::Render()
    {
        VulkanRenderer::Render();

        // Prepare next swapchain image
        swapchain->AcquireNextImage();

        // Get references to usable variables
        auto &commandBuffer = swapchain->GetCurrentCommandBuffer();
        uint currentFrame = swapchain->GetCurrentFrameIndex();

        // Begin command buffer
        commandBuffer->Begin();

        // Start GPU timer
        offscreenTimestampQueries[currentFrame]->Begin(commandBuffer);

        // Bind the pipeline
        scenePipeline->Bind(commandBuffer);

        // Begin rendering offscreen image
        sceneOffscreenRenderer->Begin(commandBuffer, currentFrame);

        // Create a vector to hold vertex buffers of the meshes
        std::vector<VkBuffer> vertexBuffers(1);

        // For each mesh in the world
        auto enttMeshView = World::GetAllComponentsOfType<MeshRenderer>();
        for (auto enttEntity : enttMeshView)
        {
            // Get current meshRenderer
            auto &meshRenderer = enttMeshView.get<MeshRenderer>(enttEntity);
            vertexBuffers[0] = meshRenderer.GetMesh()->GetVertexBuffer()->GetVulkanBuffer();

            // Bind the vertex buffer
            commandBuffer->BindVertexBuffers(vertexBuffers);

            // Bind the index buffer
            commandBuffer->BindIndexBuffer(meshRenderer.GetMesh()->GetIndexBuffer()->GetVulkanBuffer());

            // Copy push constant data to pipeline
            scenePipeline->GetPushConstantData() = meshRenderer.GetPushConstantData();

            // Send push constant data to shader
            scenePipeline->PushConstants(commandBuffer);

            // Use either bindless or bind*full* descriptor set if not supported
            scenePipeline->BindDescriptorSets(
                    commandBuffer,
                     // TODO: BINDLESS
//                    { VK::GetDevice()->GetDescriptorIndexingSupported() ? globalBindlessDescriptorSet->GetVulkanDescriptorSet() : meshRenderer.GetDescriptorSet() },
                    meshRenderer.GetDescriptorSet()
            );

            // Draw using the index buffer to prevent vertex re-usage
            commandBuffer->DrawIndexed(meshRenderer.GetMesh()->GetIndexCount());
        }

        skyboxPipeline->Bind(commandBuffer);

        commandBuffer->BindVertexBuffers({ skyboxMesh->GetVertexBuffer()->GetVulkanBuffer() });
        commandBuffer->BindIndexBuffer({ skyboxMesh->GetIndexBuffer()->GetVulkanBuffer() });

        skyboxPipeline->PushConstants(commandBuffer);
        skyboxPipeline->BindDescriptorSets(commandBuffer);

        commandBuffer->DrawIndexed(skyboxMesh->GetIndexCount());

        // End the offscreen renderer
        sceneOffscreenRenderer->End(commandBuffer);

        // Begin the render pass
        swapchain->BeginRenderPass(commandBuffer);

        // Apply scissoring and viewport update
        commandBuffer->SetViewportAndScissor(swapchain->GetWidth(), swapchain->GetHeight());

        // Render ImGui UI
        totalVerticesDrawn = Mesh::GetTotalVertexCount();
        if (ImGui::GetDrawData() != nullptr)
        {
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer->GetVulkanCommandBuffer());
            totalVerticesDrawn += ImGui::GetDrawData()->TotalVtxCount;
        }

        // End the render pass
        swapchain->EndRenderPass(commandBuffer);

        // End and save GPU timer results
        offscreenTimestampQueries[currentFrame]->End(commandBuffer);
        this->totalDrawTime = offscreenTimestampQueries[currentFrame]->GetTimeTaken();

        // End the command buffer and check for errors during command execution
        commandBuffer->End();

        // Render to swapchain
        swapchain->SubmitCommandBuffers();
    }

    void MainVulkanRenderer::InitializeOffscreenRendering()
    {
        // Set what UI panels to use
        PushUIPanel<RendererViewportPanel>(*this);
        PushUIPanel<PropertiesPanel>();
        PushUIPanel<HierarchyPanel>();
        PushUIPanel<DebugPanel>(*this);
        PushUIPanel<DetailedDebugPanel>(*this);
        PushUIPanel<GamePadDebugPanel>();

        CreateSceneRenderingObjects();
        CreateSkyboxRenderingObjects();

        CreateOffscreenDescriptorSets();
        CreateTimestampQueries();
    }

    void MainVulkanRenderer::CreateSkyboxRenderingObjects()
    {
        // Create descriptor set layout for skybox pipeline
        skyboxDescriptorSetLayout = DescriptorSetLayout::Builder()
            .SetShaderStages(ShaderType::VERTEX | ShaderType::FRAGMENT)
            .AddBinding(0, DescriptorType::UNIFORM_BUFFER)
            .AddBinding(1, DescriptorType::COMBINED_IMAGE_SAMPLER)
        .Build();

        // Create shaders for skybox pipeline
        auto skyboxVertexShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Skybox.vert.spv", .shaderType = ShaderType::VERTEX });
        auto skyboxFragmentShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Skybox.frag.spv", .shaderType = ShaderType::FRAGMENT });

        // Create pipeline
        skyboxPipeline = SkyboxPipeline::CreateFromAnotherPipeline({
            .maxConcurrentFrames = swapchain->GetMaxConcurrentFramesCount(),
            .shaders = {skyboxVertexShader, skyboxFragmentShader },
            .descriptorSetLayout = skyboxDescriptorSetLayout,
            .vertexAttributes = { VertexAttribute::POSITION },
            .renderPass = sceneOffscreenRenderer->GetRenderPass(),
            .sampling = Sampling::MSAAx1,
            .cullMode = CullMode::FRONT,
            .shadingType = ShadingType::FILL,
        }, scenePipeline, PIPELINE_COPY_OP_UNIFORM_BUFFER);

        std::vector<VertexP> cubeVertices =
        {
            { Vector3(-1, -1, -1) },
            { Vector3( 1, -1, -1) },
            { Vector3( 1,  1, -1) },
            { Vector3(-1,  1, -1) },
            { Vector3(-1, -1,  1) },
            { Vector3( 1, -1,  1) },
            { Vector3( 1,  1,  1) },
            { Vector3(-1,  1,  1) }
        };

        std::vector<uint> cubeIndices =
        {
            1, 5, 0, 0, 5, 4,
            6, 2, 7, 7, 2, 3,
            3, 0, 7, 7, 0, 4,
            7, 4, 6, 6, 4, 5,
            6, 5, 2, 2, 5, 1,
            2, 1, 3, 3, 1, 0
        };

        // Create cube vertex buffer
        skyboxMesh = std::make_unique<Mesh>(cubeVertices, cubeIndices);

        // Add skybox cubemap to its pipeline's descriptor set
        for (const auto &descriptorSet : skyboxPipeline->GetDescriptorSets())
        {
            descriptorSet->WriteCubemap(1, World::GetManager().GetSkyboxSystem().skyboxCubemap);
            descriptorSet->Allocate();
        }
    }

    void MainVulkanRenderer::CreateSceneRenderingObjects()
    {
        // Create descriptor set layout
        auto descriptorSetBuilder = DescriptorSetLayout::Builder();
        descriptorSetBuilder.SetShaderStages(ShaderType::VERTEX | ShaderType::FRAGMENT);
        descriptorSetBuilder.AddBinding(UNIFORM_BUFFER_BINDING, DescriptorType::UNIFORM_BUFFER);
        descriptorSetBuilder.AddBinding(STORAGE_BUFFER_BINDING, DescriptorType::STORAGE_BUFFER);

        if (VK::GetDevice()->GetDescriptorIndexingSupported())
        {
            // TODO: BINDLESS
            // descriptorSetBuilder.AddBinding(BINDLESS_TEXTURE_BINDING, DescriptorType::COMBINED_IMAGE_SAMPLER, VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT, MAX_TEXTURES);
        }
        else
        {
            descriptorSetBuilder.AddBinding(DIFFUSE_TEXTURE_BINDING, DescriptorType::COMBINED_IMAGE_SAMPLER);
            descriptorSetBuilder.AddBinding(SPECULAR_TEXTURE_BINDING, DescriptorType::COMBINED_IMAGE_SAMPLER);
            descriptorSetBuilder.AddBinding(NORMAL_MAP_TEXTURE_BINDING, DescriptorType::COMBINED_IMAGE_SAMPLER);
            descriptorSetBuilder.AddBinding(HEIGHT_MAP_TEXTURE_BINDING, DescriptorType::COMBINED_IMAGE_SAMPLER);
        }

        // Create descriptor set layout
        sceneDescriptorSetLayout = descriptorSetBuilder.Build();
        VK::SetGlobalDescriptorSetLayout(sceneDescriptorSetLayout);

        // Create renderer
        sceneOffscreenRenderer = OffscreenRenderer::Create({
            .width = swapchain->GetWidth(),
            .height = swapchain->GetHeight(),
            .maxConcurrentFrames = maxConcurrentFrames,
            .attachmentTypes = ATTACHMENT_COLOR | ATTACHMENT_DEPTH,
            .msaaSampling = Sampling::MSAAx1
        });

        // Create descriptor sets to rendered images
        offscreenImageDescriptorSets.resize(maxConcurrentFrames);

        // Create shaders for scene pipeline
        auto sceneVertexShader = Shader::Create({ .filePath = VK::GetDevice()->GetDescriptorIndexingSupported() ? File::OUTPUT_FOLDER_PATH + "Shaders/BlinnPhong.vert.spv" : File::OUTPUT_FOLDER_PATH + "Shaders/BlinnPhong.vert.spv", .shaderType = ShaderType::VERTEX });
        auto sceneFragmentShader = Shader::Create({ .filePath = VK::GetDevice()->GetDescriptorIndexingSupported() ? File::OUTPUT_FOLDER_PATH + "Shaders/BlinnPhong.frag.spv" : File::OUTPUT_FOLDER_PATH + "Shaders/BlinnPhong.frag.spv", .shaderType = ShaderType::FRAGMENT });

        // Create pipeline
        scenePipeline = ScenePipeline::Create({
            .maxConcurrentFrames = swapchain->GetMaxConcurrentFramesCount(),
            .vertexAttributes = { VertexAttribute::POSITION, VertexAttribute::NORMAL, VertexAttribute::TEXTURE_COORDINATE },
            .shaders = {sceneVertexShader, sceneFragmentShader },
            .descriptorSetLayout = sceneDescriptorSetLayout,
            .renderPass = sceneOffscreenRenderer->GetRenderPass(),
            .sampling = sampling,
            .shadingType = shadingType
        });
    }

    void MainVulkanRenderer::CreateTimestampQueries()
    {
        // Create timestamp queries
        offscreenTimestampQueries.resize(maxConcurrentFrames);
        for (uint i = 0; i < maxConcurrentFrames; i++)
        {
            offscreenTimestampQueries[i] = TimestampQuery::Create();
        }
    }

    void MainVulkanRenderer::CreateOffscreenDescriptorSets()
    {
        // Create descriptor sets to rendered images
        offscreenImageDescriptorSets.resize(maxConcurrentFrames);
        for (uint i = maxConcurrentFrames; i--;)
        {
            offscreenImageDescriptorSets[i] = ImGui_ImplVulkan_AddTexture(sceneOffscreenRenderer->GetSampler()->GetVulkanSampler(), sceneOffscreenRenderer->GetColorImage(i)->GetVulkanImageView(), VK_IMAGE_LAYOUT_GENERAL);
        }
    }

    /* --- DESTRUCTOR --- */

    void MainVulkanRenderer::TerminateOffscreenRendering()
    {
        skyboxMesh->Destroy();

        scenePipeline->Destroy();
        sceneOffscreenRenderer->Destroy();
        sceneDescriptorSetLayout->Destroy();

        skyboxPipeline->Destroy();
        skyboxDescriptorSetLayout->Destroy();
    }

    void MainVulkanRenderer::Destroy()
    {
        VK::GetDevice()->WaitUntilIdle();

        TerminateOffscreenRendering();

        VulkanRenderer::Destroy();
    }

}
