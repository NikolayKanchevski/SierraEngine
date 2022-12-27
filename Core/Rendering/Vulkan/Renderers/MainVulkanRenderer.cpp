//
// Created by Nikolay Kanchevski on 23.12.22.
//

#include "MainVulkanRenderer.h"

#include <imgui_impl_vulkan.h>
#include <glm/gtc/matrix_transform.hpp>
#include <tbb/parallel_for_each.h>

#include "../../UI/ImGuiCore.h"
#include "../../../../Engine/Components/Camera.h"
#include "../../../../Engine/Components/MeshRenderer.h"

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

    std::unique_ptr<MainVulkanRenderer> MainVulkanRenderer::Create(const VulkanRendererCreateInfo createInfo)
    {
        return std::make_unique<MainVulkanRenderer>(createInfo);
    }

    /* --- POLLING METHODS --- */

    void MainVulkanRenderer::Update()
    {
        // Update camera
        Camera *camera = Camera::GetMainCamera();
        Transform &cameraTransform = World::GetEnttRegistry()->get<Transform>(camera->GetEnttEntity());
        glm::vec3 rendererCameraPosition = { cameraTransform.position.x, -cameraTransform.position.y, cameraTransform.position.z };
        glm::vec3 rendererCameraFrontDirection = { camera->GetFrontDirection().x, -camera->GetFrontDirection().y, camera->GetFrontDirection().z };
        glm::vec3 rendererCameraUpDirection = { camera->GetUpDirection().x, -camera->GetUpDirection().y, camera->GetUpDirection().z };

        // Update uniform data
        auto &uniformData = offscreenGraphicsPipeline->GetUniformBufferData();
        uniformData.view = glm::lookAt(rendererCameraPosition, rendererCameraPosition + rendererCameraFrontDirection, rendererCameraUpDirection);
        uniformData.projection = glm::perspective(glm::radians(camera->fov), (float) ImGuiCore::GetSceneViewWidth() / (float) ImGuiCore::GetSceneViewHeight(), camera->nearClip, camera->farClip);
        uniformData.projection[1][1] *= -1;
        uniformData.directionalLightCount = DirectionalLight::GetDirectionalLightCount();
        uniformData.pointLightCount = PointLight::GetPointLightCount();

        // Update storage data
        auto &storageData = offscreenGraphicsPipeline->GetStorageBufferData();

        auto enttMeshView = World::GetEnttRegistry()->view<MeshRenderer>();
        tbb::parallel_for_each(enttMeshView.begin(), enttMeshView.end(),
            [&enttMeshView, &storageData](auto &enttEntity)
            {
                MeshRenderer &meshRenderer = enttMeshView.get<MeshRenderer>(enttEntity);
                storageData.objectDatas[meshRenderer.GetMeshID()].model = meshRenderer.GetModelMatrix();
            }
        );

        auto enttDirectionalLightView = World::GetEnttRegistry()->view<DirectionalLight>();
        tbb::parallel_for_each(enttDirectionalLightView.begin(), enttDirectionalLightView.end(),
            [&enttDirectionalLightView, &storageData](auto &enttEntity)
            {
                DirectionalLight &directionalLight = enttDirectionalLightView.get<DirectionalLight>(enttEntity);
                storageData.directionalLights[directionalLight.GetID()] = directionalLight;
            }
        );

        auto enttPointLightView = World::GetEnttRegistry()->view<PointLight>();
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
        uint32_t currentFrame = swapchain->GetCurrentFrameIndex();

        // Begin command buffer
        commandBuffer->Begin();

        // Start GPU timer
        offscreenTimestampQueries[currentFrame]->Begin(commandBuffer->GetVulkanCommandBuffer());

        // Bind the pipeline
        offscreenGraphicsPipeline->Bind(commandBuffer->GetVulkanCommandBuffer());

        // Begin rendering offscreen image
        offscreenRenderer->Begin(commandBuffer->GetVulkanCommandBuffer(), currentFrame);

        // Create a vector to hold vertex buffers of the meshes
        std::vector<VkBuffer> vertexBuffers(1);

        // For each mesh in the world
        auto enttMeshView = World::GetEnttRegistry()->view<MeshRenderer>();
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
            offscreenGraphicsPipeline->GetPushConstantData() = meshRenderer.GetPushConstantData();

            // Send push constant data to shader
            offscreenGraphicsPipeline->PushConstants(commandBuffer->GetVulkanCommandBuffer());

            // Use either bindless or bind*full* descriptor set if not supported
            offscreenGraphicsPipeline->BindDescriptorSets(
                    commandBuffer->GetVulkanCommandBuffer(),
                     // TODO: BINDLESS
//                    { VK::GetDevice()->GetDescriptorIndexingSupported() ? globalBindlessDescriptorSet->GetVulkanDescriptorSet() : meshRenderer.GetDescriptorSet() },
                    { meshRenderer.GetDescriptorSet() },
                    currentFrame
            );

            // Draw using the index buffer to prevent vertex re-usage
            commandBuffer->Draw(meshRenderer.GetMesh()->GetIndexCount());
        }

        // End the offscreen renderer
        offscreenRenderer->End(commandBuffer->GetVulkanCommandBuffer());

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

    void MainVulkanRenderer::SetSampling(const Sampling newSampling)
    {
        VulkanRenderer::SetSampling(newSampling);

        VK::GetDevice()->WaitUntilIdle();

        offscreenRenderer->SetMultisampling(newSampling);
        offscreenGraphicsPipeline->GetCreateInfo().sampling = newSampling;
        offscreenGraphicsPipeline->Recreate();

        CreateOffscreenDescriptorSets();
    }

    void MainVulkanRenderer::SetShadingType(const ShadingType newShadingType)
    {
        VulkanRenderer::SetShadingType(newShadingType);

        VK::GetDevice()->WaitUntilIdle();

        offscreenGraphicsPipeline->GetCreateInfo().shadingType = newShadingType;
        offscreenGraphicsPipeline->Recreate();
    }

    void MainVulkanRenderer::InitializeOffscreenRendering()
    {
        // Create descriptor set layout
        auto descriptorSetBuilder = DescriptorSetLayout::Builder();
        descriptorSetBuilder.SetShaderStages(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
        descriptorSetBuilder.AddBinding(UNIFORM_BUFFER_BINDING, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        descriptorSetBuilder.AddBinding(STORAGE_BUFFER_BINDING, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);

        if (VK::GetDevice()->GetDescriptorIndexingSupported())
        {
            // TODO: BINDLESS
//            descriptorSetBuilder.AddBinding(BINDLESS_TEXTURE_BINDING, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT, MAX_TEXTURES);
        }
        else
        {
            descriptorSetBuilder.AddBinding(DIFFUSE_TEXTURE_BINDING, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
            descriptorSetBuilder.AddBinding(SPECULAR_TEXTURE_BINDING, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
            descriptorSetBuilder.AddBinding(HEIGHT_MAP_TEXTURE_BINDING, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        }

        descriptorSetLayout = descriptorSetBuilder.BuildShared();
        VK::m_Instance.descriptorSetLayout = descriptorSetLayout;

        // Create renderer
        offscreenRenderer = OffscreenRenderer::Create({
            .width = swapchain->GetWidth(),
            .height = swapchain->GetHeight(),
            .maxConcurrentFrames = maxConcurrentFrames,
            .attachmentTypes = ATTACHMENT_COLOR | ATTACHMENT_DEPTH,
            .msaaSampling = MSAAx1
        });

        offscreenImageDescriptorSets.resize(maxConcurrentFrames);
        CreateOffscreenDescriptorSets();

        // Create shaders
        auto sceneVertexShader = Shader::Create({ .filePath = VK::GetDevice()->GetDescriptorIndexingSupported() ? "Shaders/blinn-phong-vertex_bindless.vert.spv" : "Shaders/blinn-phong-vertex.vert.spv", .shaderType = VERTEX_SHADER });
        auto sceneFragmentShader = Shader::Create({ .filePath = VK::GetDevice()->GetDescriptorIndexingSupported() ? "Shaders/blinn-phong-fragment_bindless.frag.spv" : "Shaders/blinn-phong-fragment.frag.spv", .shaderType = FRAGMENT_SHADER });

        // Create pipeline
        offscreenGraphicsPipeline = OFFSCREEN_PIPELINE::Create({
            .maxConcurrentFrames = swapchain->GetMaxConcurrentFramesCount(),
            .vertexAttributes = { VERTEX_ATTRIBUTE_POSITION, VERTEX_ATTRIBUTE_NORMAL, VERTEX_ATTRIBUTE_TEXTURE_COORDINATE },
            .shaders = {sceneVertexShader, sceneFragmentShader },
            .renderPass = offscreenRenderer->GetRenderPass(),
            .descriptorSetLayout = descriptorSetLayout,
            .sampling = sampling,
            .shadingType = shadingType,
        });

        // Create timestamp queries
        offscreenTimestampQueries.resize(maxConcurrentFrames);
        for (uint32_t i = 0; i < maxConcurrentFrames; i++)
        {
            offscreenTimestampQueries[i] = TimestampQuery::Create();
        }
    }

    void MainVulkanRenderer::CreateOffscreenDescriptorSets()
    {
        for (uint32_t i = maxConcurrentFrames; i--;)
        {
            offscreenImageDescriptorSets[i] = ImGui_ImplVulkan_AddTexture(offscreenRenderer->GetSampler()->GetVulkanSampler(), offscreenRenderer->GetColorImage(i)->GetVulkanImageView(), VK_IMAGE_LAYOUT_GENERAL);
        }
    }

    /* --- GETTER METHODS --- */

    /* --- DESTRUCTOR --- */

    void MainVulkanRenderer::TerminateOffscreenRendering()
    {
        offscreenGraphicsPipeline->Destroy();
        offscreenRenderer->Destroy();
        descriptorSetLayout->Destroy();
    }

    void MainVulkanRenderer::Destroy()
    {
        VK::GetDevice()->WaitUntilIdle();

        TerminateOffscreenRendering();

        VulkanRenderer::Destroy();
    }

}
