//
// Created by Nikolay Kanchevski on 5.02.23.
//

#include "DeferredVulkanRenderer.h"

#include "../../../EngineCore.h"
#include "../RenderingUtilities.h"
#include "../../Math/MatrixUtilities.h"
#include "../../../../Engine/Classes/Time.h"
#include "../../../../Engine/Classes/Input.h"
#include "../../../../Engine/Components/Camera.h"
#include "../../../../Engine/Components/MeshRenderer.h"
#include "../../../../Engine/Components/WorldManager.h"

#define POSITION_BUFFER_BINDING 2
#define DIFFUSE_BUFFER_BINDING 3
#define SPECULAR_AND_SHININESS_BUFFER_BINDING 4
#define NORMAL_BUFFER_BINDING 5
#define DEPTH_BUFFER_BINDING 6
#define SKYBOX_CUBEMAP_BINDING 1

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
        CreateSkyboxRenderingObjects();
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
            .format = ImageFormat::R32_UINT,
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
        depthStencilBuffer->TransitionLayout(ImageLayout::DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL);

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
        bufferRenderer = DynamicRenderer::Create({
            .attachments = defferedDynamicAttachments
        });

        mergingRenderer = DynamicRenderer::Create({
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
    }

    void DeferredVulkanRenderer::CreateSceneRenderingObjects()
    {
        // Create descriptor set layout builder
        auto descriptorLayoutBuilder = DescriptorSetLayout::Builder();
        descriptorLayoutBuilder.SetShaderStages(ShaderType::ALL);
        descriptorLayoutBuilder.AddBinding(UNIFORM_BUFFER_BINDING, DescriptorType::UNIFORM_BUFFER);
        descriptorLayoutBuilder.AddBinding(STORAGE_BUFFER_BINDING, DescriptorType::STORAGE_BUFFER);

        if (VK::GetDevice()->GetDescriptorIndexingSupported())
        {
            // TODO: BINDLESS
            // descriptorSetBuilder.AddBinding(BINDLESS_TEXTURE_BINDING, DescriptorType::COMBINED_IMAGE_SAMPLER, VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT, MAX_TEXTURES);
        }
        else
        {
            descriptorLayoutBuilder.AddBinding(DIFFUSE_TEXTURE_BINDING, DescriptorType::COMBINED_IMAGE_SAMPLER);
            descriptorLayoutBuilder.AddBinding(SPECULAR_TEXTURE_BINDING, DescriptorType::COMBINED_IMAGE_SAMPLER);
            descriptorLayoutBuilder.AddBinding(NORMAL_MAP_TEXTURE_BINDING, DescriptorType::COMBINED_IMAGE_SAMPLER);
            descriptorLayoutBuilder.AddBinding(HEIGHT_MAP_TEXTURE_BINDING, DescriptorType::COMBINED_IMAGE_SAMPLER);
        }

        // Create scene descriptor set layout
        bufferDescriptorSetLayout = descriptorLayoutBuilder.Build();
        VK::SetGlobalDescriptorSetLayout(bufferDescriptorSetLayout);

        // Load scene shaders
        // TODO: RENAME SHADERS
        auto vertexShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Deferred/DeferredSubpass_0.vert" });
        auto fragmentShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Deferred/DeferredSubpass_0.frag" });

        // Create scene renderer pipeline
        bufferPipeline = ScenePipeline::Create({
            .maxConcurrentFrames = maxConcurrentFrames,
            .shaders = { vertexShader, fragmentShader },
            .attachments = { { IDBuffer }, { positionBuffer }, { diffuseBuffer }, { specularAndShininessBuffer }, { normalBuffer }, { depthStencilBuffer } },
            .vertexAttributes = { VertexAttribute::POSITION, VertexAttribute::NORMAL, VertexAttribute::TEXTURE_COORDINATE },
            .descriptorSetLayout = bufferDescriptorSetLayout,
            .sampling = Sampling::MSAAx1,
            .shadingType = ShadingType::FILL
        });

        // Set up merging descriptor layout
        mergingDescriptorSetLayout = DescriptorSetLayout::Builder()
            .SetShaderStages(ShaderType::FRAGMENT)
            .AddBinding(STORAGE_BUFFER_BINDING, DescriptorType::STORAGE_BUFFER)
            .AddBinding(POSITION_BUFFER_BINDING, DescriptorType::COMBINED_IMAGE_SAMPLER)
            .AddBinding(DIFFUSE_BUFFER_BINDING, DescriptorType::COMBINED_IMAGE_SAMPLER)
            .AddBinding(SPECULAR_AND_SHININESS_BUFFER_BINDING, DescriptorType::COMBINED_IMAGE_SAMPLER)
            .AddBinding(NORMAL_BUFFER_BINDING, DescriptorType::COMBINED_IMAGE_SAMPLER)
            .AddBinding(DEPTH_BUFFER_BINDING, DescriptorType::COMBINED_IMAGE_SAMPLER)
        .Build();

        // Load merging shaders
        vertexShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Deferred/DeferredSubpass_1.vert", .shaderType = ShaderType::VERTEX });
        fragmentShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Deferred/DeferredSubpass_1.frag", .shaderType = ShaderType::FRAGMENT });

        // Create scene renderer pipeline
        mergingPipeline = MergingPipeline::CreateFromAnotherPipeline({
            .maxConcurrentFrames = maxConcurrentFrames,
            .shaders = { vertexShader, fragmentShader },
            .attachments = { { renderedImage } },
            .descriptorSetLayout = mergingDescriptorSetLayout,
            .sampling = Sampling::MSAAx1,
            .shadingType = ShadingType::FILL
        }, bufferPipeline, PipelineCopyOp::STORAGE_BUFFER);

        // TODO: Special write method
        for (const auto &mergingDescriptorSet : mergingPipeline->GetDescriptorSets())
        {
            mergingDescriptorSet->WriteImage(POSITION_BUFFER_BINDING, positionBuffer, textureSampler);
            mergingDescriptorSet->WriteImage(POSITION_BUFFER_BINDING, positionBuffer, textureSampler);
            mergingDescriptorSet->WriteImage(DIFFUSE_BUFFER_BINDING, diffuseBuffer, textureSampler);
            mergingDescriptorSet->WriteImage(SPECULAR_AND_SHININESS_BUFFER_BINDING, specularAndShininessBuffer, textureSampler);
            mergingDescriptorSet->WriteImage(NORMAL_BUFFER_BINDING, normalBuffer, textureSampler);
            mergingDescriptorSet->WriteImage(DEPTH_BUFFER_BINDING, depthStencilBuffer, textureSampler);
            mergingDescriptorSet->Allocate();
        }

        // Create descriptor sets to rendered images
        renderedImageDescriptorSets.resize(maxConcurrentFrames);
        for (uint i = maxConcurrentFrames; i--;)
        {
            renderedImageDescriptorSets[i] = ImGui_ImplVulkan_AddTexture(textureSampler->GetVulkanSampler(), renderedImage->GetVulkanImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }

//        RenderingUtilities::Initialize(IDBuffer, depthStencilBuffer, scenePipeline);
    }

    void DeferredVulkanRenderer::CreateSkyboxRenderingObjects()
    {
//        // Create descriptor set layout for skybox pipeline
//        skyboxDescriptorSetLayout = DescriptorSetLayout::Builder()
//            .SetShaderStages(ShaderType::VERTEX | ShaderType::FRAGMENT)
//            .AddBinding(UNIFORM_BUFFER_BINDING, DescriptorType::UNIFORM_BUFFER)
//            .AddBinding(SKYBOX_CUBEMAP_BINDING, DescriptorType::COMBINED_IMAGE_SAMPLER)
//        .Build();
//
//        // Create shaders for skybox pipeline
//        auto skyboxVertexShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Deferred/DeferredSubpass_2.vert", .shaderType = ShaderType::VERTEX });
//        auto skyboxFragmentShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Deferred/DeferredSubpass_2.frag", .shaderType = ShaderType::FRAGMENT });
//
//        // Create pipeline
//        skyboxPipeline = SkyboxPipeline::CreateFromAnotherPipeline({
//            .maxConcurrentFrames = swapchain->GetMaxConcurrentFramesCount(),
//            .shaders = { skyboxVertexShader, skyboxFragmentShader },
//            // TODO: PASS DYNAMIC RENDERER INSTEAD OF ATTACHMENTS
//            .attachments = { { IDBuffer }, { positionBuffer }, { diffuseBuffer }, { specularAndShininessBuffer }, { normalBuffer }, { depthStencilBuffer } },
//            .vertexAttributes = { VertexAttribute::POSITION },
//            .descriptorSetLayout = skyboxDescriptorSetLayout,
//            .sampling = Sampling::MSAAx1,
//            .cullMode = CullMode::FRONT,
//            .shadingType = ShadingType::FILL,
//        }, bufferPipeline, PipelineCopyOp::UNIFORM_BUFFER);
//
//        std::vector<VertexP> cubeVertices =
//        {
//            { Vector3(-1, -1, -1) },
//            { Vector3( 1, -1, -1) },
//            { Vector3( 1,  1, -1) },
//            { Vector3(-1,  1, -1) },
//            { Vector3(-1, -1,  1) },
//            { Vector3( 1, -1,  1) },
//            { Vector3( 1,  1,  1) },
//            { Vector3(-1,  1,  1) }
//        };
//
//        std::vector<uint> cubeIndices =
//        {
//            1, 5, 0, 0, 5, 4,
//            6, 2, 7, 7, 2, 3,
//            3, 0, 7, 7, 0, 4,
//            7, 4, 6, 6, 4, 5,
//            6, 5, 2, 2, 5, 1,
//            2, 1, 3, 3, 1, 0
//        };
//
//        // Create cube vertex buffer
//        skyboxMesh = std::make_unique<Mesh>(cubeVertices, cubeIndices);
//
//        // Add skybox cubemap to its pipeline's descriptor set
//        for (const auto &descriptorSet : skyboxPipeline->GetDescriptorSets())
//        {
//            descriptorSet->WriteCubemap(SKYBOX_CUBEMAP_BINDING, World::GetManager().GetSkyboxSystem().skyboxCubemap);
//            descriptorSet->Allocate();
//        }
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

        float timeAngle = std::fmod(Time::GetUpTime(), 360.0f) * 0.8f;
//        skyboxPipeline->GetPushConstantData().model = MatrixUtilities::CreateModel({ 0, 0, 0 }, { timeAngle, 0.0f, 0.0f });
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
                mergingPipeline->GetPushConstantData().renderedImageValue = (RenderedImageValue) currentRenderedImageValue;
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
        commandBuffer->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        // Start GPU timer
        renderTimestampQueries[currentFrame]->Begin(commandBuffer);

        commandBuffer->TransitionImageLayouts({ { positionBuffer }, { diffuseBuffer }, { specularAndShininessBuffer }, { normalBuffer }, { renderedImage }, }, ImageLayout::COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

        bufferRenderer->Begin(commandBuffer);

        bufferPipeline->Bind(commandBuffer);

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
            bufferPipeline->GetPushConstantData() = meshRenderer.GetPushConstantData();

            // Send push constant data to shader
            bufferPipeline->PushConstants(commandBuffer);

            // Use either bindless or bind*full* descriptor set if not supported
            bufferPipeline->BindDescriptorSets(
                commandBuffer, meshRenderer.GetDescriptorSet()
            );

            // Draw using the index buffer to prevent vertex re-usage
            commandBuffer->DrawIndexed(meshRenderer.GetMesh()->GetIndexCount());
        }

//        skyboxPipeline->Bind(commandBuffer);
//
//        commandBuffer->BindVertexBuffers({ skyboxMesh->GetVertexBuffer()->GetVulkanBuffer() });
//        commandBuffer->BindIndexBuffer({ skyboxMesh->GetIndexBuffer()->GetVulkanBuffer() });
//
//        skyboxPipeline->PushConstants(commandBuffer);
//        skyboxPipeline->BindDescriptorSets(commandBuffer);
//
//        commandBuffer->DrawIndexed(skyboxMesh->GetIndexCount());

        bufferRenderer->End(commandBuffer);

        commandBuffer->TransitionImageLayouts({ { positionBuffer }, { diffuseBuffer }, { specularAndShininessBuffer }, { normalBuffer } }, ImageLayout::SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

        mergingRenderer->Begin(commandBuffer);

        mergingPipeline->Bind(commandBuffer);

        mergingPipeline->BindDescriptorSets(commandBuffer);

        mergingPipeline->PushConstants(commandBuffer);

        commandBuffer->Draw(6);

        mergingRenderer->End(commandBuffer);

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

        // TODO: Update ID buffer here

        // End and save GPU timer results
        renderTimestampQueries[currentFrame]->End(commandBuffer);
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

//        RenderingUtilities::Destroy();

        textureSampler->Destroy();

//        skyboxMesh->Destroy();
//        skyboxPipeline->Destroy();
//        skyboxDescriptorSetLayout->Destroy();

        mergingPipeline->Destroy();
        mergingDescriptorSetLayout->Destroy();

        mergingRenderer->Destroy();
        renderedImage->Destroy();

        bufferPipeline->Destroy();
        bufferDescriptorSetLayout->Destroy();

        bufferRenderer->Destroy();
        depthStencilBuffer->Destroy();
        normalBuffer->Destroy();
        specularAndShininessBuffer->Destroy();
        diffuseBuffer->Destroy();
        positionBuffer->Destroy();
        IDBuffer->Destroy();

        VulkanRenderer::Destroy();
    }

}