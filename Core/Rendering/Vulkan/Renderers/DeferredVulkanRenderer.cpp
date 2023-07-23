//
// Created by Nikolay Kanchevski on 5.02.23.
//

#include "DeferredVulkanRenderer.h"

#include "../VK.h"
#include "../../../../Editor/GUI.h"
#include "../../../../Editor/Editor.h"
#include "../../../../Engine/Classes/Math.h"
#include "../../../../Engine/Classes/Time.h"
#include "../../../../Engine/Classes/Input.h"
#include "../../../../Engine/Classes/Entity.h"
#include "../../../../Engine/Classes/Raycast.h"
#include "../../../../Engine/Components/Camera.h"
#include "../../../../Engine/Components/MeshRenderer.h"

#define DIFFUSE_BUFFER_BINDING      2
#define SS_BUFFER_BINDING           3          // Specular & Shininess
#define NORMAL_BUFFER_BINDING       4
#define DEPTH_BUFFER_BINDING        5
#define SKYBOX_BUFFER_BINDING       6

#define TEXTURE_TYPE_TO_BINDING(textureType)(static_cast<uint>(textureType) + 2)
#define DIFFUSE_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(TextureType::DIFFUSE)
#define SPECULAR_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(TextureType::SPECULAR)
#define NORMAL_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(TextureType::NORMAL)
#define HEIGHT_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(TextureType::HEIGHT)

#define USE_THREADED_FOR_EACH
#ifdef USE_THREADED_FOR_EACH
    #define FOR_EACH_LOOP tbb::parallel_for_each
#else
    #define FOR_EACH_LOOP std::for_each
