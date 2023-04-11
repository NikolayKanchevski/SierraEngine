//
// Created by Nikolay Kanchevski on 25.02.23.
//

#include "../UI/ImGuiCore.h"
#include "RenderingUtilities.h"
#include "../../../Engine/Classes/Input.h"
#include "../../../Engine/Classes/Cursor.h"
#include "../../../Engine/Components/MeshRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

    // ========================== SHADOW MAP RENDERER ========================== \\

    /* --- POLLING METHODS --- */

    void ShadowMapRenderer::Render(const UniquePtr<CommandBuffer> &commandBuffer)
    {
        shadowRenderer->Begin(commandBuffer);

        shadowPipeline->Bind(commandBuffer);

        shadowPipeline->BindDescriptorSets(commandBuffer);

        using namespace Components;
        auto meshMeshView = World::GetAllComponentsOfType<MeshRenderer>();
        auto directionalLightView = World::GetAllComponentsOfType<DirectionalLight>();

        for (const auto &directionalLightEntity : directionalLightView)
        {
            shadowPipeline->GetPushConstantData().directionalLightID = World::GetComponent<DirectionalLight>(directionalLightEntity).GetID();

            for (auto meshEntity : meshMeshView)
            {
                auto &meshRenderer = meshMeshView.get<MeshRenderer>(meshEntity);

                shadowPipeline->GetPushConstantData().meshID = meshRenderer.GetMeshID();
                shadowPipeline->PushConstants(commandBuffer);

                shadowPipeline->DrawMesh(commandBuffer, meshRenderer.GetMesh());
            }
        }

        shadowRenderer->End(commandBuffer);

        commandBuffer->TransitionImageLayout(shadowMap, ImageLayout::SHADER_READ_ONLY_OPTIMAL);


//        varianceRenderer->Begin(commandBuffer);
//
//        variancePipeline->Bind(commandBuffer);
//
//        variancePipeline->BindDescriptorSets(commandBuffer);
//
//        using namespace Components;
//        auto meshMeshView = World::GetAllComponentsOfType<MeshRenderer>();
//        auto directionalLightView = World::GetAllComponentsOfType<DirectionalLight>();
//
//        for (const auto &directionalLightEntity : directionalLightView)
//        {
//            variancePipeline->GetPushConstantData().directionalLightID = World::GetComponent<DirectionalLight>(directionalLightEntity).GetID();
//
//            for (auto meshEntity : meshMeshView)
//            {
//                auto &meshRenderer = meshMeshView.get<MeshRenderer>(meshEntity);
//
//                variancePipeline->GetPushConstantData().meshID = meshRenderer.GetMeshID();
//                variancePipeline->PushConstants(commandBuffer);
//                variancePipeline->DrawMesh(commandBuffer, meshRenderer.GetMesh());
//            }
//        }
//
//        varianceRenderer->End(commandBuffer);
//
//        commandBuffer->TransitionImageLayout(varianceMap, ImageLayout::SHADER_READ_ONLY_OPTIMAL);

//        momentRenderer->Begin(commandBuffer);
//
//        momentPipeline->Bind(commandBuffer);
//
//        momentPipeline->BindDescriptorSets(commandBuffer);
//
//        using namespace Components;
//        auto meshMeshView = World::GetAllComponentsOfType<MeshRenderer>();
//        auto directionalLightView = World::GetAllComponentsOfType<DirectionalLight>();
//
//        for (const auto &directionalLightEntity : directionalLightView)
//        {
//            momentPipeline->GetPushConstantData().directionalLightID = World::GetComponent<DirectionalLight>(directionalLightEntity).GetID();
//
//            for (auto meshEntity : meshMeshView)
//            {
//                auto &meshRenderer = meshMeshView.get<MeshRenderer>(meshEntity);
//
//                momentPipeline->GetPushConstantData().meshID = meshRenderer.GetMeshID();
//                momentPipeline->PushConstants(commandBuffer);
//
//                momentPipeline->DrawMesh(commandBuffer, meshRenderer.GetMesh());
//            }
//        }
//
//        momentRenderer->End(commandBuffer);
//
//        commandBuffer->TransitionImageLayout(momentMap, ImageLayout::SHADER_READ_ONLY_OPTIMAL);


//        // Horizontal blur
//
//        blurRenderer->OverloadColorAttachment(0, horizontalBlurImage);
//        blurRenderer->Begin(commandBuffer);
//
//        blurPipeline->Bind(commandBuffer);
//
//        blurPipeline->GetPushConstantData().blurDirection = BlurDirection::VERTICAL;
//        blurPipeline->PushConstants(commandBuffer);
//
//        blurPipeline->BindDescriptorSets(commandBuffer, horizontalBlurDescriptorSet );
//
//        blurPipeline->Draw(commandBuffer, 3);
//
//        blurRenderer->End(commandBuffer);
//
//        commandBuffer->TransitionImageLayout(horizontalBlurImage, ImageLayout::SHADER_READ_ONLY_OPTIMAL);
//
//        // Vertical blur
//
//        blurRenderer->OverloadColorAttachment(0, finalBlurImage);
//        blurRenderer->Begin(commandBuffer);
//
//        blurPipeline->Bind(commandBuffer);
//
//        blurPipeline->GetPushConstantData().blurDirection = BlurDirection::HORIZONTAL;
//        blurPipeline->PushConstants(commandBuffer);
//
//        blurPipeline->BindDescriptorSets(commandBuffer, finalBlurDescriptorSet);
//
//        blurPipeline->Draw(commandBuffer, 3);
//
//        blurRenderer->End(commandBuffer);
    }

    void ShadowMapRenderer::Update()
    {
        using namespace Components;

        auto directionalLightEntities = World::GetAllComponentsOfType<DirectionalLight>();
        auto &storageData = shadowPipeline->GetStorageBufferData();
        tbb::parallel_for_each(directionalLightEntities.begin(), directionalLightEntities.end(), [&storageData](auto &enttEntity)
        {
            DirectionalLight &directionalLight = World::GetComponent<DirectionalLight>(enttEntity);
            storageData.directionalLights[directionalLight.GetID()].projectionView = directionalLight.GetViewSpaceMatrix();
        });
    }

    /* --- DESTRUCTOR --- */

    void ShadowMapRenderer::Destroy()
    {
        blurPipeline->Destroy();
        blurRenderer->Destroy();

        horizontalBlurImage->Destroy();
        finalBlurImage->Destroy();

        momentPipeline->Destroy();
        momentRenderer->Destroy();
        momentMap->Destroy();

        variancePipeline->Destroy();
        varianceRenderer->Destroy();
        varianceMap->Destroy();

        shadowPipeline->Destroy();
        shadowRenderer->Destroy();
        shadowMap->Destroy();

        sampler->Destroy();
    }

    // ========================== GRID RENDERER ========================== \\

    /* --- POLLING METHODS --- */

    void GridRenderer::RenderGrid(const UniquePtr<CommandBuffer> &commandBuffer)
    {
        // Make sure image is suitable for being rendering to
//        commandBuffer->TransitionImageLayout(outputImage, ImageLayout::COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
//
//        // Begin rendering
//        renderer->Begin(commandBuffer);
//
//        // Fire the pipeline up
//        pipeline->Bind(commandBuffer);
//
//        // Pass data to shaders
//        pipeline->BindDescriptorSets(commandBuffer);
//
//        // Draw grid plane
//        pipeline->Draw(commandBuffer, 6);
//
//        // End rendering
//        renderer->End(commandBuffer);
    }

    /* --- DESTRUCTOR --- */

    void GridRenderer::Destroy()
    {
        pipeline->Destroy();
        renderer->Destroy();
    }

    // ========================== RAYCASTER ========================== \\

    /* --- POLLING METHODS --- */

    void Raycaster::UpdateData(const UniquePtr<CommandBuffer> &commandBuffer)
    {
        using Sierra::Engine::Classes::Input;
        using Sierra::Engine::Classes::Cursor;
        using namespace Sierra::Core::Rendering::UI;

        // Set mouse position, which will later be used as UV in shader
        if (Window::IsFocusedWindowPresent())
        {
            Vector2 mousePositionWithinView = Cursor::GetGlfwCursorPosition();
            mousePositionWithinView.x -= ImGuiCore::GetSceneViewPositionX();
            mousePositionWithinView.x /= ImGuiCore::GetSceneViewWidth();
            mousePositionWithinView.y -= ImGuiCore::GetSceneViewPositionY();
            mousePositionWithinView.y /= ImGuiCore::GetSceneViewHeight();
            mousePositionWithinView.y = 1.0f - mousePositionWithinView.y;
            computePipeline->GetPushConstantData().mousePosition = mousePositionWithinView;
        }
        else
        {
            computePipeline->GetPushConstantData().mousePosition = { -1, -1 };
        }

        // Start pipeline
        computePipeline->Bind(commandBuffer);

        // Pass data to compute shader
        computePipeline->PushConstants(commandBuffer);
        computePipeline->BindDescriptorSets(commandBuffer);

        // Execute compute shader
        computePipeline->Dispatch(commandBuffer, 1);

        // Save new data
        data = *dataBuffer->GetDataAs<WriteBuffer>();
        data.worldPosition.y *= -1;
    }

    /* --- DESTRUCTOR --- */

    void Raycaster::Destroy()
    {
        computePipeline->Destroy();
        dataBuffer->Destroy();
    }

}