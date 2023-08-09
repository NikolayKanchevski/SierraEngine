//
// Created by Nikolay Kanchevski on 15.12.22.
//

#pragma once

using namespace Sierra;
using namespace Sierra::Engine;
using namespace Sierra::Rendering;

/* --- SYSTEMS --- */
#include "Handlers/Project.h"
#include "Handlers/Assets/AssetManager.h"

/* --- CLASSES --- */
#include "Classes/RNG.h"
#include "Classes/Time.h"
#include "Classes/File.h"
#include "Classes/Mesh.h"
#include "Classes/Math.h"
#include "Classes/Model.h"
#include "Classes/Input.h"
#include "Classes/Cursor.h"
#include "Classes/Entity.h"
#include "Classes/Binary.h"
#include "Classes/MemoryObject.h"
#include "Classes/SystemInformation.h"

/* --- COMPONENTS --- */
#include "Components/Tag.h"
#include "Components/UUID.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Components/Component.h"
#include "Components/MeshRenderer.h"
#include "Components/Relationship.h"
#include "Components/Light.h"
#include "Components/PointLight.h"
#include "Components/DirectionalLight.h"

/* --- RENDERING --- */
#include "../Core/Rendering/Renderers/DeferredVulkanRenderer.h"
#include "../Core/Rendering/Renderers/ExperimentalVulkanRenderer.h"