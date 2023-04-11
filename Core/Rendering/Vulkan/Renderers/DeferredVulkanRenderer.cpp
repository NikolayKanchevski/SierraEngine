//
// Created by Nikolay Kanchevski on 5.02.23.
//

#include "DeferredVulkanRenderer.h"

#include "../../../EngineCore.h"
#include "../../Math/MatrixUtilities.h"
#include "../../../../Engine/Classes/Time.h"
#include "../../../../Engine/Classes/Input.h"
#include "../../../../Engine/Components/Camera.h"
#include "../../../../Engine/Components/MeshRenderer.h"

#define DIFFUSE_BUFFER_BINDING 2
#define SPECULAR_SHININESS_AND_SHADOW_BUFFER_BINDING 3
#define NORMAL_BUFFER_BINDING 4
#define SHADOW_MAP_BUFFER_BINDING 5
#define DEPTH_BUFFER_BINDING 6
#define SKYBOX_BUFFER_BINDING 7

#define USE_THREADED_FOR_EACH

#ifdef USE_THREADED_FOR_EACH
    #define FOR_EACH_LOOP tbb::parallel_for_each
#else
    #define FOR_EACH_LOOP std::for_each
#endif

namespace Sierra::Core::Rendering::Vulkan::Renderers
{

    /* --- CONSTRUCTORS --- */

    DeferredVulkanRenderer::DeferredVulkanRenderer(const VulkanRendererCreateInfo &createInfo)
        : VulkanRenderer(createInfo)
    {
        PROFILE_FUNCTION();

        InitializeRenderer();
    }

    void DeferredVulkanRenderer::InitializeRenderer()
    {
        // Set what UI panels to use
        PushUIPanel<RendererViewportPanel>(*this);
        PushUIPanel<PropertiesPanel>();
        PushUIPanel<HierarchyPanel>();
        PushUIPanel<DebugPanel>(*this);
        PushUIPanel<DetailedDebugPanel>(*this);
        PushUIPanel<GamePadDebugPanel>();

        // Create texture sampler to use when passing data to shaders
        bufferSampler = Sampler::Create({ .enableAnisotropy = false, .applyBilinearFiltering = true });

        // Create entity ID buffer image
        IDBuffer = Image::Create({
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight() },
            .format = ImageFormat::R16_UINT,
            .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED
        });

