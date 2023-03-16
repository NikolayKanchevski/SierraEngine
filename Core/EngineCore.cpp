//
// Created by Nikolay Kanchevski on 20.12.22.
//

#include "EngineCore.h"

#include "Rendering/Vulkan/VK.h"
#include "Rendering/Vulkan/Abstractions/Shader.h"
#include "Rendering/Vulkan/Abstractions/Texture.h"
#include "../Engine/Components/Model.h"
#include "../Engine/Classes/SystemInformation.h"
#include "../Engine/Classes/File.h"

using Sierra::Engine::Classes::Model;
using Sierra::Engine::Classes::SystemInformation;
using Sierra::Core::Rendering::Vulkan::VK;

namespace Sierra
{

    /* --- POLLING METHODS --- */

    void EngineCore::Initialize()
    {
        glfwInit();

        File::Start();
        SystemInformation::Start();
        VK::Initialize({ .sampleRateShading = VK_TRUE, .fillModeNonSolid = VK_TRUE, .samplerAnisotropy = VK_TRUE });
    }

    void EngineCore::Terminate()
    {
        Shader::DisposePool();
        Texture::DestroyDefaultTextures();
        Texture::DisposePool();
        Model::DisposePool();
        VK::Destroy();

        glfwTerminate();
    }

    /* --- SETTER METHODS --- */

    void EngineCore::SetSelectedEntity(Entity givenEntity)
    {
        selectedEntity = givenEntity;
    }

    void EngineCore::SetMouseHoveredPosition(Vector3 givenPosition)
    {
        mouseHoveredPosition = givenPosition;
    }
}

