//
// Created by Nikolay Kanchevski on 15.12.22.
//

#pragma once

#include "../Core/World.h"

#include "Structures/Vertex.h"
#include "Structures/Material.h"

#include "Classes/Time.h"
#include "Classes/Input.h"
#include "Classes/File.h"
#include "Classes/Mesh.h"
#include "Classes/Cursor.h"
#include "Classes/Entity.h"
#include "Classes/Stopwatch.h"
#include "Classes/Math.h"
#include "Classes/Binary.h"

#include "Components/Camera.h"
#include "Components/Model.h"
#include "Components/Component.h"
#include "Components/InternalComponents.h"
#include "Components/MeshRenderer.h"
#include "Components/Lighting/PointLight.h"
#include "Components/Lighting/Light.h"
#include "Components/Lighting/DirectionalLight.h"

#include "../Core/Rendering/Vulkan/Renderer/VulkanRenderer.h"

using namespace Sierra::Engine::Classes;
using namespace Sierra::Engine::Components;

using namespace Sierra::Core;
using namespace Sierra::Core::Rendering;
using namespace Sierra::Core::Rendering::Vulkan;
