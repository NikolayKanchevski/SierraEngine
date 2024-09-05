//
// Created by Nikolay Kanchevski on 5.07.24.
//

#pragma once

namespace SierraEngine
{

    enum class AssetType : uint8
    {
        Unknown,
        Texture
    }; 

    class SIERRA_ENGINE_API Asset
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual AssetType GetType() const = 0;

        /* --- COPY SEMANTICS --- */
        Asset(const Asset&) = delete;
        Asset& operator=(const Asset&) = delete;

        /* --- MOVE SEMANTICS --- */
        Asset(Asset&&) = default;
        Asset& operator=(Asset&&) = default;

        /* --- DESTRUCTOR --- */
        virtual ~Asset() = default;

    protected:
        Asset() = default;

    };

}