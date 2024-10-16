//
// Created by Nikolay Kanchevski on 5.07.24.
//

#pragma once

namespace SierraEngine
{

    enum class AssetType : bool
    {
        Unknown,
        Texture
    }; 

    class SIERRA_ENGINE_API Asset
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual AssetType GetType() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        Asset(const Asset&) = delete;
        Asset& operator=(const Asset&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Asset() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        Asset() noexcept = default;

        /* --- MOVE SEMANTICS --- */
        Asset(Asset&&) noexcept = default;
        Asset& operator=(Asset&&) noexcept = default;

    };

}