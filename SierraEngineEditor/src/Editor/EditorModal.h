//
// Created by Nikolay Kanchevski on 7.11.24.
//

#pragma once

namespace SierraEngine
{

    class EditorModal
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual bool Draw(Sierra::CommandBuffer& commandBuffer) = 0;

        /* --- COPY SEMANTICS --- */
        EditorModal(const EditorModal&) = delete;
        EditorModal& operator=(const EditorModal&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~EditorModal() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        EditorModal() noexcept = default;

        /* --- POLLING METHODS --- */
        bool BeginWizard(std::string_view title) const noexcept;
        void EndWizard() const noexcept;

        /* --- MOVE SEMANTICS --- */
        EditorModal(EditorModal&&) noexcept = default;
        EditorModal& operator=(EditorModal&&) noexcept = default;

    };

}