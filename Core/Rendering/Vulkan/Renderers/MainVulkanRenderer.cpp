//
// Created by Nikolay Kanchevski on 23.12.22.
//

#include "MainVulkanRenderer.h"

#include "../../UI/ImGuiCore.h"
#include "../../../../Engine/Components/Camera.h"
#include "../../../../Engine/Classes/Time.h"
#include "../../../../Engine/Components/MeshRenderer.h"
#include "../../Math/MatrixUtilities.h"

using Rendering::UI::ImGuiCore;
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
        Camera *camera = Camera::GetMainCamera();

        // Update uniform data
        auto &sceneUniformData = scenePipeline->GetUniformBufferData();
        sceneUniformData.view = camera->GetViewMatrix();
        sceneUniformData.projection = camera->GetProjectionMatrix();
        sceneUniformData.directionalLightCount = DirectionalLight::GetDirectionalLightCount();
        sceneUniformData.pointLightCount = PointLight::GetPointLightCount();

        auto &skyboxUniformData = skyboxPipeline->GetUniformBufferData();
        skyboxUniformData.view = sceneUniformData.view;
        skyboxUniformData.projection = sceneUniformData.projection;

        float timeAngle = std::fmod(Time::GetUpTime(), 360.0f) * -0.8f;
        skyboxUniformData.model = MatrixUtilities::CreateModel(Vector3(0.0f), {timeAngle, 0.0f, 0.0f });

        // Update storage data
        auto &storageData = scenePipeline->GetStorageBufferData();

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
        offscreenTimestampQueries[currentFrame]->Begin(commandBuffer->GetVulkanCommandBuffer());

        // Bind the pipeline
        scenePipeline->Bind(commandBuffer->GetVulkanCommandBuffer());

        // Begin rendering offscreen image
        sceneOffscreenRenderer->Begin(commandBuffer->GetVulkanCommandBuffer(), currentFrame);

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
                    commandBuffer->GetVulkanCommandBuffer(),
                     // TODO: BINDLESS
