//
// Created by Nikolay Kanchevski on 13.05.24.
//

#pragma once

#include "../../src/srepch.h"

#include "../../src/Core/Application.h"
#include "../../src/Core/FrameLimiter.h"
#include "../../src/Core/ThreadPool.h"

#include "../../src/Rendering/ArenaAllocator.h"
#include "../../src/Rendering/Mesh.h"
#include "../../src/Rendering/SceneRenderer.h"
#include "../../src/Rendering/Vertex.h"

#include "../../src/Assets/Asset.h"
#include "../../src/Assets/AssetHeader.h"
#include "../../src/Assets/AssetID.h"
#include "../../src/Assets/AssetImporter.h"
#include "../../src/Assets/AssetManager.h"
#include "../../src/Assets/AssetMetadata.h"
#include "../../src/Assets/AssetSerializer.h"

#include "../../src/Assets/Materials/MaterialImporter.h"
#include "../../src/Assets/Materials/MaterialSerializer.h"

#include "../../src/Assets/Textures/ImageAccessor.h"
#include "../../src/Assets/Textures/ImageCompressor.h"
#include "../../src/Assets/Textures/ImageConverter.h"
#include "../../src/Assets/Textures/ImageLoader.h"
#include "../../src/Assets/Textures/ImageTranscoder.h"
#include "../../src/Assets/Textures/TextureImporter.h"
#include "../../src/Assets/Textures/TextureSerializer.h"

#include "../../src/Assets/Textures/Compressors/BasisUniversalCompressor.h"

#include "../../src/Assets/Textures/Transcoders/BasisUniversalTranscoder.h"

#include "../../src/Scene/Component.h"
#include "../../src/Scene/Components.h"
#include "../../src/Scene/EntityID.h"
#include "../../src/Scene/Scene.h"

#include "../../src/Scene/Components/Camera.h"
#include "../../src/Scene/Components/MeshRenderer.h"
#include "../../src/Scene/Components/Relationship.h"
#include "../../src/Scene/Components/Tag.h"
#include "../../src/Scene/Components/Transform.h"
