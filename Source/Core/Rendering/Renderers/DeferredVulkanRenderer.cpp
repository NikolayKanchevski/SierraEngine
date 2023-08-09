//
// Created by Nikolay Kanchevski on 5.02.23.
//

#include "DeferredVulkanRenderer.h"

#include "../Bases/VK.h"
#include "../../../Editor/GUI.h"
#include "../../../Editor/Editor.h"
#include "../../../Engine/Classes/Math.h"
#include "../../../Engine/Classes/Input.h"
#include "../../../Engine/Classes/Raycast.h"
#include "../../../Engine/Handlers/Project.h"
#include "../../../Engine/Components/Camera.h"
#include "../../../Engine/Components/Transform.h"
#include "../../../Engine/Components/PointLight.h"
#include "../../../Engine/Components/MeshRenderer.h"
#include "../../../Engine/Components/DirectionalLight.h"
#include "../../../Engine/Handlers/Assets/AssetManager.h"

#define DIFFUSE_BUFFER_BINDING      2
#define SS_BUFFER_BINDING           3          // Specular & Shininess
#define NORMAL_BUFFER_BINDING       4
#define DEPTH_BUFFER_BINDING        5
#define SKYBOX_BUFFER_BINDING       6

#define TEXTURE_TYPE_TO_BINDING(textureType)(static_cast<uint32>(textureType) + 2)
#define DIFFUSE_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(Engine::TextureType::DIFFUSE)
#define SPECULAR_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(Engine::TextureType::SPECULAR)
#define NORMAL_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(Engine::TextureType::NORMAL)
#define HEIGHT_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(Engine::TextureType::HEIGHT)

