//
// Created by Nikolay Kanchevski on 8.12.2023.
//

#pragma once

#include "../../src/srpch.h"

#include "../../src/Core/Application.h"
#include "../../src/Core/CursorManager.h"
#include "../../src/Core/EntryPoint.h"
#include "../../src/Core/EventDispatcher.hpp"
#include "../../src/Core/FileManager.h"
#include "../../src/Core/InputManager.h"
#include "../../src/Core/Key.h"
#include "../../src/Core/Logger.h"
#include "../../src/Core/MouseButton.h"
#include "../../src/Core/PlatformContext.h"
#include "../../src/Core/ScopeProfiler.h"
#include "../../src/Core/Screen.h"
#include "../../src/Core/Touch.h"
#include "../../src/Core/TouchManager.h"
#include "../../src/Core/Version.hpp"
#include "../../src/Core/Window.h"
#include "../../src/Core/WindowManager.h"

#include "../../src/Rendering/Buffer.h"
#include "../../src/Rendering/CommandBuffer.h"
#include "../../src/Rendering/ComputePipeline.h"
#include "../../src/Rendering/Device.h"
#include "../../src/Rendering/GraphicsPipeline.h"
#include "../../src/Rendering/Image.h"
#include "../../src/Rendering/RenderingContext.h"
#include "../../src/Rendering/RenderingResource.h"
#include "../../src/Rendering/RenderPass.h"
#include "../../src/Rendering/ResourceTable.h"
#include "../../src/Rendering/Sampler.h"
#include "../../src/Rendering/Shader.h"
#include "../../src/Rendering/Swapchain.h"

#include "../../src/Utilities/FrameLimiter.h"
#include "../../src/Utilities/RNG.h"
#include "../../src/Utilities/Time.h"

#if defined(SR_BUILD_IMGUI_EXTENSION)
    #include "../../src/Extensions/ImGui/ImGuiRenderTask.h"
#endif