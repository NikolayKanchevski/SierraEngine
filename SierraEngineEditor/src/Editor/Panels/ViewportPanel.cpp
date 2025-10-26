//
// Created by Nikolay Kanchevski on 24.10.24.
//

#include "ViewportPanel.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    ViewportPanel::ViewportPanel(const ViewportCreateInfo& createInfo)
        : device(&createInfo.device), renderer(&createInfo.renderer), resourceTable(&createInfo.resourceTable), title(createInfo.title)
    {
        renderTargetID = renderer->CreateRenderTarget({ .width = width, .height = height });
        imageID = resourceTable->BindSampledImage(*renderer->GetRenderTargetImage(renderTargetID));
    }

    /* --- POLLING METHODS --- */

    void ViewportPanel::Draw(bool* const open)
    {
        if (ImGui::Begin(title.data(), open, DEFAULT_WINDOW_FLAGS))
        {
            width = static_cast<uint32>(ImGui::GetContentRegionAvail().x);
            height = static_cast<uint32>(ImGui::GetContentRegionAvail().y);

            resourceTable->FreeSampledImage(imageID);
            imageID = resourceTable->BindSampledImage(*renderer->GetRenderTargetImage(renderTargetID));

            ImGui::Image(ImTextureRef(imageID), { static_cast<float32>(width), static_cast<float32>(height) });
            if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Right))
            {
                constexpr float32 CAMERA_MOVE_SPEED = 0.25f;
                constexpr float32 CAMERA_LOOK_SPEED = 0.20f;

                transform.SetRotation(transform.GetRotation() + Vector3(ImGui::GetIO().MouseDelta.x, -ImGui::GetIO().MouseDelta.y, 0) * CAMERA_LOOK_SPEED);
                if (ImGui::IsKeyDown(ImGuiKey_W)) transform.SetPosition(transform.GetPosition() + transform.GetForwardDirection() * CAMERA_MOVE_SPEED);
                if (ImGui::IsKeyDown(ImGuiKey_S)) transform.SetPosition(transform.GetPosition() + transform.GetBackwardDirection() * CAMERA_MOVE_SPEED);
                if (ImGui::IsKeyDown(ImGuiKey_A)) transform.SetPosition(transform.GetPosition() + transform.GetLeftDirection() * CAMERA_MOVE_SPEED);
                if (ImGui::IsKeyDown(ImGuiKey_D)) transform.SetPosition(transform.GetPosition() + transform.GetRightDirection() * CAMERA_MOVE_SPEED);
                if (ImGui::IsKeyDown(ImGuiKey_E) || ImGui::IsKeyDown(ImGuiKey_Space))    transform.SetPosition(transform.GetPosition() + transform.GetUpDirection() * CAMERA_MOVE_SPEED);
                if (ImGui::IsKeyDown(ImGuiKey_Q) || ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) transform.SetPosition(transform.GetPosition() + transform.GetDownDirection() * CAMERA_MOVE_SPEED);
            }
        }
        ImGui::End();
    }

    /* --- SETTER METHODS --- */

    void ViewportPanel::SetRenderer(SceneRenderer& givenRenderer)
    {
//        if (IsActive()) renderer->DestroyRenderTarget(renderTargetID);

//        renderer = &givenRenderer;
//        renderTargetID = renderer->CreateRenderTarget({ .width = width, .height = height });
    }

    /* --- DESTRUCTOR --- */

    ViewportPanel::~ViewportPanel() noexcept
    {
//        renderer->DestroyRenderTarget(renderTargetID);
    }

    /* --- POLLING METHODS --- */

//    void ViewportPanel::CreateOrResizeImage(const uint32 width, const uint32 height)
//    {
//        bool firstTime = image == nullptr;
//        if (!firstTime && image->GetWidth() == width && height == image->GetHeight()) return;
//
//        constexpr Sierra::ImageUsage REQUIRED_COLOR_IMAGE_USAGE = Sierra::ImageUsage::ColorAttachment | Sierra::ImageUsage::Sample;
//        constexpr Sierra::ImageFormat PREFERRED_COLOR_IMAGE_FORMAT = Sierra::ImageFormat::R8G8B8_UNorm;
//
//        const std::optional<Sierra::ImageFormat> colorFormat = device->GetSupportedImageFormat(PREFERRED_COLOR_IMAGE_FORMAT, REQUIRED_COLOR_IMAGE_USAGE);
//        APP_THROW_IF(!colorFormat.has_value(), Sierra::UnsupportedFeatureError("Cannot create viewport, as device [{0}] does not support any suitable color format"));
//
//        image = device->CreateImage({
//            .name = SR_FORMAT("Image of viewport [{0}]", title),
//            .width = width,
//            .height = height,
//            .format = colorFormat.value(),
//            .usage = REQUIRED_COLOR_IMAGE_USAGE,
//            .sampling = Sierra::ImageSampling::x1
//        });
//
//        constexpr Sierra::ImageUsage REQUIRED_DEPTH_IMAGE_USAGE = Sierra::ImageUsage::DepthAttachment;
//        constexpr Sierra::ImageFormat PREFERRED_DEPTH_IMAGE_FORMAT = Sierra::ImageFormat::D16_UNorm;
//
//        const std::optional<Sierra::ImageFormat> depthFormat = device->GetSupportedImageFormat(PREFERRED_DEPTH_IMAGE_FORMAT, REQUIRED_DEPTH_IMAGE_USAGE);
//        APP_THROW_IF(!depthFormat.has_value(), Sierra::UnsupportedFeatureError("Cannot create viewport, as device [{0}] does not support any suitable depth format"));
//
//        depthImage = device->CreateImage({
//            .name = SR_FORMAT("Depth image of viewport [{0}]", title),
//            .width = width,
//            .height = height,
//            .format = depthFormat.value(),
//            .usage = REQUIRED_DEPTH_IMAGE_USAGE,
//            .sampling = Sierra::ImageSampling::x1
//        });
//
//        if (firstTime) renderTargetID = renderer->CreateRenderTarget({ .templateOutputImage = *image, .depthImage = depthImage.get() });
//        else resourceTable->FreeSampledImage(imageID);

//        imageID = resourceTable->BindSampledImage(*image);
//    }

}