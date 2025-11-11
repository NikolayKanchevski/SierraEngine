//
// Created by Nikolay Kanchevski on 27.10.25.
//

#pragma once

namespace SierraEngine
{

    struct RenderingContextCreateInfo
    {
        const Sierra::RenderingInstance& renderingInstance;
    };

    class SIERRA_ENGINE_API RenderingContext final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit RenderingContext(const RenderingContextCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Bind(Sierra::CommandBuffer& commandBuffer) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const Sierra::Device& GetDevice() const noexcept { return *device; }

        [[nodiscard]] Sierra::ResourceTable& GetResourceTable() const noexcept { return *resourceTable; }
        [[nodiscard]] Sierra::DestructionScheduler& GetDestructionScheduler() const noexcept { return *destructionScheduler; }

        /* --- COPY SEMANTICS --- */
        RenderingContext(const RenderingContext&) = delete;
        RenderingContext& operator=(const RenderingContext&) = delete;

        /* --- MOVE SEMANTICS --- */
        RenderingContext(RenderingContext&&) noexcept = default;
        RenderingContext& operator=(RenderingContext&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~RenderingContext() noexcept = default;

    private:
        std::unique_ptr<Sierra::Device> device = nullptr;
        std::unique_ptr<Sierra::ResourceTable> resourceTable = nullptr;
        std::unique_ptr<Sierra::DestructionScheduler> destructionScheduler = nullptr;

    };

}