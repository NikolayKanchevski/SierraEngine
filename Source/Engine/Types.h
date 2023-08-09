//
// Created by Nikolay Kanchevski on 6.08.23.
//

#pragma once

namespace Sierra::Engine
{

    enum class ModelType
    {
        CUBE = 0,
        CUSTOM = 1
    };

    inline constexpr const char* ModelType_ToString(const ModelType type)
    {
        switch (type)
        {
            case ModelType::CUBE:       return "Cube";
            default:                    return "Custom";
        }
    }

    inline constexpr ModelType ModelType_FromString(const char* string)
    {
        if (StringView(string) == "Cube")           return ModelType::CUBE;
        if (StringView(string) == "Custom")         return ModelType::CUSTOM;
        return ModelType::CUSTOM;
    }

    enum class TextureType
    {
        UNDEFINED = -1,
        DIFFUSE = 0,
        SPECULAR = 1,
        NORMAL = 2,
        HEIGHT = 3,
        TOTAL_COUNT = 4
    };

    inline constexpr const char* TextureType_ToString(const TextureType type)
    {
        switch (type)
        {
            case TextureType::UNDEFINED:        return "Undefined";
            case TextureType::DIFFUSE:          return "Diffuse";
            case TextureType::SPECULAR:         return "Specular";
            case TextureType::NORMAL:           return "Normal";
            case TextureType::HEIGHT:           return "Height";
            default:                            return "Undefined";
        }
    }

    inline constexpr TextureType TextureType_FromString(const char* string)
    {
        if (StringView(string) == "Undefined")      return TextureType::UNDEFINED;
        if (StringView(string) == "Diffuse")        return TextureType::DIFFUSE;
        if (StringView(string) == "Specular")       return TextureType::SPECULAR;
        if (StringView(string) == "Normal")         return TextureType::NORMAL;
        if (StringView(string) == "Height")         return TextureType::HEIGHT;
        return TextureType::UNDEFINED;
    }

    enum class CubemapType
    {
        UNDEFINED = -1,
        SKYBOX = 0
    };

    inline constexpr const char* CubemapType_ToString(const CubemapType type)
    {
        switch (type)
        {
            case CubemapType::UNDEFINED:        return "Undefined";
            case CubemapType::SKYBOX:           return "Skybox";
        }
    }

    inline constexpr CubemapType CubemapType_FromString(const char* string)
    {
        if (StringView(string) == "Skybox")         return CubemapType::SKYBOX;
        return CubemapType::UNDEFINED;
    }

    enum EngineIconType
    {
        DIRECTORY   = 0,
        FILE        = 1,
        PNG         = 2,
        JPG         = 3,
        FBX         = 4,
        TTF         = 5
    };

    inline constexpr EngineIconType EngineIconType_FromFileExtension(const char* extension)
    {
        if (StringView(extension) == ".png")        return EngineIconType::PNG;
        if (StringView(extension) == ".jpg")        return EngineIconType::JPG;
        if (StringView(extension) == ".jpeg")       return EngineIconType::JPG;
        if (StringView(extension) == ".fbx")        return EngineIconType::FBX;
        if (StringView(extension) == ".ttf")        return EngineIconType::TTF;
        return EngineIconType::FILE;
    }

}