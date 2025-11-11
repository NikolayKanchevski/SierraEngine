//
// Created by Nikolay Kanchevski on 13.10.25.
//

#pragma once

namespace SierraEngine
{

    struct ImagePreviewInfo
    {
        const RenderingContext& renderingContext;
        Sierra::CommandBuffer& commandBuffer;
        const LoadedImage& image;
    };

    struct ImagePreview
    {
        Sierra::SampledImageID ID = { };
        std::unique_ptr<Sierra::Image> image = nullptr;
    };

    namespace ImagePreviewer
    {
        [[nodiscard]] std::optional<ImagePreview> Preview(const ImagePreviewInfo& previewInfo);
    }

}