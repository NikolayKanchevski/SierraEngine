//
// Created by Nikolay Kanchevski on 7.11.24.
//

#pragma once

namespace SierraEngine
{

    class EditorWizard
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void Draw(bool& open, Sierra::CommandBuffer& commandBuffer, Sierra::ResourceTable& resourceTable) = 0;

        /* --- COPY SEMANTICS --- */
        EditorWizard(const EditorWizard&) = delete;
        EditorWizard& operator=(const EditorWizard&) = delete;

        /* --- MOVE SEMANTICS --- */
        EditorWizard(EditorWizard&&) = delete;
        EditorWizard& operator=(EditorWizard&&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~EditorWizard() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        EditorWizard() noexcept = default;

        /* --- POLLING METHODS --- */
        bool BeginWizard(std::string_view title, bool& open) const noexcept;
        void EndWizard() const noexcept;

    };

}