        // Create diffuse image
        diffuseBuffer = Image::Create({
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight() },
            .format = ImageFormat::R16G16B16A16_SFLOAT,
            .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED
        });

        // Create specular and shininess image
        specularAndShininessBuffer = Image::Create({
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight() },
            .format = ImageFormat::R8G8_UNORM,
            .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED
        });

        // Create normals data image
        normalBuffer = Image::Create({
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight() },
            .format = ImageFormat::R16G16B16A16_SFLOAT,
            .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED
        });

        // Create depth stencil image
        depthStencilBuffer = Image::Create({
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight() },
            .format = VK::GetDevice()->GetBestDepthImageFormat(),
            .usage = ImageUsage::DEPTH_STENCIL_ATTACHMENT | ImageUsage::SAMPLED
        });

        // Create final rendered image
        renderedImage = Image::Create({
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight() },
            .format = ImageFormat::R16G16B16A16_SFLOAT,
            .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED
        });

        // Create dynamic renderers
        bufferPass = DynamicRenderer::Create({
            .attachments = {
                // Entity ID buffer attachment [0]
                {
                    .image = IDBuffer,
                    .loadOp = LoadOp::CLEAR,
                    .storeOp = StoreOp::STORE,
                },

                // Color buffer attachment [2]
                {
                    .image = diffuseBuffer,
                    .loadOp = LoadOp::CLEAR,
                    .storeOp = StoreOp::STORE,
                },

                // Specular and shininess buffer attachment [3]
                {
                    .image = specularAndShininessBuffer,
                    .loadOp = LoadOp::CLEAR,
                    .storeOp = StoreOp::STORE,
                },

                // Normal buffer attachment [4]
                {
                    .image = normalBuffer,
                    .loadOp = LoadOp::CLEAR,
                    .storeOp = StoreOp::STORE,
                },

                // Depth stencil buffer attachment [5]
                {
                    .image = depthStencilBuffer,
                    .loadOp = LoadOp::CLEAR,
                    .storeOp = StoreOp::STORE,
                }
            }
        });

        // Create composition renderer
        compositionPass = DynamicRenderer::Create({
            .attachments = {
                {
                    .image = renderedImage,
                    .loadOp = LoadOp::CLEAR,
                    .storeOp = StoreOp::STORE
                }
            }
        });

        // Create timestamp queries
        renderTimestampQueries.resize(maxConcurrentFrames);
        for (uint i = 0; i < maxConcurrentFrames; i++)
        {
            renderTimestampQueries[i] = TimestampQuery::Create();
        }

        // Load scene shaders
        auto vertexShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Deferred/G-Buffer.vert" });
        auto fragmentShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Deferred/G-Buffer.frag" });

        // Create scene renderer pipeline
        bufferPipeline = ScenePipeline::Create({
            .shaders = { vertexShader, fragmentShader },
            .dynamicRenderingInfo = GraphicsPipelineDynamicRenderingInfo {
                .attachments = {{ IDBuffer }, { diffuseBuffer }, { specularAndShininessBuffer }, { normalBuffer }, { depthStencilBuffer } }
            },
            .shadingType = ShadingType::FILL
        });

        // Set global layout
        VK::SetGlobalDescriptorSetLayout(bufferPipeline->GetDescriptorSetLayout());

        // Load merging shaders
        vertexShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Deferred/Composition.vert", .shaderType = ShaderType::VERTEX });
        fragmentShader = Shader::Create({
            .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Deferred/Composition.frag",
            .shaderType = ShaderType::FRAGMENT,
            .definitions = {
                { .name = "SETTINGS_SHADING_MODEL_BLINN_PHONG" },
//                { .name = "SETTINGS_USE_PCF_SHADOWS" },
                { .name = "SETTINGS_USE_POISSON_PCF_SHADOWS" },
//                { .name = "SETTINGS_USE_RANDOM_SAMPLE_FOR_POISSON" }
                { .name = "SETTINGS_USE_GRADIENT_SAMPLING_FOR_POISSON" },
//                { .name = "SETTINGS_USE_VARIANCE_SHADOWS" },
//                { .name = "SETTINGS_USE_MOMENT_SHADOWS" },
            }
        });

        // Create scene renderer pipeline
        compositionPipeline = CompositionPipeline::CreateFromAnotherPipeline({
            .shaders = { vertexShader, fragmentShader },
            .dynamicRenderingInfo = GraphicsPipelineDynamicRenderingInfo {
                .attachments = { { renderedImage } }
            },
            .shadingType = ShadingType::FILL
        }, bufferPipeline, PipelineCopyOp::UNIFORM_BUFFERS | PipelineCopyOp::STORAGE_BUFFERS);

        // Crete skybox cubemap
        skyboxCubemap = Cubemap::Create({ .filePaths = {
            File::OUTPUT_FOLDER_PATH + "Textures/Skyboxes/Default/skybox_right.png",
            File::OUTPUT_FOLDER_PATH + "Textures/Skyboxes/Default/skybox_left.png",
            File::OUTPUT_FOLDER_PATH + "Textures/Skyboxes/Default/skybox_top.png",
            File::OUTPUT_FOLDER_PATH + "Textures/Skyboxes/Default/skybox_bottom.png",
            File::OUTPUT_FOLDER_PATH + "Textures/Skyboxes/Default/skybox_front.png",
            File::OUTPUT_FOLDER_PATH + "Textures/Skyboxes/Default/skybox_back.png",
        }, .cubemapType = CubemapType::SKYBOX });

        // Create shadow renderer
        shadowRenderer = RenderingUtilities::CreateShadowMapRenderer(bufferPipeline);

        for (const auto &descriptorSet : compositionPipeline->GetDescriptorSets())
        {
            descriptorSet
                ->WriteImage(DIFFUSE_BUFFER_BINDING, diffuseBuffer, bufferSampler)
                ->WriteImage(SPECULAR_SHININESS_AND_SHADOW_BUFFER_BINDING, specularAndShininessBuffer, bufferSampler)
                ->WriteImage(NORMAL_BUFFER_BINDING, normalBuffer, bufferSampler)
                ->WriteImage(SHADOW_MAP_BUFFER_BINDING, shadowRenderer->GetShadowMap(), shadowRenderer->GetShadowMapSampler())
                ->WriteImage(DEPTH_BUFFER_BINDING, depthStencilBuffer, bufferSampler)
                ->WriteCubemap(SKYBOX_BUFFER_BINDING, skyboxCubemap)
            ->Allocate();
        }

        // Create descriptor sets to rendered images
        renderedImageDescriptorSets.resize(maxConcurrentFrames);
        for (uint i = maxConcurrentFrames; i--;)
        {
            renderedImageDescriptorSets[i] = ImGui_ImplVulkan_AddTexture(bufferSampler->GetVulkanSampler(), renderedImage->GetVulkanImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }

        // Create modules
        raycaster = RenderingUtilities::CreateRaycaster(IDBuffer, diffuseBuffer, bufferPipeline);
    }

    UniquePtr<DeferredVulkanRenderer> DeferredVulkanRenderer::Create(const VulkanRendererCreateInfo createInfo)
    {
        return std::make_unique<DeferredVulkanRenderer>(createInfo);
    }

    /* --- POLLING METHODS --- */

    void DeferredVulkanRenderer::Update()
    {
        // Update camera
        Camera &camera = Camera::GetMainCamera();

        // Update uniform data
        auto &sceneUniformData = bufferPipeline->GetUniformBufferData();
        sceneUniformData.view = camera.GetViewMatrix();
        sceneUniformData.projection = camera.GetProjectionMatrix();
        sceneUniformData.inverseView = camera.GetInverseViewMatrix();
        sceneUniformData.inverseProjection = camera.GetInverseProjectionMatrix();

        // Update storage data
        auto &storageData = bufferPipeline->GetStorageBufferData();
        storageData.directionalLightCount = DirectionalLight::GetDirectionalLightCount();
        storageData.pointLightCount = PointLight::GetPointLightCount();

        auto enttMeshView = World::GetAllComponentsOfType<MeshRenderer>();
        FOR_EACH_LOOP(enttMeshView.begin(), enttMeshView.end(), [&enttMeshView, &storageData](auto &enttEntity)
        {
            MeshRenderer &meshRenderer = enttMeshView.get<MeshRenderer>(enttEntity);
            storageData.objectDatas[meshRenderer.GetMeshID()].model = meshRenderer.GetModelMatrix();
        });

        auto enttDirectionalLightView = World::GetAllComponentsOfType<DirectionalLight>();
        FOR_EACH_LOOP(enttDirectionalLightView.begin(), enttDirectionalLightView.end(), [&enttDirectionalLightView, &storageData](auto &enttEntity)
        {
            DirectionalLight &directionalLight = enttDirectionalLightView.get<DirectionalLight>(enttEntity);
            storageData.directionalLights[directionalLight.GetID()] = directionalLight;
        });

        auto enttPointLightView = World::GetAllComponentsOfType<PointLight>();
        FOR_EACH_LOOP(enttPointLightView.begin(), enttPointLightView.end(), [&enttPointLightView, &storageData](auto &enttEntity)
        {
            PointLight &pointLight = enttPointLightView.get<PointLight>(enttEntity);
            storageData.pointLights[pointLight.GetID()] = pointLight;
        });

        // Rotate skybox
        float timeAngle = std::fmod(Time::GetUpTime(), 360.0f) * 0.8f;
        compositionPipeline->GetPushConstantData().skyboxModel = MatrixUtilities::CreateModelMatrix({ 0.0f, 0.0f, 0.0f }, { timeAngle, 0.0f, 0.0f });
        compositionPipeline->GetPushConstantData().lightSpaceMatrix = World::GetComponent<DirectionalLight>(World::GetAllComponentsOfType<DirectionalLight>()[0]).GetViewSpaceMatrix();

        shadowRenderer->Update();
    }

    void DeferredVulkanRenderer::DrawUI()
    {
        ViewportPanel().DrawUI();

        GUI::BeginWindow("Renderer's Properties", nullptr, ImGuiWindowFlags_NoNav);

        // Rendered image type dropdown
        {
            static uint currentRenderedImageValue = RenderedImageValue::RenderedImage;
            static const char* renderedImageValueTypes[] = {"Final Render", "Position Buffer", "Diffuse Buffer",
                                                            "Specular Buffer", "Shininess Buffer", "Normal Buffer",
                                                            "Shadow Buffer", "Depth Buffer"};

            GUI::CustomLabel("Renderer Image Output:");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            if (GUI::Dropdown("##DEFFERED_VULKAN_RENDERER_RENDERED_IMGAGE_TYPE_DROPDOWN", currentRenderedImageValue,
                              renderedImageValueTypes, 8)) {
                compositionPipeline->GetPushConstantData().renderedImageValue = (RenderedImageValue) currentRenderedImageValue;
            }
        }

        // Shading technique dropdown
        {
            static uint currentShadingTechniqueIndex = 1;
            static uint previouseShadingTechniqueIndex = currentShadingTechniqueIndex;
            static const char* shadingTechniques[] = { "Phong", "Blinn-Phong", "Labmertian", "Gaussian" };

            static std::unordered_map<uint, const char*> shadingTechniqueDictionary =
            {
                { 0, "SETTINGS_SHADING_MODEL_PHONG" },
                { 1, "SETTINGS_SHADING_MODEL_BLINN_PHONG" },
                { 2, "SETTINGS_SHADING_MODEL_LAMBERTIAN" },
                { 3, "SETTINGS_SHADING_MODEL_GAUSSIAN" }
            };

            GUI::CustomLabel("Renderer Shading Technique:");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            if (GUI::Dropdown("##DEFFERED_VULKAN_RENDERER_SHADING_TECHNIQUE_DROPDOWN", currentShadingTechniqueIndex, shadingTechniques, 4))
            {
                VK::GetDevice()->WaitUntilIdle();

                compositionPipeline->ClearShaderDefinitionForShader(ShaderType::FRAGMENT, shadingTechniqueDictionary[previouseShadingTechniqueIndex]);
                compositionPipeline->SetShaderDefinition(ShaderType::FRAGMENT, { .name = shadingTechniqueDictionary[currentShadingTechniqueIndex], .value = "1" });

                previouseShadingTechniqueIndex = currentShadingTechniqueIndex;
            }
        }

        // Shading type dropdown
        {
            static uint currentShadingTypeIndex = 0;
            static const char* shadingTypes[] = {"Shaded", "Wireframe"};

            GUI::CustomLabel("Renderer Shading:");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            if (GUI::Dropdown("##DEFFERED_VULKAN_RENDERER_SHADING_TYPE_DROPDOWN", currentShadingTypeIndex, shadingTypes, 2)) {
                VK::GetDevice()->WaitUntilIdle();

                bufferPipeline->GetCreateInfo().shadingType = (ShadingType) currentShadingTypeIndex;
                bufferPipeline->Recreate();
            }
        }

        // Shadow Settings
        {
            static bool enableShadows = true;

            MergingRendererPushConstant &pushConstant = compositionPipeline->GetPushConstantData();
            if (GUI::Checkbox("Enable Shadows:", enableShadows))
            {
                pushConstant.enableShadows = enableShadows;
            }
        }

        GUI::EndWindow();

//        GUI::BeginWindow();
//
//        static auto a = ImGui_ImplVulkan_AddTexture(shadowRenderer->GetShadowMapSampler()->GetVulkanSampler(), shadowRenderer->GetShadowMap()->GetVulkanImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
//        ImGui::Image((ImTextureID) a, { 512, 512 }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
//
//        GUI::EndWindow();

        VulkanRenderer::DrawUI();
    }

    void DeferredVulkanRenderer::Render()
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
        renderTimestampQueries[currentFrame]->Begin(commandBuffer);

        // Render shadows
        if (compositionPipeline->GetPushConstantData().enableShadows) shadowRenderer->Render(commandBuffer);

        // Begin rendering
        bufferPass->Begin(commandBuffer);

        // Bind the pipeline which will draw to the G-Buffer
        bufferPipeline->Bind(commandBuffer);

        // For each mesh in the world
        auto enttMeshView = World::GetAllComponentsOfType<MeshRenderer>();
        for (auto enttEntity : enttMeshView)
        {
            // Bind mesh
            auto &meshRenderer = enttMeshView.get<MeshRenderer>(enttEntity);

            // Copy push constant data to pipeline
            bufferPipeline->GetPushConstantData() = meshRenderer.GetPushConstantData();
            bufferPipeline->GetPushConstantData().directionalLightID = 0;

            // Send push constant data to shader
            bufferPipeline->PushConstants(commandBuffer);

            // Use either bindless or bind*full* descriptor set if not supported
            bufferPipeline->BindDescriptorSets(commandBuffer, meshRenderer.GetDescriptorSet());

            // Draw mesh
            bufferPipeline->DrawMesh(commandBuffer, meshRenderer.GetMesh());
        }

        // End rendering to G-Buffer
        bufferPass->End(commandBuffer);

        // Transition images' layouts to be suitable for reading from in shaders
        commandBuffer->TransitionImageLayouts({ { IDBuffer }, { diffuseBuffer }, { specularAndShininessBuffer }, { normalBuffer }, { depthStencilBuffer }  }, ImageLayout::SHADER_READ_ONLY_OPTIMAL);

        // Begin combining renderer
        compositionPass->Begin(commandBuffer);

        // Bind the pipeline which will be combining the G-Buffer data into single image
        compositionPipeline->Bind(commandBuffer);

        // Update pipeline's shaders' data
        compositionPipeline->BindDescriptorSets(commandBuffer);
        compositionPipeline->PushConstants(commandBuffer);

        // Draw a fullscreen triangle (with 3 vertices) and the skybox cube, which consists of 36 more
        compositionPipeline->Draw(commandBuffer, 39);

        // End rendering
        compositionPass->End(commandBuffer);

        commandBuffer->TransitionImageLayouts({ { IDBuffer }, { diffuseBuffer }, { specularAndShininessBuffer }, { normalBuffer }, { depthStencilBuffer }  }, ImageLayout::SHADER_READ_ONLY_OPTIMAL);


        // End and save GPU timer results
        raycaster->UpdateData(commandBuffer);

        // Update ID buffer on CPU side
        renderTimestampQueries[currentFrame]->End(commandBuffer);

        // Handle viewport clicking
        static auto sceneWindowHash = ImHashStr("Scene View");
        if (Input::GetMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT) && !ImGuizmo::IsOver())
        {
            entt::entity hoveredEntity = raycaster->GetHoveredEntityID();
            if (hoveredEntity == entt::null)
            {
                if (GImGui->HoveredWindow != nullptr && GImGui->HoveredWindow->ID == sceneWindowHash)
                {
                    EngineCore::SetSelectedEntity(hoveredEntity);
                }
            }
            else
            {
                EngineCore::SetSelectedEntity(hoveredEntity);
            }

            EngineCore::SetMouseHoveredPosition(raycaster->GetHoveredWorldPosition());
        }

        // Transition rendered image's layout to be suitable for reading in ImGui shaders
        commandBuffer->TransitionImageLayout(renderedImage, ImageLayout::SHADER_READ_ONLY_OPTIMAL);

        // Begin the render pass
        swapchain->BeginRenderPass(commandBuffer);

        // Render ImGui UI
        totalVerticesDrawn = Mesh::GetTotalVertexCount();
        if (ImGui::GetDrawData() != nullptr)
        {
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer->GetVulkanCommandBuffer());
            totalVerticesDrawn += ImGui::GetDrawData()->TotalVtxCount;
        }

        // End the render pass
        swapchain->EndRenderPass(commandBuffer);
        this->totalDrawTime = renderTimestampQueries[currentFrame]->GetTimeTaken();

        // End the command buffer and check for errors during command execution
        commandBuffer->End();

        // Render to swapchain
        swapchain->SubmitCommandBuffers();
    }

    /* --- DESTRUCTOR --- */

    void DeferredVulkanRenderer::Destroy()
    {
        VK::GetDevice()->WaitUntilIdle();

        raycaster->Destroy();
        shadowRenderer->Destroy();

        skyboxCubemap->Destroy();
        bufferSampler->Destroy();

        compositionPipeline->Destroy();
        compositionPass->Destroy();
        renderedImage->Destroy();

        bufferPipeline->Destroy();
        bufferPass->Destroy();
        depthStencilBuffer->Destroy();
        normalBuffer->Destroy();
        specularAndShininessBuffer->Destroy();
        diffuseBuffer->Destroy();
        IDBuffer->Destroy();

        VulkanRenderer::Destroy();
    }

}