#endif

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
            .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::INPUT_ATTACHMENT | ImageUsage::SAMPLED
        });

        // Create diffuse image
        diffuseBuffer = Image::Create({
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight() },
            .format = ImageFormat::R8G8B8A8_UNORM,
            .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::INPUT_ATTACHMENT | ImageUsage::TRANSIENT_ATTACHMENT
        });

        // Create specular and shininess image
        specularAndShininessBuffer = Image::Create({
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight() },
            .format = ImageFormat::R8G8_UNORM,
            .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::INPUT_ATTACHMENT | ImageUsage::TRANSIENT_ATTACHMENT
        });

        // Create normals data image
        normalBuffer = Image::Create({
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight() },
            .format = ImageFormat::R32_UINT, // Using a single channel 32-bit UINT, as the normal (Vector3) is compressed to a UINT with a 15-bit precision
            .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::INPUT_ATTACHMENT | ImageUsage::TRANSIENT_ATTACHMENT
        });

        // Create depth stencil image
        depthBuffer = Image::Create({
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight() },
            .format = VK::GetDevice()->GetBestDepthImageFormat(),
            .usage = ImageUsage::DEPTH_STENCIL_ATTACHMENT | ImageUsage::INPUT_ATTACHMENT | ImageUsage::SAMPLED
        });

        // Create final composition image
        compositionImage = Image::Create({
            .dimensions = { swapchain->GetWidth(), swapchain->GetHeight() },
            .format = ImageFormat::R8G8B8A8_UNORM,
            .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED
        });

        // Create texture sampler to use when passing data to shaders
        bufferSampler = Sampler::Create({ .enableAnisotropy = false, .applyBilinearFiltering = true });

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
            .filePath = Engine::File::OUTPUT_FOLDER_PATH + "Shaders/Deferred/EarlyDepthPass.vert.spv",
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
            .filePath = Engine::File::OUTPUT_FOLDER_PATH + "Shaders/Deferred/G-Buffer.vert.spv",
            .shaderType = ShaderType::VERTEX,
            .vertexAttributes = {
                VertexAttributeType::POSITION,
                VertexAttributeType::NORMAL,
                VertexAttributeType::UV
            }
        });
        auto fragmentShader = Shader::Load({ .filePath = Engine::File::OUTPUT_FOLDER_PATH + "Shaders/Deferred/G-Buffer.frag.spv", .shaderType = ShaderType::FRAGMENT });

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
        vertexShader = Shader::Load({ .filePath = Engine::File::OUTPUT_FOLDER_PATH + "Shaders/Deferred/Composition.vert.spv", .shaderType = ShaderType::VERTEX });
        fragmentShader = Shader::Load({
            .filePath = Engine::File::OUTPUT_FOLDER_PATH + "Shaders/Deferred/Composition.frag.spv",
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

        // Crete skybox cubemap
        skyboxCubemap = Cubemap::Create({ .filePaths = {
            Engine::File::OUTPUT_FOLDER_PATH + "Textures/Skyboxes/Default/skybox_right.png",
            Engine::File::OUTPUT_FOLDER_PATH + "Textures/Skyboxes/Default/skybox_left.png",
            Engine::File::OUTPUT_FOLDER_PATH + "Textures/Skyboxes/Default/skybox_top.png",
            Engine::File::OUTPUT_FOLDER_PATH + "Textures/Skyboxes/Default/skybox_bottom.png",
            Engine::File::OUTPUT_FOLDER_PATH + "Textures/Skyboxes/Default/skybox_front.png",
            Engine::File::OUTPUT_FOLDER_PATH + "Textures/Skyboxes/Default/skybox_back.png",
        }, .cubemapType = CubemapType::SKYBOX });

        // Create modules
        raycaster = Modules::Raycaster::Create({ .IDBuffer = IDBuffer, .depthBuffer = depthBuffer });

        // Create UI-related objects
        renderTimestampQueries.resize(maxConcurrentFrames);
        renderedImageDescriptorSets.resize(maxConcurrentFrames);
        for (uint i = maxConcurrentFrames; i--;)
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
            storageData.objectDatas[meshRenderer.GetMeshID()].modelMatrix = meshRenderer.GetModelMatrix();
            storageData.objectDatas[meshRenderer.GetMeshID()].normalMatrix = meshRenderer.GetNormalMatrix();
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
            static uint currentRenderedImageValue = RendererOutput::RenderedImage;
            static const char* renderedImageValueTypes[] = { "Final Render", "Position Buffer", "Diffuse Buffer", "Specular Buffer", "Shininess Buffer", "Normal Buffer", "Depth Buffer" };

            GUI::Text("Renderer Image Output:");
            GUI::SetNextItemWidthToFit();
            if (GUI::Dropdown("##DEFFERED_VULKAN_RENDERER_RENDERED_IMGAGE_TYPE_DROPDOWN", currentRenderedImageValue,renderedImageValueTypes, 8))
            {
                compositionPipeline->SetSpecializationConstant(0, currentRenderedImageValue);
            }
        }

        // Shading type dropdown
        {
            static uint currentShadingTypeIndex = 0;
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
        VulkanRenderer::Render();

        // Prepare next swapchain image
        swapchain->AcquireNextImage();

        // Get references to usable variables
        auto &commandBuffer = swapchain->GetCurrentCommandBuffer();
        uint currentFrame = swapchain->GetCurrentFrameIndex();
        
        // Begin command buffer
        commandBuffer->Begin(CommandBufferUsage::ONE_TIME_SUBMIT);

        // Bind all vertex & index data
        VK::GetArenaAllocator()->Bind(commandBuffer);

        // Start GPU timer
        renderTimestampQueries[currentFrame]->Begin(commandBuffer);

        // Begin rendering
        deferredRenderPass->Begin(deferredFramebuffer, commandBuffer);

        // Begin rendering early depth pass stage
        earlyDepthPassPipeline->Bind(commandBuffer);

        // Set global buffer data
        earlyDepthPassPipeline->SetShaderBinding(UNIFORM_BUFFER_BINDING, uniformBuffers[currentFrame]);
        earlyDepthPassPipeline->SetShaderBinding(STORAGE_BUFFER_BINDING, storageBuffers[currentFrame]);

        // Define the mesh push constant, which will be modified before each draw call
        BufferPushConstant pushConstant{};

        // Loop through all meshes in the world
        auto enttMeshView = Engine::World::GetAllComponentsOfType<Engine::MeshRenderer>();
        for (auto enttEntity : enttMeshView)
        {
            // Bind mesh
            auto &meshRenderer = enttMeshView.get<Engine::MeshRenderer>(enttEntity);

            // Send push constant data to shader
            pushConstant.meshID = meshRenderer.GetMeshID();
            pushConstant.material = meshRenderer.GetMaterial();
            pushConstant.entityID = static_cast<uint>(meshRenderer.GetEnttEntity());
            pushConstant.meshTexturesPresence = meshRenderer.GetTexturePresence();
            earlyDepthPassPipeline->SetPushConstants(commandBuffer, pushConstant);

            // Set mesh's height map
            earlyDepthPassPipeline->SetShaderBinding(HEIGHT_TEXTURE_BINDING, meshRenderer.GetTexture(TextureType::HEIGHT));

            // Draw mesh
            earlyDepthPassPipeline->DrawMesh(commandBuffer, meshRenderer.GetMesh());
        }

        // End rendering to the depth map
        earlyDepthPassPipeline->End(commandBuffer);

        // Begin rendering G-Buffer
        deferredRenderPass->NextSubpass(commandBuffer);

        // Bind the pipeline, which will draw to the separate G-Buffer images
        bufferPipeline->Bind(commandBuffer);

        // Set global buffer data
        bufferPipeline->SetShaderBinding(UNIFORM_BUFFER_BINDING, uniformBuffers[currentFrame]);
        bufferPipeline->SetShaderBinding(STORAGE_BUFFER_BINDING, storageBuffers[currentFrame]);

        // Loop through all meshes in the world
        for (auto enttEntity : enttMeshView)
        {
            // Bind mesh
            auto &meshRenderer = enttMeshView.get<Engine::MeshRenderer>(enttEntity);

            // Send push constant data to shader
            pushConstant.meshID = meshRenderer.GetMeshID();
            pushConstant.material = meshRenderer.GetMaterial();
            pushConstant.entityID = static_cast<uint>(meshRenderer.GetEnttEntity());
            pushConstant.meshTexturesPresence = meshRenderer.GetTexturePresence();
            bufferPipeline->SetPushConstants(commandBuffer, pushConstant);

            // Set mesh's textures
            for (uint i = static_cast<uint>(TextureType::TOTAL_COUNT); i--;)
            {
                bufferPipeline->SetShaderBinding(TEXTURE_TYPE_TO_BINDING(i), meshRenderer.GetTexture(static_cast<TextureType>(i)));
            }

            // Draw mesh
            bufferPipeline->DrawMesh(commandBuffer, meshRenderer.GetMesh());
        }

        // End G-Buffer pipeline
        bufferPipeline->End(commandBuffer);

        // End rendering to G-Buffer and begin rendering composition image
        deferredRenderPass->NextSubpass(commandBuffer);

        // Bind the pipeline, which will be combining the G-Buffer data into single image
        compositionPipeline->Bind(commandBuffer);

        // Update pipeline's shaders' data
        compositionPipeline->SetShaderBinding(UNIFORM_BUFFER_BINDING, uniformBuffers[currentFrame]);
        compositionPipeline->SetShaderBinding(STORAGE_BUFFER_BINDING, storageBuffers[currentFrame]);
        compositionPipeline->SetShaderBinding(DIFFUSE_BUFFER_BINDING, diffuseBuffer, bufferSampler);
        compositionPipeline->SetShaderBinding(SS_BUFFER_BINDING, specularAndShininessBuffer, bufferSampler);
        compositionPipeline->SetShaderBinding(NORMAL_BUFFER_BINDING, normalBuffer, bufferSampler);
        compositionPipeline->SetShaderBinding(DEPTH_BUFFER_BINDING, depthBuffer, bufferSampler);
        compositionPipeline->SetShaderBinding(SKYBOX_BUFFER_BINDING, skyboxCubemap);
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
        Engine::Entity hoveredEntity = Engine::Entity(raycaster->GetHoveredEntityID());

        // Handle viewport clicking
        if (Engine::Input::GetMouseButtonPressed(MouseButton::LEFT) && !ImGuizmo::IsOver())
        {
            // Update selected entity
            if (hoveredEntity.IsNull())
            {
                using namespace Editor;

                static auto sceneWindowHash = GUI::HashString("Scene View");
                if (GUI::GetHoveredWindow() != nullptr && GUI::GetHoveredWindow()->ID == sceneWindowHash)
                {
                    Engine::World::SetSelectedEntity(hoveredEntity);
                }
            }
            else
            {
                Engine::World::SetSelectedEntity(hoveredEntity);
            }
        }

        // Update hovered world position
        Engine::Raycast::HoveredEntityCallback(hoveredEntity);
        Engine::Raycast::HoveredWorldPositionCallback(raycaster->GetHoveredPosition());

        // Begin the render pass
        swapchain->BeginRenderPass(commandBuffer);

        // Render ImGui UI
        if (Editor::GUI::GetDrawData() != nullptr)
        {
            ImGui_ImplVulkan_RenderDrawData(Editor::GUI::GetDrawData(), commandBuffer->GetVulkanCommandBuffer());
        }

        // End the render pass
        swapchain->EndRenderPass(commandBuffer);

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
        compositionDescriptorSetLayout->Destroy();

        skyboxCubemap->Destroy();
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

        for (uint i = maxConcurrentFrames; i--;)
        {
            uniformBuffers[i]->Destroy();
            storageBuffers[i]->Destroy();
        }

        VulkanRenderer::Destroy();
    }

}