//                    { VK::GetDevice()->GetDescriptorIndexingSupported() ? globalBindlessDescriptorSet->GetVulkanDescriptorSet() : meshRenderer.GetDescriptorSet() },
                    { meshRenderer.GetDescriptorSet() },
                    currentFrame
            );

            // Draw using the index buffer to prevent vertex re-usage
            commandBuffer->Draw(meshRenderer.GetMesh()->GetIndexCount());
        }

        skyboxPipeline->Bind(commandBuffer->GetVulkanCommandBuffer());

        commandBuffer->BindVertexBuffers({ skyboxMesh->GetVertexBuffer()->GetVulkanBuffer() });
        commandBuffer->BindIndexBuffer({ skyboxMesh->GetIndexBuffer()->GetVulkanBuffer() });

        skyboxPipeline->BindDescriptorSets(commandBuffer->GetVulkanCommandBuffer(), { }, currentFrame);

        commandBuffer->Draw(skyboxMesh->GetIndexCount());

        // End the offscreen renderer
        sceneOffscreenRenderer->End(commandBuffer->GetVulkanCommandBuffer());

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
        offscreenTimestampQueries[currentFrame]->End(commandBuffer->GetVulkanCommandBuffer());
        this->totalDrawTime = offscreenTimestampQueries[currentFrame]->GetTimeTaken();

        // End the command buffer and check for errors during command execution
        commandBuffer->End();

        // Render to swapchain
        swapchain->SubmitCommandBuffers();
    }

    /* --- SETTER METHODS --- */

    void MainVulkanRenderer::SetSampling(Sampling newSampling)
    {
        VulkanRenderer::SetSampling(newSampling);

        VK::GetDevice()->WaitUntilIdle();

        sceneOffscreenRenderer->SetMultisampling(newSampling);

        scenePipeline->GetCreateInfo().sampling = newSampling;
        scenePipeline->Recreate();

        skyboxPipeline->GetCreateInfo().sampling = newSampling;
        skyboxPipeline->Recreate();

        CreateOffscreenDescriptorSets();
    }

    void MainVulkanRenderer::SetShadingType(const ShadingType newShadingType)
    {
        VulkanRenderer::SetShadingType(newShadingType);

        VK::GetDevice()->WaitUntilIdle();

        scenePipeline->GetCreateInfo().shadingType = newShadingType;
        scenePipeline->Recreate();
    }

    void MainVulkanRenderer::InitializeOffscreenRendering()
    {
        CreateSceneRenderingObjects();
        CreateSkyboxRenderingObjects();

        CreateOffscreenDescriptorSets();
        CreateTimestampQueries();
    }

    void MainVulkanRenderer::CreateSkyboxRenderingObjects()
    {
        // Create descriptor set layout for skybox pipeline
        skyboxDescriptorSetLayout = DescriptorSetLayout::Builder()
            .SetShaderStages(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
            .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            .AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
        .Build();

        // Create shaders for skybox pipeline
        auto skyboxVertexShader = Shader::Create({ .filePath = "Shaders/skybox_vertex.vert.spv", .shaderType = ShaderType::VERTEX });
        auto skyboxFragmentShader = Shader::Create({ .filePath = "Shaders/skybox_fragment.frag.spv", .shaderType = ShaderType::FRAGMENT });

        // Create pipeline
        skyboxPipeline = SKYBOX_PIPELINE::Create({
            .maxConcurrentFrames = swapchain->GetMaxConcurrentFramesCount(),
            .vertexAttributes = { VertexAttribute::POSITION },
            .shaders = {skyboxVertexShader, skyboxFragmentShader },
            .renderPass = sceneOffscreenRenderer->GetRenderPass(),
            .descriptorSetLayout = skyboxDescriptorSetLayout,
            .sampling = Sampling::MSAAx1,
            .cullMode = CullMode::FRONT,
            .shadingType = ShadingType::FILL,
        });

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

        // Create cubemap texture
        skyboxCubemap = Cubemap::Create({ .filePaths = {
            "Textures/Skyboxes/Default/skybox_right.png",
            "Textures/Skyboxes/Default/skybox_left.png",
            "Textures/Skyboxes/Default/skybox_top.png",
            "Textures/Skyboxes/Default/skybox_bottom.png",
            "Textures/Skyboxes/Default/skybox_front.png",
            "Textures/Skyboxes/Default/skybox_back.png",
        }, .cubemapType = CUBEMAP_TYPE_SKYBOX });

        // Add skybox cubemap to its pipeline's descriptor set
        for (const auto &descriptorSet : skyboxPipeline->GetDescriptorSets())
        {
            descriptorSet->WriteCubemap(1, skyboxCubemap);
            descriptorSet->Allocate();
        }
    }

    void MainVulkanRenderer::CreateSceneRenderingObjects()
    {
        // Create descriptor set layout
        auto descriptorSetBuilder = DescriptorSetLayout::Builder();
        descriptorSetBuilder.SetShaderStages(VK_SHADER_STAGE_ALL);
        descriptorSetBuilder.AddBinding(UNIFORM_BUFFER_BINDING, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        descriptorSetBuilder.AddBinding(STORAGE_BUFFER_BINDING, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);

        if (VK::GetDevice()->GetDescriptorIndexingSupported())
        {
            // TODO: BINDLESS
            // descriptorSetBuilder.AddBinding(BINDLESS_TEXTURE_BINDING, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT, MAX_TEXTURES);
        }
        else
        {
            descriptorSetBuilder.AddBinding(DIFFUSE_TEXTURE_BINDING, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
            descriptorSetBuilder.AddBinding(SPECULAR_TEXTURE_BINDING, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
            descriptorSetBuilder.AddBinding(HEIGHT_MAP_TEXTURE_BINDING, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        }

        // Create descriptor set layout
        sceneDescriptorSetLayout = descriptorSetBuilder.Build();
        VK::m_Instance.descriptorSetLayout = sceneDescriptorSetLayout;

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
        auto sceneVertexShader = Shader::Create({ .filePath = VK::GetDevice()->GetDescriptorIndexingSupported() ? "Shaders/blinn-phong-vertex_bindless.vert.spv" : "Shaders/blinn-phong-vertex.vert.spv", .shaderType = ShaderType::VERTEX });
        auto sceneFragmentShader = Shader::Create({ .filePath = VK::GetDevice()->GetDescriptorIndexingSupported() ? "Shaders/blinn-phong-fragment_bindless.frag.spv" : "Shaders/blinn-phong-fragment.frag.spv", .shaderType = ShaderType::FRAGMENT });

        // Create pipeline
        scenePipeline = OFFSCREEN_PIPELINE::Create({
            .maxConcurrentFrames = swapchain->GetMaxConcurrentFramesCount(),
            .vertexAttributes = { VertexAttribute::POSITION, VertexAttribute::NORMAL, VertexAttribute::TEXTURE_COORDINATE },
            .shaders = {sceneVertexShader, sceneFragmentShader },
            .renderPass = sceneOffscreenRenderer->GetRenderPass(),
            .descriptorSetLayout = sceneDescriptorSetLayout,
            .sampling = sampling,
            .shadingType = shadingType,
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
        skyboxCubemap->Destroy();
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
