//
// Created by Nikolay Kanchevski on 15.12.22.
//

#pragma once

using namespace Sierra;
using namespace Sierra::Engine;
using namespace Sierra::Rendering;

/* --- CLASSES --- */
#include "Classes/RNG.h"
#include "Classes/Time.h"
#include "Classes/File.h"
#include "Classes/Mesh.h"
#include "Classes/Math.h"
#include "Classes/Input.h"
#include "Classes/Cursor.h"
#include "Classes/Entity.h"
#include "Classes/Binary.h"
#include "Classes/Stopwatch.h"
#include "Classes/MemoryObject.h"
#include "Classes/SystemInformation.h"

/* --- COMPONENTS --- */
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

/* --- RENDERING --- */
#include "../Core/Rendering/Vulkan/Renderers/DeferredVulkanRenderer.h"
#include "../Core/Rendering/Vulkan/Renderers/ExperimentalVulkanRenderer.h"