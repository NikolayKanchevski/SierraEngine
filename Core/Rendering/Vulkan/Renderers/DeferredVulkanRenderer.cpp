//
// Created by Nikolay Kanchevski on 5.02.23.
//

#include "DeferredVulkanRenderer.h"

#include "../../Math/MatrixUtilities.h"
#include "../../../../Engine/Components/Camera.h"
#include "../../../../Engine/Components/MeshRenderer.h"
#include "../../../../Engine/Components/WorldManager.h"
#include "../../../../Engine/Classes/Time.h"

#define POSITION_BUFFER_BINDING 2
#define COLOR_BUFFER_BINDING 3
#define SPECULAR_AND_SHININESS_BUFFER_BINDING 4
#define NORMAL_BUFFER_BINDING 5
#define DEPTH_BUFFER_BINDING 6
#define SKYBOX_CUBEMAP_BINDING 1

#define G_BUFFER_IMAGE_FORMAT ImageFormat::R16G16B16A16_SFLOAT

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
        // Create texture sampler to use when passing data to shaders
        textureSampler = Sampler::Create({ .maxAnisotropy = 1.0f, .applyBilinearFiltering = true });

        // Create (world) position buffer image
        positionBuffer = Image::Create({
            .format = G_BUFFER_IMAGE_FORMAT,
            .sampling = Sampling::MSAAx1,
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight(), 1 },
            .usageFlags = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED | ImageUsage::INPUT_ATTACHMENT
        });

        positionBuffer->CreateImageView(ImageAspectFlags::COLOR);

        // Create diffuse image
        diffuseBuffer = Image::Create({
            .format = G_BUFFER_IMAGE_FORMAT,
            .sampling = Sampling::MSAAx1,
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight(), 1 },
            .usageFlags = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED | ImageUsage::INPUT_ATTACHMENT
        });

        diffuseBuffer->CreateImageView(ImageAspectFlags::COLOR);

        // Create specular and shininess image
        specularAndShininessBuffer = Image::Create({
            .format = ImageFormat::R8G8_UNORM,
            .sampling = Sampling::MSAAx1,
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight(), 1 },
            .usageFlags = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED | ImageUsage::INPUT_ATTACHMENT
        });

        specularAndShininessBuffer->CreateImageView(ImageAspectFlags::COLOR);

        // Create normals data image
        normalBuffer = Image::Create({
            .format = G_BUFFER_IMAGE_FORMAT,
            .sampling = Sampling::MSAAx1,
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight(), 1 },
            .usageFlags = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED | ImageUsage::INPUT_ATTACHMENT
        });

        normalBuffer->CreateImageView(ImageAspectFlags::COLOR);

        // Create depth stencil image
        depthStencilBuffer = Image::Create({
            .format = VK::GetDevice()->GetBestDepthImageFormat(),
            .sampling = Sampling::MSAAx1,
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight(), 1 },
            .usageFlags = ImageUsage::DEPTH_STENCIL_ATTACHMENT | ImageUsage::INPUT_ATTACHMENT
        });

        depthStencilBuffer->CreateImageView(ImageAspectFlags::DEPTH);
        depthStencilBuffer->TransitionLayout(ImageLayout::DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL);

        renderedImage = Image::Create({
            .format = G_BUFFER_IMAGE_FORMAT,
            .sampling = Sampling::MSAAx1,
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight(), 1 },
            .usageFlags = ImageUsage::COLOR_ATTACHMENT | ImageUsage::INPUT_ATTACHMENT | ImageUsage::SAMPLED
        });

        renderedImage->CreateImageView(ImageAspectFlags::COLOR);

        // Set up render pass attachments
        const std::vector<RenderPassAttachment> defferedRenderPassAttachments
        {
            // Position buffer attachment
            {
                .imageAttachment = positionBuffer,
                .loadOp = LoadOp::CLEAR,
                .storeOp = StoreOp::STORE,
                .finalLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL
            },

            // Color buffer attachment
            {
                .imageAttachment = diffuseBuffer,
                .loadOp = LoadOp::CLEAR,
                .storeOp = StoreOp::STORE,
                .finalLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL
            },

            // Specular and shininess buffer attachment
            {
                .imageAttachment = specularAndShininessBuffer,
                .loadOp = LoadOp::CLEAR,
                .storeOp = StoreOp::STORE,
                .finalLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL
            },

            // Normal buffer attachment
            {
                .imageAttachment = normalBuffer,
                .loadOp = LoadOp::CLEAR,
                .storeOp = StoreOp::STORE,
                .finalLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL
            },

            // Depth stencil buffer attachment
            {
                .imageAttachment = depthStencilBuffer,
                .loadOp = LoadOp::CLEAR,
                .storeOp = StoreOp::STORE,
                .finalLayout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            },

            // Finalized/rendered image attachment
            {
                .imageAttachment = renderedImage,
                .loadOp = LoadOp::CLEAR,
                .storeOp = StoreOp::STORE,
                .finalLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL
            },
        };

        // Create render pass
        deferredRenderPass = RenderPass::Create({
            .attachments = defferedRenderPassAttachments,
            .subpassInfos = {
                // Subpass 0
                {
                    .renderTargets = { 0, 1, 2, 3, 4 }
                },
                // Subpass 1
                {
                    .renderTargets = { 5 },
                    .subpassInputs = { 0, 1, 2, 3, 4 }
                },
                // Subpass 2
                {
                    .renderTargets = { 5 },
                    .subpassInputs = { 4, 5 }
                }
            }
        });

        // Create framebuffer
        deferredFramebuffer = Framebuffer::Create({
            .width = swapchain->GetWidth(),
            .height = swapchain->GetHeight(),
            .attachments = {
                &positionBuffer,
                &diffuseBuffer,
                &specularAndShininessBuffer,
                &normalBuffer,
                &depthStencilBuffer,
                &renderedImage
            },
            .renderPass = deferredRenderPass->GetVulkanRenderPass()
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
        sceneDescriptorSetLayout = descriptorLayoutBuilder.Build();
        VK::m_Instance.descriptorSetLayout = sceneDescriptorSetLayout;

        // Load scene shaders
        auto vertexShader = Shader::Create({ .filePath = "Shaders/deferred_subpass_0.vert.spv", .shaderType = ShaderType::VERTEX });
        auto fragmentShader = Shader::Create({ .filePath = "Shaders/deferred_subpass_0.frag.spv", .shaderType = ShaderType::FRAGMENT });

        // Create scene renderer pipeline
        scenePipeline = ScenePipeline::Create({
            .maxConcurrentFrames = maxConcurrentFrames,
            .sampling = Sampling::MSAAx1,
            .vertexAttributes = { VertexAttribute::POSITION, VertexAttribute::NORMAL, VertexAttribute::TEXTURE_COORDINATE },
            .renderPass = deferredRenderPass,
            .subpasss = 0,
            .descriptorSetLayout = sceneDescriptorSetLayout,
            .shaders = { vertexShader, fragmentShader },
            .shadingType = ShadingType::FILL,
            .colorAttachmentCount = 4,
            .createDepthBuffer = true
        });

        // Set up merging descriptor layout
        descriptorLayoutBuilder = DescriptorSetLayout::Builder();
        descriptorLayoutBuilder.SetShaderStages(ShaderType::FRAGMENT);
        descriptorLayoutBuilder.AddBinding(STORAGE_BUFFER_BINDING, DescriptorType::STORAGE_BUFFER);
        descriptorLayoutBuilder.AddBinding(POSITION_BUFFER_BINDING, DescriptorType::INPUT_ATTACHMENT);
        descriptorLayoutBuilder.AddBinding(COLOR_BUFFER_BINDING, DescriptorType::INPUT_ATTACHMENT);
        descriptorLayoutBuilder.AddBinding(SPECULAR_AND_SHININESS_BUFFER_BINDING, DescriptorType::INPUT_ATTACHMENT);
        descriptorLayoutBuilder.AddBinding(NORMAL_BUFFER_BINDING, DescriptorType::INPUT_ATTACHMENT);
        descriptorLayoutBuilder.AddBinding(DEPTH_BUFFER_BINDING, DescriptorType::INPUT_ATTACHMENT);

        // Creaete merging descriptor set layout
        mergingDescriptorSetLayout = descriptorLayoutBuilder.Build();

        mergingDescriptorSet = DescriptorSet::Build(mergingDescriptorSetLayout);
        mergingDescriptorSet->WriteImage(POSITION_BUFFER_BINDING, positionBuffer, textureSampler);
        mergingDescriptorSet->WriteImage(COLOR_BUFFER_BINDING, diffuseBuffer, textureSampler);
        mergingDescriptorSet->WriteImage(SPECULAR_AND_SHININESS_BUFFER_BINDING, specularAndShininessBuffer, textureSampler);
        mergingDescriptorSet->WriteImage(NORMAL_BUFFER_BINDING, normalBuffer, textureSampler);
        mergingDescriptorSet->WriteImage(DEPTH_BUFFER_BINDING, depthStencilBuffer, textureSampler);
        mergingDescriptorSet->Allocate();

        // Load merging shaders
        vertexShader = Shader::Create({ .filePath = "Shaders/deferred_subpass_1.vert.spv", .shaderType = ShaderType::VERTEX });
        fragmentShader = Shader::Create({ .filePath = "Shaders/deferred_subpass_1.frag.spv", .shaderType = ShaderType::FRAGMENT });

        // Create scene renderer pipeline
        mergingPipeline = MergingPipeline::CreateFromAnotherPipeline({
            .maxConcurrentFrames = maxConcurrentFrames,
            .sampling = Sampling::MSAAx1,
            .vertexAttributes = {  },
            .renderPass = deferredRenderPass,
            .subpasss = 1,
            .descriptorSetLayout = mergingDescriptorSetLayout,
            .shaders = { vertexShader, fragmentShader },
            .shadingType = ShadingType::FILL,
            .colorAttachmentCount = 1,
            .createDepthBuffer = false
        }, scenePipeline, PIPELINE_COPY_OP_STORAGE_BUFFER);

        // Create descriptor sets to rendered images
        renderedImageDescriptorSets.resize(maxConcurrentFrames);
        for (uint i = maxConcurrentFrames; i--;)
        {
            renderedImageDescriptorSets[i] = ImGui_ImplVulkan_AddTexture(textureSampler->GetVulkanSampler(), renderedImage->GetVulkanImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
    }

    void DeferredVulkanRenderer::CreateSkyboxRenderingObjects()
    {
        // Create descriptor set layout for skybox pipeline
        skyboxDescriptorSetLayout = DescriptorSetLayout::Builder()
            .SetShaderStages(ShaderType::VERTEX | ShaderType::FRAGMENT)
            .AddBinding(UNIFORM_BUFFER_BINDING, DescriptorType::UNIFORM_BUFFER)
            .AddBinding(SKYBOX_CUBEMAP_BINDING, DescriptorType::COMBINED_IMAGE_SAMPLER)
            .AddBinding(SKYBOX_CUBEMAP_BINDING + 1, DescriptorType::INPUT_ATTACHMENT, ShaderType::FRAGMENT)
        .Build();

        // Create shaders for skybox pipeline
        auto skyboxVertexShader = Shader::Create({ .filePath = "Shaders/deferred_subpass_2.vert.spv", .shaderType = ShaderType::VERTEX });
        auto skyboxFragmentShader = Shader::Create({ .filePath = "Shaders/deferred_subpass_2.frag.spv", .shaderType = ShaderType::FRAGMENT });

        // Create pipeline
        skyboxPipeline = SkyboxPipeline::CreateFromAnotherPipeline({
            .maxConcurrentFrames = swapchain->GetMaxConcurrentFramesCount(),
            .vertexAttributes = { VertexAttribute::POSITION },
            .shaders = { skyboxVertexShader, skyboxFragmentShader },
            .renderPass = deferredRenderPass,
            .subpasss = 2,
            .descriptorSetLayout = skyboxDescriptorSetLayout,
            .sampling = Sampling::MSAAx1,
            .cullMode = CullMode::FRONT,
            .shadingType = ShadingType::FILL,
            .colorAttachmentCount = 1,
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
            descriptorSet->WriteCubemap(SKYBOX_CUBEMAP_BINDING, World::GetManager().GetSkyboxSystem().skyboxCubemap);
            descriptorSet->WriteImage(SKYBOX_CUBEMAP_BINDING + 1, depthStencilBuffer, textureSampler);
            descriptorSet->Allocate();
        }
    }

    UniquePtr<DeferredVulkanRenderer> DeferredVulkanRenderer::Create(const VulkanRendererCreateInfo createInfo)
    {
        return std::make_unique<DeferredVulkanRenderer>(createInfo);
    }

    /* --- POLLING METHODS --- */

    void DeferredVulkanRenderer::Update()
    {
        // Update camera
        Camera *camera = Camera::GetMainCamera();

        // Update uniform data
        auto &sceneUniformData = scenePipeline->GetUniformBufferData();
        sceneUniformData.view = camera->GetViewMatrix();
        sceneUniformData.projection = camera->GetProjectionMatrix();

        // Update storage data
        auto &storageData = scenePipeline->GetStorageBufferData();
        storageData.directionalLightCount = DirectionalLight::GetDirectionalLightCount();
        storageData.pointLightCount = PointLight::GetPointLightCount();

        auto enttMeshView = World::GetAllComponentsOfType<MeshRenderer>();
        tbb::parallel_for_each(enttMeshView.begin(), enttMeshView.end(),
           [&enttMeshView, &storageData](auto &enttEntity)
           {
               MeshRenderer &meshRenderer = enttMeshView.get<MeshRenderer>(enttEntity);
               storageData.objectDatas[meshRenderer.GetMeshID()].model = meshRenderer.GetModelMatrix();
           }
        );

        auto enttDirectionalLightView = World::GetAllComponentsOfType<DirectionalLight>();
        tbb::parallel_for_each(enttDirectionalLightView.begin(), enttDirectionalLightView.end(),
           [&enttDirectionalLightView, &storageData](auto &enttEntity)
           {
               DirectionalLight &directionalLight = enttDirectionalLightView.get<DirectionalLight>(enttEntity);
               storageData.directionalLights[directionalLight.GetID()] = directionalLight;
           }
        );

        auto enttPointLightView = World::GetAllComponentsOfType<PointLight>();
        tbb::parallel_for_each(enttPointLightView.begin(), enttPointLightView.end(),
           [&enttPointLightView, &storageData](auto &enttEntity)
           {
               PointLight &pointLight = enttPointLightView.get<PointLight>(enttEntity);
               storageData.pointLights[pointLight.GetID()] = pointLight;
           }
        );

        float timeAngle = std::fmod(Time::GetUpTime(), 360.0f) * 0.8f;
        skyboxPipeline->GetPushConstantData().model = MatrixUtilities::CreateModel({ 0, 0, 0 }, { timeAngle, 0.0f, 0.0f });
    }

    void DeferredVulkanRenderer::DrawUI()
    {
        // Rendered image type dropdown
        {
            static uint currentRenderedImageValue = RenderedImageValue::RenderedImage;
            static const char* renderedImageValueTypes[] = { "Final Render", "Position Buffer", "Diffuse Buffer", "Specular Buffer", "Shininess Buffer", "Normal Buffer", "Depth Buffer" };

            ImGui::CustomLabel("Renderer Image Output:");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            if (ImGui::Dropdown("##DEFFERED_VULKAN_RENDERER_RENDERED_IMGAGE_TYPE_DROPDOWN", currentRenderedImageValue, renderedImageValueTypes, 7))
            {
                mergingPipeline->GetPushConstantData().renderedImageValue = (RenderedImageValue) currentRenderedImageValue;
            }
        }

        // Shading type dropdown
        {
            static uint currentShadingTypeIndex = 0;
            static const char* shadingTypes[] = {"Shaded", "Wireframe" };

            ImGui::CustomLabel("Renderer Shading:");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            if (ImGui::Dropdown("##DEFFERED_VULKAN_RENDERER_SHADING_TYPE_DROPDOWN", currentShadingTypeIndex, shadingTypes, 2))
            {
                VK::GetDevice()->WaitUntilIdle();

                scenePipeline->GetCreateInfo().shadingType = (ShadingType) currentShadingTypeIndex;
                scenePipeline->Recreate();
            }
        }
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
        renderTimestampQueries[currentFrame]->Begin(commandBuffer->GetVulkanCommandBuffer());

        scenePipeline->Bind(commandBuffer->GetVulkanCommandBuffer());

        deferredRenderPass->Begin(deferredFramebuffer, commandBuffer->GetVulkanCommandBuffer());
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
            scenePipeline->PushConstants(commandBuffer->GetVulkanCommandBuffer());

            // Use either bindless or bind*full* descriptor set if not supported
            scenePipeline->BindDescriptorSets(
                commandBuffer->GetVulkanCommandBuffer(),{ meshRenderer.GetDescriptorSet() }, currentFrame
            );

            // Draw using the index buffer to prevent vertex re-usage
            commandBuffer->Draw(meshRenderer.GetMesh()->GetIndexCount());
        }

        deferredRenderPass->NextSubpass(commandBuffer->GetVulkanCommandBuffer());

        mergingPipeline->Bind(commandBuffer->GetVulkanCommandBuffer());

        mergingPipeline->BindDescriptorSets(commandBuffer->GetVulkanCommandBuffer(), {mergingDescriptorSet->GetVulkanDescriptorSet() }, currentFrame);

        mergingPipeline->PushConstants(commandBuffer->GetVulkanCommandBuffer());

        commandBuffer->DrawUnindexed(3);

        deferredRenderPass->NextSubpass(commandBuffer->GetVulkanCommandBuffer());

        skyboxPipeline->Bind(commandBuffer->GetVulkanCommandBuffer());

        commandBuffer->BindVertexBuffers({ skyboxMesh->GetVertexBuffer()->GetVulkanBuffer() });
        commandBuffer->BindIndexBuffer({ skyboxMesh->GetIndexBuffer()->GetVulkanBuffer() });

        skyboxPipeline->PushConstants(commandBuffer->GetVulkanCommandBuffer());
        skyboxPipeline->BindDescriptorSets(commandBuffer->GetVulkanCommandBuffer(), currentFrame);

        commandBuffer->Draw(skyboxMesh->GetIndexCount());

        deferredRenderPass->End(commandBuffer->GetVulkanCommandBuffer());

        // Begin the render pass
        swapchain->BeginRenderPass(commandBuffer->GetVulkanCommandBuffer());

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
        swapchain->EndRenderPass(commandBuffer->GetVulkanCommandBuffer());

        // End and save GPU timer results
        renderTimestampQueries[currentFrame]->End(commandBuffer->GetVulkanCommandBuffer());
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

        textureSampler->Destroy();

        skyboxMesh->Destroy();
        skyboxPipeline->Destroy();
        skyboxDescriptorSetLayout->Destroy();

        mergingPipeline->Destroy();
        mergingDescriptorSetLayout->Destroy();

        scenePipeline->Destroy();
        sceneDescriptorSetLayout->Destroy();

        deferredFramebuffer->Destroy();
        deferredRenderPass->Destroy();

        renderedImage->Destroy();
        depthStencilBuffer->Destroy();
        normalBuffer->Destroy();
        specularAndShininessBuffer->Destroy();
        diffuseBuffer->Destroy();
        positionBuffer->Destroy();

        VulkanRenderer::Destroy();
    }

}