//
// Created by Nikolay Kanchevski on 29.01.23.
//

#pragma once

#ifdef __cplusplus
    // === Standard Library == //
    #include <iostream>
    #include <algorithm>
    #include <functional>
    #include <memory>
    #include <thread>
    #include <utility>
    #include <cstdint>
    #include <array>
    #include <vector>
    #include <map>
    #include <unordered_map>
    #include <set>
    #include <unordered_set>
    #include <exception>
    #include <execution>
    #include <random>
    #include <regex>
    #include <chrono>
    #include <mutex>
    #include <shared_mutex>
    #include <future>
    #include <string>
    #include <cstring>
    #include <string_view>
    #include <sstream>
    #include <stack>
    #include <deque>
    #include <optional>
    #include <fstream>
    #include <filesystem>
    #include <bitset>
    #include <any>
    #include <cstdio>

    #define NOMINMAX
    #include <limits>

    #if defined _MSC_VER
        #include <direct.h>
    #elif defined __GNUC__
        #include <sys/types.h>
        #include <sys/stat.h>
    #endif

    #if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
        #include <unistd.h>
    #endif

    #if __APPLE__
        #include "TargetConditionals.h"
    #endif

    // === External Libraries == //

    // '-- Vulkan
    #if __APPLE__
        // For VkPhysicalDevicePortabilitySubsetFeaturesKHR
        #define VK_ENABLE_BETA_EXTENSIONS
    #endif
    #include <volk.h>
    #include <vulkan/vulkan.hpp>
    #include <vulkan/vulkan_to_string.hpp>

    // '-- Shaderc
    #include <shaderc/shaderc.hpp>

    // '- SPIRV Utilities
    #include <spirv_reflect.h>

    // '-- GLFW
    #include <GLFW/glfw3.h>

    // '-- GLM
    #define GLM_DEPTH_ZERO_TO_ONE
    #include <glm/glm.hpp>
    #include <glm/gtc/type_ptr.hpp>
    #include <glm/gtx/quaternion.hpp>
    #include <glm/gtx/euler_angles.hpp>
    #include <glm/ext/matrix_transform.hpp>
    #include <glm/ext/matrix_clip_space.hpp>
    #include <glm/gtx/matrix_decompose.hpp>

    // '-- ImGui
    #define IMGUI_DEFINE_MATH_OPERATORS
    #include <imgui.h>
    #include <imgui_internal.h>
    #define VK_NO_PROTOTYPES
    #define IMGUI_IMPL_VULKAN_NO_PROTOTYPES
    #include <imgui_stdlib.h>
    #include <imgui_impl_glfw.h>
    #include <imgui_impl_vulkan.h>

    // '-- ImGuizmo
    #include <ImGuizmo.h>

    // '-- ENTT
    #include <entt/entt.hpp>

    // '-- VMA
    // Needs VMA_IMPLEMENTATION defined in CPP
    // Needs VMA_STATS_STRING_ENABLED 0 defined in CPP
    // Needs VMA_STATIC_VULKAN_FUNCTIONS 0 defined in CPP
    // Needs VMA_DYNAMIC_VULKAN_FUNCTIONS 1 defined in CPP
    #include <vk_mem_alloc.h>

    // '-- Assimp
    #include <assimp/scene.h>
    #include <assimp/Importer.hpp>
    #include <assimp/postprocess.h>

    // '-- STB
    // Needs STB_IMAGE_IMPLEMENTATION defined in CPP
    #include <stb_image.h>

    // '-- FMT
    #include <fmt/format.h>

    // '-- RareCpp
    #include <reflect.h>

    // '-- Infoware
    #include <infoware/system.hpp>
    #include <infoware/version.hpp>
    #include <infoware/cpu.hpp>
    #include <infoware/pci.hpp>

    // '-- TBB
    #include <tbb/parallel_for.h>
    #include <tbb/parallel_for_each.h>
    #include <tbb/concurrent_vector.h>
    #include <tbb/concurrent_queue.h>
    #include <tbb/concurrent_priority_queue.h>
    #include <tbb/concurrent_map.h>
    #include <tbb/concurrent_unordered_map.h>
    #include <tbb/concurrent_hash_map.h>
    #include <tbb/concurrent_set.h>
    #include <tbb/concurrent_unordered_set.h>

    // '-- Whereami
    #include <whereami.h>

    // '-- Discord SDK
    #include <discord.h>

    // === Custom Types === //
    #include "Types.h"
    #include "../Rendering/Vulkan/VulkanTypes.h"

    // === Custom Utilities == //
    #include "Macros.h"
    #include "Definitions.h"
    #include "../Debugger.h"
    #include "../World.h"
#endif