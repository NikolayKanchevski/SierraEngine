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

#define POSITION_BUFFER_BINDING 2
#define DIFFUSE_BUFFER_BINDING 3
#define SPECULAR_AND_SHININESS_BUFFER_BINDING 4
#define NORMAL_BUFFER_BINDING 5
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
        CreateSceneRenderingObjects();
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
        textureSampler = Sampler::Create({ .maxAnisotropy = 1.0f, .applyBilinearFiltering = true });

        // Create entity ID buffer image
        IDBuffer = Image::Create({
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight(), 1 },
            .format = ImageFormat::R16_UINT,
            .sampling = Sampling::MSAAx1,
            .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED
        });

        IDBuffer->CreateImageView(ImageAspectFlags::COLOR);

        // Create (world) position buffer image
        positionBuffer = Image::Create({
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight(), 1 },
            .format = ImageFormat::R16G16B16A16_SFLOAT,
            .sampling = Sampling::MSAAx1,
            .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED
        });

        positionBuffer->CreateImageView(ImageAspectFlags::COLOR);

        // Create diffuse image
        diffuseBuffer = Image::Create({
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight(), 1 },
            .format = ImageFormat::R16G16B16A16_SFLOAT,
            .sampling = Sampling::MSAAx1,
            .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED
        });

        diffuseBuffer->CreateImageView(ImageAspectFlags::COLOR);

        // Create specular and shininess image
        specularAndShininessBuffer = Image::Create({
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight(), 1 },
            .format = ImageFormat::R8G8_UNORM,
            .sampling = Sampling::MSAAx1,
            .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED
        });

        specularAndShininessBuffer->CreateImageView(ImageAspectFlags::COLOR);

        // Create normals data image
        normalBuffer = Image::Create({
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight(), 1 },
            .format = ImageFormat::R16G16B16A16_SFLOAT,
            .sampling = Sampling::MSAAx1,
            .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED
        });

        normalBuffer->CreateImageView(ImageAspectFlags::COLOR);

        // Create depth stencil image
        depthStencilBuffer = Image::Create({
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight(), 1 },
            .format = VK::GetDevice()->GetBestDepthImageFormat(),
            .sampling = Sampling::MSAAx1,
            .usage = ImageUsage::DEPTH_STENCIL_ATTACHMENT | ImageUsage::SAMPLED
        });

        depthStencilBuffer->CreateImageView(ImageAspectFlags::DEPTH);

        // Create final rendered image
        renderedImage = Image::Create({
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight(), 1 },
            .format = ImageFormat::R16G16B16A16_SFLOAT,
            .sampling = Sampling::MSAAx1,
            .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED
        });

        renderedImage->CreateImageView(ImageAspectFlags::COLOR);

        // Set up rendering attachments
        const std::vector<DynamicRendererAttachment> defferedDynamicAttachments
        {
            // Entity ID buffer attachment [0]
            {
                .image = IDBuffer,
                .loadOp = LoadOp::CLEAR,
                .storeOp = StoreOp::STORE,
            },

            // Position buffer attachment [1]
            {
                .image = positionBuffer,
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
        };

        // Create dynamic renderers
        bufferPass = DynamicRenderer::Create({
            .attachments = defferedDynamicAttachments
        });

        // Create composition renderer
        compositionPass = DynamicRenderer::Create({
            .attachments = {
                {
                    .image = renderedImage,
                    .loadOp = LoadOp::CLEAR,
                    .storeOp = StoreOp::STORE
                },
                {
                    .image = depthStencilBuffer,
                    .loadOp = LoadOp::LOAD,
                    .storeOp = StoreOp::DONT_CARE
                }
            }
        });

        // Create timestamp queries
        renderTimestampQueries.resize(maxConcurrentFrames);
        for (uint i = 0; i < maxConcurrentFrames; i++)
        {
            renderTimestampQueries[i] = TimestampQuery::Create();
        }
    }

    void DeferredVulkanRenderer::CreateSceneRenderingObjects()
    {
        // Load scene shaders
        auto vertexShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Deferred/G-Buffer.vert" });
        auto fragmentShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Deferred/G-Buffer.frag" });

        // Create scene renderer pipeline
        bufferPipeline = ScenePipeline::Create({
            .maxConcurrentFrames = maxConcurrentFrames,
            .shaders = { vertexShader, fragmentShader },
            .dynamicRenderingInfo = GraphicsPipelineDynamicRenderingInfo {
                .attachments = { { IDBuffer }, { positionBuffer }, { diffuseBuffer }, { specularAndShininessBuffer }, { normalBuffer }, { depthStencilBuffer } }
            },
            .sampling = Sampling::MSAAx1,
            .shadingType = ShadingType::FILL
        });

        VK::SetGlobalDescriptorSetLayout(bufferPipeline->GetDescriptorSetLayout());

        // Load merging shaders
        vertexShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Deferred/Composition.vert", .shaderType = ShaderType::VERTEX });
        fragmentShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Deferred/Composition.frag", .shaderType = ShaderType::FRAGMENT });

        // Create scene renderer pipeline
        compositionPipeline = CompositionPipeline::CreateFromAnotherPipeline({
            .maxConcurrentFrames = maxConcurrentFrames,
            .shaders = { vertexShader, fragmentShader },
            .dynamicRenderingInfo = GraphicsPipelineDynamicRenderingInfo {
                .attachments = { { renderedImage }, { depthStencilBuffer } }
            },
            .sampling = Sampling::MSAAx1,
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

        // Write external data to pipeline
        for (const auto &descriptorSet : compositionPipeline->GetDescriptorSets())
        {
            descriptorSet
                ->WriteImage(POSITION_BUFFER_BINDING, positionBuffer, textureSampler)
                ->WriteImage(POSITION_BUFFER_BINDING, positionBuffer, textureSampler)
                ->WriteImage(DIFFUSE_BUFFER_BINDING, diffuseBuffer, textureSampler)
                ->WriteImage(SPECULAR_AND_SHININESS_BUFFER_BINDING, specularAndShininessBuffer, textureSampler)
                ->WriteImage(NORMAL_BUFFER_BINDING, normalBuffer, textureSampler)
                ->WriteImage(DEPTH_BUFFER_BINDING, depthStencilBuffer, textureSampler)
                ->WriteCubemap(SKYBOX_BUFFER_BINDING, skyboxCubemap)
            ->Allocate();
        }

        // Create descriptor sets to rendered images
        renderedImageDescriptorSets.resize(maxConcurrentFrames);
        for (uint i = maxConcurrentFrames; i--;)
        {
            renderedImageDescriptorSets[i] = ImGui_ImplVulkan_AddTexture(textureSampler->GetVulkanSampler(), renderedImage->GetVulkanImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }

        raycaster = RenderingUtilities::CreateRaycaster(IDBuffer, depthStencilBuffer, bufferPipeline);
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
    }

    void DeferredVulkanRenderer::DrawUI()
    {
        ViewportPanel().DrawUI();

        GUI::BeginWindow("Renderer's Properties", nullptr, ImGuiWindowFlags_NoNav);

        // Rendered image type dropdown
        {
            static uint currentRenderedImageValue = RenderedImageValue::RenderedImage;
            static const char *renderedImageValueTypes[] = {"Final Render", "Position Buffer", "Diffuse Buffer",
                                                            "Specular Buffer", "Shininess Buffer", "Normal Buffer",
                                                            "Depth Buffer"};

            GUI::CustomLabel("Renderer Image Output:");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            if (GUI::Dropdown("##DEFFERED_VULKAN_RENDERER_RENDERED_IMGAGE_TYPE_DROPDOWN", currentRenderedImageValue,
                              renderedImageValueTypes, 7)) {
                compositionPipeline->GetPushConstantData().renderedImageValue = (RenderedImageValue) currentRenderedImageValue;
            }
        }

        // Shading type dropdown
        {
            static uint currentShadingTypeIndex = 0;
            static const char *shadingTypes[] = {"Shaded", "Wireframe"};

            GUI::CustomLabel("Renderer Shading:");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            if (GUI::Dropdown("##DEFFERED_VULKAN_RENDERER_SHADING_TYPE_DROPDOWN", currentShadingTypeIndex,
                              shadingTypes, 2)) {
                VK::GetDevice()->WaitUntilIdle();

                bufferPipeline->GetCreateInfo().shadingType = (ShadingType) currentShadingTypeIndex;
                bufferPipeline->Recreate();
            }
        }

        GUI::EndWindow();

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

        // Transition images' layouts to be suitable for drawing to
        commandBuffer->TransitionImageLayout(depthStencilBuffer, ImageLayout::DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
        commandBuffer->TransitionImageLayouts({ { IDBuffer }, { positionBuffer }, { diffuseBuffer }, { specularAndShininessBuffer }, { normalBuffer }, { renderedImage } }, ImageLayout::COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

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
            meshRenderer.GetMesh()->Bind(commandBuffer);

            // Copy push constant data to pipeline
            bufferPipeline->GetPushConstantData() = meshRenderer.GetPushConstantData();

            // Send push constant data to shader
            bufferPipeline->PushConstants(commandBuffer);

            // Use either bindless or bind*full* descriptor set if not supported
            bufferPipeline->BindDescriptorSets(commandBuffer, meshRenderer.GetDescriptorSet());

            // Draw using the index buffer to prevent vertex re-usage
            commandBuffer->DrawIndexed(meshRenderer.GetMesh()->GetIndexCount());
        }

        // End rendering to G-Buffer
        bufferPass->End(commandBuffer);

        // Transition images' layouts to be suitable for reading from in shaders
        commandBuffer->TransitionImageLayout(depthStencilBuffer, ImageLayout::SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
        commandBuffer->TransitionImageLayouts({ { IDBuffer }, { positionBuffer }, { diffuseBuffer }, { specularAndShininessBuffer }, { normalBuffer }  }, ImageLayout::SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

        // Begin combining renderer
        compositionPass->Begin(commandBuffer);

        // Bind the pipeline which will be combining the G-Buffer data into single image
        compositionPipeline->Bind(commandBuffer);

        // Update pipeline's shaders' data
        compositionPipeline->BindDescriptorSets(commandBuffer);
        compositionPipeline->PushConstants(commandBuffer);

        // Draw a fullscreen quad (2x triangles each with 3 vertices + skybox cube which consists of 36 more)
        commandBuffer->Draw(42);

        // End rendering
        compositionPass->End(commandBuffer);

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
                if (GImGui->HoveredWindow->ID == sceneWindowHash)
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
        commandBuffer->TransitionImageLayout(renderedImage, ImageLayout::SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

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

        skyboxCubemap->Destroy();
        textureSampler->Destroy();

        compositionPipeline->Destroy();
        compositionPass->Destroy();
        renderedImage->Destroy();

        bufferPipeline->Destroy();
        bufferPass->Destroy();
        depthStencilBuffer->Destroy();
        normalBuffer->Destroy();
        specularAndShininessBuffer->Destroy();
        diffuseBuffer->Destroy();
        positionBuffer->Destroy();
        IDBuffer->Destroy();

        VulkanRenderer::Destroy();
    }

}