//
// Created by Nikolay Kanchevski on 5.02.23.
//

#include "DeferredVulkanRenderer.h"

#include "../VK.h"
#include "../../../../Engine/Classes/Time.h"
#include "../../../../Engine/Classes/Input.h"
#include "../../../../Engine/Components/Camera.h"
#include "../../../../Engine/Components/MeshRenderer.h"
#include "../../../EngineCore.h"

#define DIFFUSE_BUFFER_BINDING 2
#define SS_BUFFER_BINDING 3          // Specular & Shininess
#define NORMAL_BUFFER_BINDING 4
#define SHADOW_MAP_BUFFER_BINDING 5 // TODO: UNITE INTO SSS ^
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

        // Create buffers
        uniformBuffers.resize(maxConcurrentFrames);
        storageBuffers.resize(maxConcurrentFrames);
        for (uint i = maxConcurrentFrames; i--;)
        {
            uniformBuffers[i] = Buffer::Create({
                .memorySize = sizeof(UniformData),
                .bufferUsage = BufferUsage::UNIFORM
            });

            storageBuffers[i] = Buffer::Create({
                .memorySize = sizeof(StorageData),
                .bufferUsage = BufferUsage::STORAGE
            });
        }

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

        // Create texture sampler to use when passing data to shaders
        bufferSampler = Sampler::Create({ .enableAnisotropy = false, .applyBilinearFiltering = true });

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

        // Create descriptor set layout for G-Buffer pipeline
        bufferDescriptorSetLayout = DescriptorSetLayout::Create({
            .bindings = {
            { UNIFORM_BUFFER_BINDING,      { .descriptorType = DescriptorType::UNIFORM_BUFFER,         .shaderStages = ShaderType::VERTEX   } },
            { STORAGE_BUFFER_BINDING,      { .descriptorType = DescriptorType::STORAGE_BUFFER,         .shaderStages = ShaderType::VERTEX   } },
            { DIFFUSE_TEXTURE_BINDING,     { .descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER, .shaderStages = ShaderType::FRAGMENT } },
            { SPECULAR_TEXTURE_BINDING,    { .descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER, .shaderStages = ShaderType::FRAGMENT } },
            { NORMAL_TEXTURE_BINDING,      { .descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER, .shaderStages = ShaderType::FRAGMENT } },
            { HEIGHT_TEXTURE_BINDING,      { .descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER, .shaderStages = ShaderType::VERTEX   } }
            },
            .flags = DescriptorSetLayoutFlag::PUSH_DESCRIPTOR_KHR
        });

        // Load G-Buffer (scene) shaders
        auto vertexShader = Shader::Load({
            .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Deferred/G-Buffer.vert.spv",
            .shaderType = ShaderType::VERTEX,
            .vertexAttributes = {
                VertexAttributeType::POSITION,
                VertexAttributeType::NORMAL,
                VertexAttributeType::UV
            }
        });
        auto fragmentShader = Shader::Load({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Deferred/G-Buffer.frag.spv", .shaderType = ShaderType::FRAGMENT });

        // Create scene renderer pipeline
        bufferPipeline = GraphicsPipeline::Create({
            .shaders = { vertexShader, fragmentShader },
            .shaderInfo = CompiledPipelineShaderInfo {
                .pushConstantData = PushConstantData {
                    .size = sizeof(MeshPushConstant),
                    .shaderStages = ShaderType::VERTEX | ShaderType::FRAGMENT
                },
                .descriptorSetLayout = &bufferDescriptorSetLayout
            },
            .dynamicRenderingInfo = GraphicsPipelineDynamicRenderingInfo {
                .dynamicRenderer = bufferPass
            }
        });

        // Crete skybox cubemap
        skyboxCubemap = Cubemap::Create({ .filePaths = {
            File::OUTPUT_FOLDER_PATH + "Textures/Skyboxes/Default/skybox_right.png",
            File::OUTPUT_FOLDER_PATH + "Textures/Skyboxes/Default/skybox_left.png",
            File::OUTPUT_FOLDER_PATH + "Textures/Skyboxes/Default/skybox_top.png",
            File::OUTPUT_FOLDER_PATH + "Textures/Skyboxes/Default/skybox_bottom.png",
            File::OUTPUT_FOLDER_PATH + "Textures/Skyboxes/Default/skybox_front.png",
            File::OUTPUT_FOLDER_PATH + "Textures/Skyboxes/Default/skybox_back.png",
        }, .cubemapType = CubemapType::SKYBOX });

        // Create final rendered image
        renderedImage = Image::Create({
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight() },
            .format = ImageFormat::R16G16B16A16_SFLOAT,
            .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED
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

        // Create set layout for merging pipeline
        compositionDescriptorSetLayout = DescriptorSetLayout::Create({
            .bindings = {
            { UNIFORM_BUFFER_BINDING,    { .descriptorType = DescriptorType::UNIFORM_BUFFER,         .shaderStages = ShaderType::VERTEX | ShaderType::FRAGMENT } },
            { STORAGE_BUFFER_BINDING,    { .descriptorType = DescriptorType::STORAGE_BUFFER,         .shaderStages = ShaderType::FRAGMENT                      } },
            { DIFFUSE_BUFFER_BINDING,    { .descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER, .shaderStages = ShaderType::FRAGMENT                      } },
            { SS_BUFFER_BINDING,         { .descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER, .shaderStages = ShaderType::FRAGMENT                      } },
            { NORMAL_BUFFER_BINDING,     { .descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER, .shaderStages = ShaderType::FRAGMENT                      } },
            { SHADOW_MAP_BUFFER_BINDING, { .descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER, .shaderStages = ShaderType::FRAGMENT                      } },
            { DEPTH_BUFFER_BINDING,      { .descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER, .shaderStages = ShaderType::FRAGMENT                      } },
            { SKYBOX_BUFFER_BINDING,     { .descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER, .shaderStages = ShaderType::FRAGMENT                      } }
            },
            .flags = DescriptorSetLayoutFlag::PUSH_DESCRIPTOR_KHR
        });

        // Load merging shaders
        vertexShader = Shader::Load({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Deferred/Composition.vert.spv", .shaderType = ShaderType::VERTEX });
        fragmentShader = Shader::Load({
            .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Deferred/Composition.frag.spv",
            .shaderType = ShaderType::FRAGMENT,
            .specializationConstants = {
                { 0, { .size = UINT_SIZE } },
                { 1, { .size = UINT_SIZE } }
            }
        });

        // Create scene renderer pipeline
        compositionPipeline = GraphicsPipeline::Create({
            .shaders = { vertexShader, fragmentShader },
            .shaderInfo = CompiledPipelineShaderInfo {
                .pushConstantData = PushConstantData {
                    .size = sizeof(CompositionPushConstant),
                    .shaderStages = ShaderType::VERTEX
                },
                .descriptorSetLayout = &compositionDescriptorSetLayout
            },
            .dynamicRenderingInfo = GraphicsPipelineDynamicRenderingInfo {
                .dynamicRenderer = compositionPass
            },
            .shadingType = ShadingType::FILL
        });

        // Create descriptor sets for rendered images & create timestamp queries
        renderTimestampQueries.resize(maxConcurrentFrames);
        renderedImageDescriptorSets.resize(maxConcurrentFrames);
        for (uint i = maxConcurrentFrames; i--;)
        {
            renderTimestampQueries[i] = TimestampQuery::Create();
            renderedImageDescriptorSets[i] = ImGui_ImplVulkan_AddTexture(bufferSampler->GetVulkanSampler(), renderedImage->GetVulkanImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }

        // Create modules
        raycaster = Raycaster::Create({ .IDBuffer = IDBuffer, .depthBuffer = depthStencilBuffer });
    }

    UniquePtr<DeferredVulkanRenderer> DeferredVulkanRenderer::Create(const VulkanRendererCreateInfo createInfo)
    {
        return std::make_unique<DeferredVulkanRenderer>(createInfo);
    }

    /* --- POLLING METHODS --- */

    void DeferredVulkanRenderer::Update()
    {
        // Get current frame index
        uint currentFrame = swapchain->GetCurrentFrameIndex();

        // Update camera
        Camera &camera = Camera::GetMainCamera();

        // Update uniform data
        auto &sceneUniformData = *uniformBuffers[currentFrame]->GetDataAs<UniformData>();
        sceneUniformData.view = camera.GetViewMatrix();
        sceneUniformData.projection = camera.GetProjectionMatrix();
        sceneUniformData.inverseView = camera.GetInverseViewMatrix();
        sceneUniformData.inverseProjection = camera.GetInverseProjectionMatrix();
        sceneUniformData.nearPlane = camera.GetNearClip();
        sceneUniformData.farPlane = camera.GetFarClip();

        // Update storage data
        auto &storageData = *storageBuffers[currentFrame]->GetDataAs<StorageData>();
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
        compositionPushConstantData.skyboxModel = MatrixUtilities::CreateModelMatrix({ 0.0f, 0.0f, 0.0f }, { timeAngle, 0.0f, 0.0f });
    }

    void DeferredVulkanRenderer::DrawUI()
    {
        ViewportPanel().DrawUI();

        GUI::BeginWindow("Renderer's Properties", nullptr, ImGuiWindowFlags_NoNav);

        // Rendered image type dropdown
        {
            static uint currentRenderedImageValue = RendererOutput::RenderedImage;
            static const char* renderedImageValueTypes[] = {"Final Render", "Position Buffer", "Diffuse Buffer",
                                                            "Specular Buffer", "Shininess Buffer", "Normal Buffer",
                                                            "Shadow Buffer", "Depth Buffer" };

            GUI::CustomLabel("Renderer Image Output:");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            if (GUI::Dropdown("##DEFFERED_VULKAN_RENDERER_RENDERED_IMGAGE_TYPE_DROPDOWN", currentRenderedImageValue,renderedImageValueTypes, 8))
            {
                compositionPipeline->SetSpecializationConstant(0, currentRenderedImageValue);
            }
        }

        // Shading type dropdown
        {
            static uint currentShadingTypeIndex = 0;
            static const char* shadingTypes[] = {"Shaded", "Wireframe"};

            GUI::CustomLabel("Renderer Shading:");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            if (GUI::Dropdown("##DEFFERED_VULKAN_RENDERER_SHADING_TYPE_DROPDOWN", currentShadingTypeIndex, shadingTypes, 2))
            {
                bufferPipeline->SetShadingType(static_cast<ShadingType>(currentShadingTypeIndex));
            }
        }

        // Shadow Settings
        {
            static bool enableShadows = 1;
            if (GUI::Checkbox("Enable Shadows:", enableShadows))
            {
                uint value = static_cast<uint>(enableShadows);
                compositionPipeline->SetSpecializationConstant(1, value);
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
        commandBuffer->Begin(CommandBufferUsage::ONE_TIME_SUBMIT);

        // Start GPU timer
        renderTimestampQueries[currentFrame]->Begin(commandBuffer);

        // Begin rendering
        bufferPass->Begin(commandBuffer);

        // Bind the pipeline which will draw to the G-Buffer
        bufferPipeline->Bind(commandBuffer);

        // Set global buffer data
        bufferPipeline->SetShaderBinding(UNIFORM_BUFFER_BINDING, uniformBuffers[currentFrame]);
        bufferPipeline->SetShaderBinding(STORAGE_BUFFER_BINDING, storageBuffers[currentFrame]);

        // For each mesh in the world
        auto enttMeshView = World::GetAllComponentsOfType<MeshRenderer>();
        for (auto enttEntity : enttMeshView)
        {
            // Bind mesh
            auto &meshRenderer = enttMeshView.get<MeshRenderer>(enttEntity);

            // Send push constant data to shader
            auto pushConstantData = meshRenderer.GetPushConstantData();
            bufferPipeline->SetPushConstants(commandBuffer, pushConstantData);

            // Set mesh's textures
            for (uint i = static_cast<uint>(TextureType::TOTAL_COUNT); i--;)
            {
                bufferPipeline->SetShaderBinding(TEXTURE_TYPE_TO_BINDING(i), meshRenderer.GetTexture(static_cast<TextureType>(i)));
            }

            // Draw mesh
            bufferPipeline->DrawMesh(commandBuffer, meshRenderer.GetMesh());
        }

        // End rendering to G-Buffer
        bufferPass->End(commandBuffer);

        // End G-Buffer pipeline
        bufferPipeline->End(commandBuffer);

        // Transition images' layouts to be suitable for reading from in shaders
        commandBuffer->TransitionImageLayouts({ IDBuffer, diffuseBuffer, specularAndShininessBuffer, normalBuffer, depthStencilBuffer }, ImageLayout::SHADER_READ_ONLY_OPTIMAL);

        // Begin combining renderer
        compositionPass->Begin(commandBuffer);

        // Bind the pipeline which will be combining the G-Buffer data into single image
        compositionPipeline->Bind(commandBuffer);

        // Update pipeline's shaders' data
        compositionPipeline->SetShaderBinding(UNIFORM_BUFFER_BINDING, uniformBuffers[currentFrame]);
        compositionPipeline->SetShaderBinding(STORAGE_BUFFER_BINDING, storageBuffers[currentFrame]);
        compositionPipeline->SetShaderBinding(DIFFUSE_BUFFER_BINDING, diffuseBuffer, bufferSampler);
        compositionPipeline->SetShaderBinding(SS_BUFFER_BINDING, specularAndShininessBuffer, bufferSampler);
        compositionPipeline->SetShaderBinding(NORMAL_BUFFER_BINDING, normalBuffer, bufferSampler);
        compositionPipeline->SetShaderBinding(SHADOW_MAP_BUFFER_BINDING, normalBuffer, bufferSampler);
        compositionPipeline->SetShaderBinding(DEPTH_BUFFER_BINDING, depthStencilBuffer, bufferSampler);
        compositionPipeline->SetShaderBinding(SKYBOX_BUFFER_BINDING, skyboxCubemap);
        compositionPipeline->SetPushConstants(commandBuffer, compositionPushConstantData);

        // Draw a fullscreen triangle (with 3 vertices) and the skybox cube, which consists of 36 more
        compositionPipeline->Draw(commandBuffer, 39);

        // End rendering
        compositionPass->End(commandBuffer);

        // End composition pipeline
        compositionPipeline->End(commandBuffer);

        // Update ID buffer on CPU side
        raycaster->UpdateData(commandBuffer, uniformBuffers[currentFrame]);

        // End and save GPU timer results
        renderTimestampQueries[currentFrame]->End(commandBuffer);

        // Transition rendered image's layout to be suitable for reading in ImGui shaders
        commandBuffer->TransitionImageLayout(renderedImage, ImageLayout::SHADER_READ_ONLY_OPTIMAL);

        // Handle viewport clicking
        static auto sceneWindowHash = ImHashStr("Scene View");
        if (Input::GetMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT) && !ImGuizmo::IsOver())
        {
            // Update selected entity
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
        }

        // Update hovered world position
        EngineCore::SetMouseHoveredPosition(raycaster->GetHoveredPosition());

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
        totalDrawTime = renderTimestampQueries[currentFrame]->GetTimeTaken();

        // End the command buffer and check for errors during command execution
        commandBuffer->End();

        // Render to swapchain
        swapchain->SwapImage();
    }

    /* --- DESTRUCTOR --- */

    void DeferredVulkanRenderer::Destroy()
    {
        VK::GetDevice()->WaitUntilIdle();

        raycaster->Destroy();

        compositionPipeline->Destroy();
        compositionPass->Destroy();
        compositionDescriptorSetLayout->Destroy();

        skyboxCubemap->Destroy();
        renderedImage->Destroy();

        bufferPipeline->Destroy();
        bufferPass->Destroy();
        bufferDescriptorSetLayout->Destroy();

        depthStencilBuffer->Destroy();
        normalBuffer->Destroy();
        specularAndShininessBuffer->Destroy();
        diffuseBuffer->Destroy();
        IDBuffer->Destroy();
        bufferSampler->Destroy();

        for (uint i = maxConcurrentFrames; i--;)
        {
            uniformBuffers[i]->Destroy();
            storageBuffers[i]->Destroy();
        }

        VulkanRenderer::Destroy();
    }

}