namespace Sierra::Rendering
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
        // Create command buffers to record GPU commands on
        commandBuffers.resize(MAX_CONCURRENT_FRAMES);
        for (uint32 i = MAX_CONCURRENT_FRAMES; i--;)
        {
            commandBuffers[i] = CommandBuffer::Create({ });
        }

        // Create buffers
        uniformBuffers.resize(MAX_CONCURRENT_FRAMES);
        storageBuffers.resize(MAX_CONCURRENT_FRAMES);
        for (uint32 i = MAX_CONCURRENT_FRAMES; i--;)
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
            .width = swapchain->GetWidth(),
            .height = swapchain->GetHeight(),
            .format = ImageFormat::R16_UINT,
            .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::INPUT_ATTACHMENT | ImageUsage::SAMPLED
        });

        // Create diffuse image
        diffuseBuffer = Image::Create({
            .width = swapchain->GetWidth(),
            .height = swapchain->GetHeight(),
            .format = ImageFormat::R8G8B8A8_UNORM,
            .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::INPUT_ATTACHMENT | ImageUsage::TRANSIENT_ATTACHMENT
        });

        // Create specular and shininess image
        specularAndShininessBuffer = Image::Create({
            .width = swapchain->GetWidth(),
            .height = swapchain->GetHeight(),
            .format = ImageFormat::R8G8_UNORM,
            .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::INPUT_ATTACHMENT | ImageUsage::TRANSIENT_ATTACHMENT
        });

        // Create normals data image
        normalBuffer = Image::Create({
            .width = swapchain->GetWidth(),
            .height = swapchain->GetHeight(),
            .format = ImageFormat::R32_UINT, // Using a single channel 32-bit UINT, as the normal (Vector3) is compressed to a UINT with a 15-bit precision per channel
            .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::INPUT_ATTACHMENT | ImageUsage::TRANSIENT_ATTACHMENT
        });

        // Create depth stencil image
        depthBuffer = Image::Create({
            .width = swapchain->GetWidth(),
            .height = swapchain->GetHeight(),
            .format = VK::GetDevice()->GetBestDepthImageFormat(),
            .usage = ImageUsage::DEPTH_STENCIL_ATTACHMENT | ImageUsage::INPUT_ATTACHMENT | ImageUsage::SAMPLED
        });

        // Create final composition image
        compositionImage = Image::Create({
            .width = swapchain->GetWidth(),
            .height = swapchain->GetHeight(),
            .format = ImageFormat::R8G8B8A8_UNORM,
            .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED
        });

        // Create texture sampler to use when passing data to shaders
        bufferSampler = Sampler::Create({ .applyBilinearFiltering = true });

        // Create render pass
        deferredRenderPass = RenderPass::Create({
            .attachments = {
                // Depth stencil buffer attachment [0]
                {
                    .image = depthBuffer,
                    .loadOp = LoadOp::CLEAR,
                    .storeOp = StoreOp::STORE,
                    .type = RenderPassAttachmentType::DEPTH
                },

                // Entity ID buffer attachment [1]
                {
                    .image = IDBuffer,
                    .loadOp = LoadOp::CLEAR,
                    .storeOp = StoreOp::STORE,
                    .type = RenderPassAttachmentType::COLOR
                },

                // Color buffer attachment [2]
                {
                    .image = diffuseBuffer,
                    .loadOp = LoadOp::CLEAR,
                    .storeOp = StoreOp::DONT_CARE,
                    .type = RenderPassAttachmentType::COLOR
                },

                // Specular and shininess buffer attachment [3]
                {
                    .image = specularAndShininessBuffer,
                    .loadOp = LoadOp::CLEAR,
                    .storeOp = StoreOp::DONT_CARE,
                    .type = RenderPassAttachmentType::COLOR
                },

                // Normal buffer attachment [4]
                {
                    .image = normalBuffer,
                    .loadOp = LoadOp::CLEAR,
                    .storeOp = StoreOp::DONT_CARE,
                    .type = RenderPassAttachmentType::COLOR
                },

                // Final composition [5]
                {
                    .image = compositionImage,
                    .loadOp = LoadOp::CLEAR,
                    .storeOp = StoreOp::STORE,
                    .type = RenderPassAttachmentType::COLOR
                }
            },
            .subpassInfos = {
                // Subpass 0
                {
                    .renderTargets = { 0 }
                },

                // Subpass 1
                {
                    .renderTargets = { 1, 2, 3, 4 },
                    .loadDepthAttachment = true
                },

                // Subpass 2
                {
                    .renderTargets = { 5 },
                    .subpassInputs = { 0, 1, 2, 3, 4 }
                }
            }
        });

        // Create framebuffer
        deferredFramebuffer = Framebuffer::Create({
            .renderPass = deferredRenderPass,
            .attachments = {
                depthBuffer,
                IDBuffer,
                diffuseBuffer,
                specularAndShininessBuffer,
                normalBuffer,
                compositionImage
            }
        });

        // Create descriptor set layout for depth pre-pass
        earlyDepthPassDescriptorSetLayout = DescriptorSetLayout::Create({
            .bindings = {
                { UNIFORM_BUFFER_BINDING, { .descriptorType = DescriptorType::UNIFORM_BUFFER,            .shaderStages = ShaderType::VERTEX } },
                { STORAGE_BUFFER_BINDING, { .descriptorType = DescriptorType::STORAGE_BUFFER,            .shaderStages = ShaderType::VERTEX } },
                { HEIGHT_TEXTURE_BINDING, { .descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,    .shaderStages = ShaderType::VERTEX } }
            },
            .flags = DescriptorSetLayoutFlag::PUSH_DESCRIPTOR_KHR
        });

        // Load vertex shader for depth pre-pass
        auto vertexShader = Shader::Load({
            .filePath = Engine::Project::GetProjectDirectory() / "Shaders/Deferred/EarlyDepthPass.vert.spv",
            .shaderType = ShaderType::VERTEX,
            .vertexAttributes = {
                VertexAttributeType::POSITION,
                VertexAttributeType::NORMAL,
                VertexAttributeType::UV
            }
        });

        // Create early depth pass pipeline
        earlyDepthPassPipeline = GraphicsPipeline::Create({
            .shaders = { vertexShader },
            .shaderInfo = CompiledPipelineShaderInfo {
                .pushConstantData = PushConstantData {
                    .size = sizeof(BufferPushConstant),
                    .shaderStages = ShaderType::VERTEX
                },
                .descriptorSetLayout = &earlyDepthPassDescriptorSetLayout
            },
            .renderPassInfo = GraphicsPipelineRenderPassInfo {
                .subpass = 0,
                .renderPass = deferredRenderPass
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
        vertexShader = Shader::Load({
            .filePath = Engine::Project::GetProjectDirectory() / "Shaders/Deferred/G-Buffer.vert.spv",
            .shaderType = ShaderType::VERTEX,
            .vertexAttributes = {
                VertexAttributeType::POSITION,
                VertexAttributeType::NORMAL,
                VertexAttributeType::UV
            }
        });
        auto fragmentShader = Shader::Load({ .filePath = Engine::Project::GetProjectDirectory() / "Shaders/Deferred/G-Buffer.frag.spv", .shaderType = ShaderType::FRAGMENT });

        // Create scene renderer pipeline
        bufferPipeline = GraphicsPipeline::Create({
            .shaders = { vertexShader, fragmentShader },
            .shaderInfo = CompiledPipelineShaderInfo {
                .pushConstantData = PushConstantData {
                    .size = sizeof(BufferPushConstant),
                    .shaderStages = ShaderType::VERTEX | ShaderType::FRAGMENT
                },
                .descriptorSetLayout = &bufferDescriptorSetLayout
            },
            .renderPassInfo = GraphicsPipelineRenderPassInfo {
                .subpass = 1,
                .renderPass = deferredRenderPass
            }
        });

        // Create set layout for composition pipeline
        compositionDescriptorSetLayout = DescriptorSetLayout::Create({
            .bindings = {
                { UNIFORM_BUFFER_BINDING,    { .descriptorType = DescriptorType::UNIFORM_BUFFER,         .shaderStages = ShaderType::VERTEX | ShaderType::FRAGMENT } },
                { STORAGE_BUFFER_BINDING,    { .descriptorType = DescriptorType::STORAGE_BUFFER,         .shaderStages = ShaderType::FRAGMENT                      } },
                { DIFFUSE_BUFFER_BINDING,    { .descriptorType = DescriptorType::INPUT_ATTACHMENT,       .shaderStages = ShaderType::FRAGMENT                      } },
                { SS_BUFFER_BINDING,         { .descriptorType = DescriptorType::INPUT_ATTACHMENT,       .shaderStages = ShaderType::FRAGMENT                      } },
                { NORMAL_BUFFER_BINDING,     { .descriptorType = DescriptorType::INPUT_ATTACHMENT,       .shaderStages = ShaderType::FRAGMENT                      } },
                { DEPTH_BUFFER_BINDING,      { .descriptorType = DescriptorType::INPUT_ATTACHMENT,       .shaderStages = ShaderType::FRAGMENT                      } },
                { SKYBOX_BUFFER_BINDING,     { .descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER, .shaderStages = ShaderType::FRAGMENT                      } }
            },
            .flags = DescriptorSetLayoutFlag::PUSH_DESCRIPTOR_KHR
        });

        // Load merging shaders
        vertexShader = Shader::Load({ .filePath = Engine::Project::GetProjectDirectory() / "Shaders/Deferred/Composition.vert.spv", .shaderType = ShaderType::VERTEX });
        fragmentShader = Shader::Load({
            .filePath = Engine::Project::GetProjectDirectory() / "Shaders/Deferred/Composition.frag.spv",
            .shaderType = ShaderType::FRAGMENT,
            .specializationConstants = {
                { 0, { .size = UINT_SIZE } }
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
            .renderPassInfo = GraphicsPipelineRenderPassInfo {
                .subpass = 2,
                .renderPass = deferredRenderPass
            },
            .shadingType = ShadingType::FILL
        });

        // Create modules
        raycaster = Modules::Raycaster::Create({ .IDBuffer = IDBuffer, .depthBuffer = depthBuffer, .sampler = bufferSampler });

        // Create UI-related objects
        renderTimestampQueries.resize(MAX_CONCURRENT_FRAMES);
        renderedImageDescriptorSets.resize(MAX_CONCURRENT_FRAMES);
        for (uint32 i = MAX_CONCURRENT_FRAMES; i--;)
        {
            renderTimestampQueries[i] = TimestampQuery::Create();
            renderedImageDescriptorSets[i] = ImGui_ImplVulkan_AddTexture(bufferSampler->GetVulkanSampler(), compositionImage->GetVulkanImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
    }

    UniquePtr<DeferredVulkanRenderer> DeferredVulkanRenderer::Create(const VulkanRendererCreateInfo &createInfo)
    {
        return std::make_unique<DeferredVulkanRenderer>(createInfo);
    }

    /* --- POLLING METHODS --- */

    void DeferredVulkanRenderer::Update()
    {
        using namespace Engine;

        // Get current frame index
        uint32 currentFrame = swapchain->GetCurrentFrameIndex();

        // Update camera (for now use the first spawned camera)
        Camera &camera = World::GetComponent<Camera>(World::GetAllComponentsOfType<Camera>()[0]);

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

        auto enttMeshView = World::GetAllComponentsOfType<MeshRenderer>();
        for (const auto &enttEntity : enttMeshView)
        {
            MeshRenderer &meshRenderer = enttMeshView.get<MeshRenderer>(enttEntity);
            storageData.objectData[meshRenderer.GetMeshID()].modelMatrix = meshRenderer.GetModelMatrix();
            storageData.objectData[meshRenderer.GetMeshID()].normalMatrix = meshRenderer.GetNormalMatrix();
        }

        auto enttDirectionalLightView = World::GetAllComponentsOfType<DirectionalLight>();
        storageData.directionalLightCount = enttDirectionalLightView.size();
        for (const auto &enttEntity : enttDirectionalLightView)
        {
            DirectionalLight &directionalLight = enttDirectionalLightView.get<DirectionalLight>(enttEntity);

            auto &shaderDirectionalLight = storageData.directionalLights[directionalLight.GetID()];
            shaderDirectionalLight.viewSpaceMatrix = directionalLight.GetViewSpaceMatrix();
            shaderDirectionalLight.direction = directionalLight.GetDirection();
            shaderDirectionalLight.intensity = directionalLight.intensity;
            shaderDirectionalLight.color = glm::normalize(directionalLight.color);
        }

        auto enttPointLightView = World::GetAllComponentsOfType<PointLight>();
        storageData.pointLightCount = enttPointLightView.size();
        for (const auto &enttEntity : enttPointLightView)
        {
            PointLight &pointLight = enttPointLightView.get<PointLight>(enttEntity);

            auto &shaderPointLight = storageData.pointLights[pointLight.GetID()];
            shaderPointLight.viewSpaceMatrix = pointLight.GetViewSpaceMatrix();
            shaderPointLight.color = glm::normalize(pointLight.color);
            shaderPointLight.intensity = pointLight.intensity;
            shaderPointLight.position = pointLight.GetComponent<Engine::Transform>().GetWorldPosition();
            shaderPointLight.linear = pointLight.linear;
            shaderPointLight.quadratic = pointLight.quadratic;
        }

        // Flush buffers
        uniformBuffers[currentFrame]->Flush();
        storageBuffers[currentFrame]->Flush();

        // Rotate skybox
        float timeAngle = std::fmod(Time::GetUpTime(), 360.0f) * 0.8f;
        compositionPushConstantData.skyboxModel = Engine::Math::CreateModelMatrix({ 0.0f, 0.0f, 0.0f }, { timeAngle, 0.0f, 0.0f });
    }

    void DeferredVulkanRenderer::DrawUI()
    {
        using namespace Editor;

        // Draw global editor
        Editor::DrawEditor({
            .imGuiInstance = imGuiInstance,
            .renderedTextureDescriptorSet = renderedImageDescriptorSets[swapchain->GetCurrentFrameIndex()],
            .frameDrawTime = renderTimestampQueries[swapchain->GetCurrentFrameIndex()]->GetTimeTaken()
        });

        // Draw renderer's window
        GUI::BeginWindow("Renderer's Properties", WindowFlags::NO_NAV);

        // Rendered image type dropdown
        {
            static uint32 currentRenderedImageValue = RendererOutput::RenderedImage;
            static const char* renderedImageValueTypes[] = { "Final Render", "Position Buffer", "Diffuse Buffer", "Specular Buffer", "Shininess Buffer", "Normal Buffer", "Depth Buffer" };

            GUI::Text("Renderer Image Output:");
            GUI::SetNextItemWidthToFit();
            if (GUI::Dropdown("##DEFFERED_VULKAN_RENDERER_RENDERED_IMGAGE_TYPE_DROPDOWN", currentRenderedImageValue, renderedImageValueTypes, 7))
            {
                compositionPipeline->SetSpecializationConstant(0, currentRenderedImageValue);
            }
        }

        // Shading type dropdown
        {
            static uint32 currentShadingTypeIndex = 0;
            static const char* shadingTypes[] = { "Shaded", "Wireframe"};

            GUI::Text("Renderer Shading:");
            GUI::SetNextItemWidthToFit();
            if (GUI::Dropdown("##DEFFERED_VULKAN_RENDERER_SHADING_TYPE_DROPDOWN", currentShadingTypeIndex, shadingTypes, 2))
            {
                earlyDepthPassPipeline->SetShadingType(static_cast<ShadingType>(currentShadingTypeIndex));
                bufferPipeline->SetShadingType(static_cast<ShadingType>(currentShadingTypeIndex));
            }
        }
        GUI::EndWindow();

        VulkanRenderer::DrawUI();
    }

    void DeferredVulkanRenderer::Render()
    {
        Engine::AssetManager::SubmitCommands();

        VulkanRenderer::Render();

        // Prepare next swapchain image
        swapchain->AcquireNextImage();

        // Acquire current frame's command buffer
        uint32 currentFrame = swapchain->GetCurrentFrameIndex();
        auto &commandBuffer = commandBuffers[currentFrame];

        // Begin recording GPU commands
        commandBuffer->Begin();

        // Bind all vertex & index data in bulk
        Engine::AssetManager::BindMeshes(commandBuffer);

        // Start GPU timer
        renderTimestampQueries[currentFrame]->Begin(commandBuffer);

        // Begin rendering to the deferred pass
        deferredRenderPass->Begin(deferredFramebuffer, commandBuffer);

        // Bind pipeline, which is to draw the depth image
        earlyDepthPassPipeline->Begin(commandBuffer);

        // Set global buffer data for depth pass stage
        earlyDepthPassPipeline->SetShaderBinding(UNIFORM_BUFFER_BINDING, uniformBuffers[currentFrame]);
        earlyDepthPassPipeline->SetShaderBinding(STORAGE_BUFFER_BINDING, storageBuffers[currentFrame]);

        // Define the mesh push constant, which will be modified before each draw call
        BufferPushConstant pushConstant{};

        // Loop through all meshes in the world to draw the depth image only
        auto enttMeshView = Engine::World::GetAllComponentsOfType<Engine::MeshRenderer>();
        for (auto enttEntity : enttMeshView)
        {
            // Get current mesh
            auto &meshRenderer = enttMeshView.get<Engine::MeshRenderer>(enttEntity);

            // Send push constant data to shader
            pushConstant.material.diffuse = meshRenderer.GetMaterial()->diffuse;
            pushConstant.material.specular = meshRenderer.GetMaterial()->specular;
            pushConstant.material.shininess = meshRenderer.GetMaterial()->shininess;
            pushConstant.material.vertexExaggeration = meshRenderer.GetMaterial()->vertexExaggeration;

            pushConstant.meshID = meshRenderer.GetMeshID();
            pushConstant.entityID = static_cast<uint32>(meshRenderer.GetEntity().GetID());
            pushConstant.meshTexturesPresence = meshRenderer.GetTexturePresence();
            earlyDepthPassPipeline->SetPushConstants(commandBuffer, pushConstant);

            // Set mesh's height map
            earlyDepthPassPipeline->SetShaderBinding(HEIGHT_TEXTURE_BINDING, meshRenderer.GetMaterial()->GetTexture(Engine::TextureType::HEIGHT));

            // Draw mesh
            earlyDepthPassPipeline->DrawMesh(commandBuffer, meshRenderer.GetMesh());
        }

        // End rendering to the depth map
        earlyDepthPassPipeline->End(commandBuffer);

        // Begin rendering G-Buffer
        deferredRenderPass->NextSubpass(commandBuffer);

        // Bind the pipeline, which will draw to the separate G-Buffer images
        bufferPipeline->Begin(commandBuffer);

        // Set global buffer data for G-Buffer shaders
        bufferPipeline->SetShaderBinding(UNIFORM_BUFFER_BINDING, uniformBuffers[currentFrame]);
        bufferPipeline->SetShaderBinding(STORAGE_BUFFER_BINDING, storageBuffers[currentFrame]);

        // Loop through all meshes in the world
        for (auto enttEntity : enttMeshView)
        {
            // Get current mesh
            auto &meshRenderer = enttMeshView.get<Engine::MeshRenderer>(enttEntity);

            // Send push constant data to shader
            pushConstant.material.diffuse = meshRenderer.GetMaterial()->diffuse;
            pushConstant.material.specular = meshRenderer.GetMaterial()->specular;
            pushConstant.material.shininess = meshRenderer.GetMaterial()->shininess;
            pushConstant.material.vertexExaggeration = meshRenderer.GetMaterial()->vertexExaggeration;

            pushConstant.meshID = meshRenderer.GetMeshID();
            pushConstant.entityID = static_cast<uint32>(meshRenderer.GetEntity().GetID());
            pushConstant.meshTexturesPresence = meshRenderer.GetTexturePresence();
            bufferPipeline->SetPushConstants(commandBuffer, pushConstant);

            // Set mesh's textures
            for (uint32 i = static_cast<uint32>(Engine::TextureType::TOTAL_COUNT); i--;)
            {
                bufferPipeline->SetShaderBinding(TEXTURE_TYPE_TO_BINDING(i), meshRenderer.GetMaterial()->GetTexture(static_cast<Engine::TextureType>(i)));
            }

            // Draw mesh
            bufferPipeline->DrawMesh(commandBuffer, meshRenderer.GetMesh());
        }

        // End G-Buffer pipeline
        bufferPipeline->End(commandBuffer);

        // End rendering to G-Buffer and begin rendering composition image
        deferredRenderPass->NextSubpass(commandBuffer);

        // Bind the pipeline, which will be combining the G-Buffer data into the final image
        compositionPipeline->Begin(commandBuffer);

        // Send all G-Buffer attachments to shaders
        compositionPipeline->SetShaderBinding(UNIFORM_BUFFER_BINDING, uniformBuffers[currentFrame]);
        compositionPipeline->SetShaderBinding(STORAGE_BUFFER_BINDING, storageBuffers[currentFrame]);
        compositionPipeline->SetShaderBinding(DIFFUSE_BUFFER_BINDING, diffuseBuffer, bufferSampler);
        compositionPipeline->SetShaderBinding(SS_BUFFER_BINDING, specularAndShininessBuffer, bufferSampler);
        compositionPipeline->SetShaderBinding(NORMAL_BUFFER_BINDING, normalBuffer, bufferSampler);
        compositionPipeline->SetShaderBinding(DEPTH_BUFFER_BINDING, depthBuffer, bufferSampler);
        compositionPipeline->SetShaderBinding(SKYBOX_BUFFER_BINDING, Engine::AssetManager::GetDefaultCollection().GetCubemap(Engine::CubemapType::SKYBOX));
        compositionPipeline->SetPushConstants(commandBuffer, compositionPushConstantData);

        // Draw a fullscreen triangle (with 3 vertices) and the skybox cube, which consists of 36 more
        compositionPipeline->Draw(commandBuffer, 39);

        // End composition pipeline
        compositionPipeline->End(commandBuffer);

        // End rendering
        deferredRenderPass->End(commandBuffer);

        // Update ID buffer on CPU side
        raycaster->UpdateData(commandBuffer, uniformBuffers[currentFrame]);

        // End and save GPU timer results
        renderTimestampQueries[currentFrame]->End(commandBuffer);

        // Get hovered entity
        Engine::Entity hoveredEntity = raycaster->GetHoveredEntity();

        // Handle viewport clicking
        if (Engine::Input::GetMouseButtonPressed(MouseButton::LEFT) && !ImGuizmo::IsOver() && !ImGuizmo::IsUsing() && Editor::GUI::GetHoveredWindow() != nullptr && Editor::GUI::GetHoveredWindow()->ID == Editor::GUI::HashString("Scene View"))
        {
            Editor::SetSelectedEntity(hoveredEntity);
        }

        // Update hovered world position
        Engine::Raycast::SetHoveredEntity(hoveredEntity);
        Engine::Raycast::SetHoveredWorldPosition(raycaster->GetHoveredPosition());

        // Begin rendering to swapchain, which is connected to the window
        swapchain->BeginRenderPass(commandBuffer);

        // Render ImGui UI
        if (Editor::GUI::GetDrawData() != nullptr)
        {
            ImGui_ImplVulkan_RenderDrawData(Editor::GUI::GetDrawData(), commandBuffer->GetVulkanCommandBuffer());
        }

        // End swapchain rendering
        swapchain->EndRenderPass(commandBuffer);

        // Stop recording GPU commands
        commandBuffer->End();

        // Swap out old swapchain image
        swapchain->SwapImage(commandBuffer);
    }

    /* --- DESTRUCTOR --- */

    void DeferredVulkanRenderer::Destroy()
    {
        VK::GetDevice()->WaitUntilIdle();

        raycaster->Destroy();

        compositionPipeline->Destroy();
        compositionDescriptorSetLayout->Destroy();

        compositionImage->Destroy();

        bufferPipeline->Destroy();
        bufferDescriptorSetLayout->Destroy();

        deferredRenderPass->Destroy();
        deferredFramebuffer->Destroy();

        depthBuffer->Destroy();
        normalBuffer->Destroy();
        specularAndShininessBuffer->Destroy();
        diffuseBuffer->Destroy();
        IDBuffer->Destroy();
        bufferSampler->Destroy();

        earlyDepthPassPipeline->Destroy();
        earlyDepthPassDescriptorSetLayout->Destroy();

        for (uint32 i = MAX_CONCURRENT_FRAMES; i--;)
        {
            uniformBuffers[i]->Destroy();
            storageBuffers[i]->Destroy();
        }

        VulkanRenderer::Destroy();
    }

}