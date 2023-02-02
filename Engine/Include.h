//
// Created by Nikolay Kanchevski on 15.12.22.
//

#pragma once

#define NO_CHANGE std::nullopt

#include "../Core/EngineCore.h"
#include "../Core/Rendering/Vulkan/Renderers/MainVulkanRenderer.h"

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
#include "Classes/RNG.h"

#include "Components/Tag.h"
#include "Components/UUID.h"
#include "Components/Model.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Components/Component.h"
#include "Components/MeshRenderer.h"
#include "Components/Relationship.h"
#include "Components/Lighting/Light.h"
#include "Components/Lighting/PointLight.h"
#include "Components/Lighting/DirectionalLight.h"

using namespace Sierra;
using namespace Sierra::Engine::Classes;
using namespace Sierra::Engine::Components;

using namespace Renderers;
using namespace Sierra::Core;
using namespace Sierra::Core::Rendering;
using namespace Sierra::Core::Rendering::Vulkan;
