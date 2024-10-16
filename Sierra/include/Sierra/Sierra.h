//
// Created by Nikolay Kanchevski on 8.12.2023.
//

#pragma once

#include "../../src/srpch.h"

#include "../../src/Core/Application.h"
#include "../../src/Core/Logging.h"
#include "../../src/Core/Version.h"

#include "../../src/Events/Event.h"
#include "../../src/Events/EventDispatcher.hpp"

#if defined(SR_BUILD_IMGUI_EXTENSION)
    #include "../../src/Extensions/ImGui/ImGuiRenderer.h"
#endif

#include "../../src/Files/FileErrors.h"
#include "../../src/Files/FileManager.h"
#include "../../src/Files/FileStream.h"
#include "../../src/Files/PathErrors.h"

#include "../../src/Platform/EntryPoint.h"
#include "../../src/Platform/PlatformContext.h"

#include "../../src/Utilities/Date.h"
#include "../../src/Utilities/Hash.hpp"
#include "../../src/Utilities/RNG.h"
#include "../../src/Utilities/Time.h"

#include "../../src/Rendering/Buffer.h"
#include "../../src/Rendering/CommandBuffer.h"
#include "../../src/Rendering/ComputePipeline.h"
#include "../../src/Rendering/Device.h"
#include "../../src/Rendering/GraphicsPipeline.h"
#include "../../src/Rendering/Image.h"
#include "../../src/Rendering/Renderer.h"
#include "../../src/Rendering/RenderingContext.h"
#include "../../src/Rendering/RenderingResource.h"
#include "../../src/Rendering/RenderPass.h"
#include "../../src/Rendering/ResourceTable.h"
#include "../../src/Rendering/Sampler.h"
#include "../../src/Rendering/Shader.h"
#include "../../src/Rendering/Swapchain.h"

#include "../../src/Windowing/CursorEvents.h"
#include "../../src/Windowing/CursorManager.h"
#include "../../src/Windowing/InputEvents.h"
#include "../../src/Windowing/InputManager.h"
#include "../../src/Windowing/Key.h"
#include "../../src/Windowing/MouseButton.h"
#include "../../src/Windowing/Screen.h"
#include "../../src/Windowing/ScreenEvents.h"
#include "../../src/Windowing/ScreenOrientation.h"
#include "../../src/Windowing/Touch.h"
#include "../../src/Windowing/TouchEvents.h"
#include "../../src/Windowing/TouchManager.h"
#include "../../src/Windowing/Window.h"
#include "../../src/Windowing/WindowEvents.h"
