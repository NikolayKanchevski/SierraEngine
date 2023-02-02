//
// Created by Nikolay Kanchevski on 29.01.23.
//

#pragma once

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
#include <stack>
#include <deque>
#include <optional>
#include <fstream>
#include <filesystem>
#include <bitset>
#define NOMINMAX
#include <limits>

// === External Libraries == //

// '-- Vulkan
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

// '-- GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/matrix_decompose.hpp>

// '-- GLFW
#include <GLFW/glfw3.h>

// '-- ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

// '-- ImGuizmo
#include <ImGuizmo.h>

// '-- Assimp
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

// '-- STB
// Needs STB_IMAGE_IMPLEMENTATION defined in CPP
#include <stb_image.h>

// '-- ENTT
#include <entt/entt.hpp>

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

// '-- FMT
#include <fmt/format.h>

// '-- VMA
// Needs VMA_IMPLEMENTATION defined in CPP
// Needs VMA_STATS_STRING_ENABLED 0 defined in CPP
// Needs VMA_STATIC_VULKAN_FUNCTIONS 0 defined in CPP
// Needs VMA_DYNAMIC_VULKAN_FUNCTIONS 1 defined in CPP
#include <vk_mem_alloc.h>

// === Custom Types === //
#include "Types.h"
#include "../Rendering/Vulkan/VulkanTypes.h"

// === Custom Utilities == //
#include "../Debugger.h"
#include "../World.h"
