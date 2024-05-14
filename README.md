[![Stand With Ukraine](https://raw.githubusercontent.com/vshymanskyy/StandWithUkraine/main/banner2-direct.svg)](https://bit.ly/3OMysM8)

# Sierra Engine [![License](https://img.shields.io/github/license/NikichaTV/SierraEngine.svg)](https://github.com/NikichaTV/SierraEngine/blob/master/LICENSE)

![Sierra](Media/SierraLogoTextBackground.png "Sierra")

<br>

# What is Sierra Engine?

An **open-source** rendering engine based on **[C++20](https://en.cppreference.com/w/cpp/20/)**, which aims to provide users with an all-in-one development kit, suited for any graphics-intensive tasks (such as video games, 3D editors, etc.). Built on top of the **[Vulkan](https://www.vulkan.org/)** and [Metal](https://developer.apple.com/metal/) rendering APIs, it provides support for **Windows 7-11**, **macOS**, **Linux**, **Android** and **iOS**, and is planned to also feature other native APIs (like **[DirectX](https://developer.nvidia.com/directx)**), as well as support consoles.

<br>

## 🛠️ Building and Running the Engine

**Compatible compilers:**

The following compilers have been tested and confirmed to be able to successfully build the project. Other ones may still be used, though there could be building issues.

* [![MSVC Compiler Status](https://img.shields.io/badge/MSVC-2019\/2022-blue.svg)](https://visualstudio.microsoft.com/vs/)
* [![Clang LLVM Compiler Status](https://img.shields.io/badge/Clang-LLVM-blue.svg)](https://clang.llvm.org/)
* [![Clang Apple Compiler Status](https://img.shields.io/badge/Clang-Apple-blue.svg)](https://developer.apple.com/xcode/)
* [![GCC Compiler Status](https://img.shields.io/badge/GCC-Unix-blue.svg)](https://gcc.gnu.org/)

<br>

**Prerequisites:**

In order to build it, you must have the following modules installed (some of them may come bundled with your IDE of choice):

<br>

* **[Git](https://git-scm.com/downloads/)**
* A C++ compiler (preferably **[CLang](https://clang.llvm.org)** or **[MSVC](https://visualstudio.microsoft.com/downloads/)**)
* **[CMake](https://cmake.org/download/)** 3.20 or higher
* The **[VulkanSDK](https://vulkan.lunarg.com)**
* **[Python](https://www.python.org/downloads/)**

<br>

**Installation:**

Simply clone the repository either directly from your favourite IDE (<b><a href="https://visualstudio.microsoft.com">Visual Studio</a></b>, <b><a href="https://www.jetbrains.com/clion/">CLion</a></b>, etc.), or by opening the command line and running:

```bat
$ git clone --recursive https://github.com/NikichaTV/SierraEngine
```

<br>

If you clone the repository directly from an IDE, you may need to also run this:

```bat
$ git submodule update --init --recursive
```

And there you have it! You can now create your very own application using the Sierra API, which is contained in the [Sierra](https://github.com/NikichaTV/SierraEngine/tree/842946e5840b1bd03f32df8e2f2da3b6ae46ff2d/Sierra) subdirectory. Unfortunately, there is not an existing editor, however, rest assured, one is on its way!

Tip: In order to get a better grasp of how the API works, you can always take a look at the [Sandbox](https://github.com/NikichaTV/SierraEngine/tree/842946e5840b1bd03f32df8e2f2da3b6ae46ff2d/Sandbox) project, which has a few ready-to-use examples.

<br>

## 💻 System Requirements

Minimal system requirements for every supported platform of the [Sierra API](https://github.com/NikichaTV/SierraEngine/tree/master/Sierra/) are listed here. Built on top of it, the whole [Sierra Engine](https://github.com/NikichaTV/SierraEngine/) shares the exact same requirements. *Do note that the following information is a subject to change.*

## Minimal System Requirements

Complying with the following requirements guarantees that at least one graphics API backend is supported, hence the [Sierra API](https://github.com/NikichaTV/SierraEngine/tree/master/Sierra/) is supported as well. However, the usage of the only available API will be enforced.

<br>

### Minimum Operating System Version

|    **[Windows](www.microsoft.com/en-us/windows/)**    | **[Linux](https://www.linux.org/)** |                        **[macOS](www.apple.com/macos/)**                        |             **[Android](https://www.android.com/)**              |          **[iOS](www.apple.com/ios/)**           |
|:-----------------------------------------------------:|:-----------------------------------:|:-------------------------------------------------------------------------------:|:----------------------------------------------------------------:|:------------------------------------------------:|
| [Windows 7](https://bg.wikipedia.org/wiki/Windows_7/) | Any [Linux](https://www.linux.org/) | [macOS 10.13.0 (High Sierra)](https://en.wikipedia.org/wiki/MacOS_High_Sierra/) | [Android 13.0](https://developer.android.com/about/versions/13/) | [iOS 13.0](https://en.wikipedia.org/wiki/IOS_13) |

<br>

### Minimal GPU

|                      [AMD](https://www.amd.com/en.html/)                      |                             [NVIDIA](https://www.nvidia.com/en-us/)                              |             [Intel](https://www.intel.com/content/www/us/en/homepage.html)             |                   **[Android](https://www.android.com/)**                   |                                                                     [Apple]([Apple](www.apple.com/))                                                                      |
|:-----------------------------------------------------------------------------:|:------------------------------------------------------------------------------------------------:|:--------------------------------------------------------------------------------------:|:---------------------------------------------------------------------------:|:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|
| Radeon HD 7970 ([GCN](https://en.wikipedia.org/wiki/Graphics_Core_Next/) 1.0) | GeForce GeForce GT 630 OEM ([Kepler](https://en.wikipedia.org/wiki/Kepler_(microarchitecture)/)) | HD Graphics 2000 [Skylake](https://en.wikipedia.org/wiki/Skylake_(microarchitecture)/) | Any [Android 13.0](https://developer.android.com/about/versions/13/) device | [Apple Silicon](https://bg.wikipedia.org/wiki/Apple_Silicon/) ([M1 Chip](https://en.wikipedia.org/wiki/Apple_M1/)) / [Apple A13](https://en.wikipedia.org/wiki/Apple_A13) |

<br>

## Graphics API Support

Below are listed the minimal requirements for every graphics API backend separately. Some systems might support multiple APIs, meaning that their respective user would be free to change their rendering backend at any time.

<br>

### Minimum Graphics API Version

|                                                      | **[Windows](www.microsoft.com/en-us/windows/)** | **[Linux](https://www.linux.org/)** | **[macOS](www.apple.com/macos/)** | **[Android](https://www.android.com/)** | **[iOS](www.apple.com/ios/)** |
|:----------------------------------------------------:|:-----------------------------------------------:|:-----------------------------------:|:---------------------------------:|:---------------------------------------:|:-----------------------------:|
|        **[Vulkan](https://www.vulkan.org/)**         |                       1.2                       |                 1.2                 |                 ❌                 |                   1.2                   |               ❌               |
| **[DirectX](https://developer.nvidia.com/directx/)** |                        ❌                        |                  ❌                  |                 ❌                 |                    ❌                    |               ❌               |
|   **[Metal](https://developer.apple.com/metal/)**    |                        ❌                        |                  ❌                  |                3.0                |                    ❌                    |              3.0              |
|    **[OpenGL](https://www.khronos.org/opengl/)**     |                        ❌                        |                  ❌                  |                 ❌                 |                    ❌                    |               ❌               |

<br>

### Minimal GPU (per Graphics API)

|                                                      |                      [AMD](https://www.amd.com/en.html/)                      |                             [NVIDIA](https://www.nvidia.com/en-us/)                              |             [Intel](https://www.intel.com/content/www/us/en/homepage.html)             |                   **[Android](https://www.android.com/)**                   |                                                                          [Apple](www.apple.com/)                                                                          |
|:----------------------------------------------------:|:-----------------------------------------------------------------------------:|:------------------------------------------------------------------------------------------------:|:--------------------------------------------------------------------------------------:|:---------------------------------------------------------------------------:|:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|
|        **[Vulkan](https://www.vulkan.org/)**         | Radeon HD 7970 ([GCN](https://en.wikipedia.org/wiki/Graphics_Core_Next/) 1.0) | GeForce GeForce GT 630 OEM ([Kepler](https://en.wikipedia.org/wiki/Kepler_(microarchitecture)/)) | HD Graphics 2000 [Skylake](https://en.wikipedia.org/wiki/Skylake_(microarchitecture)/) | Any [Android 13.0](https://developer.android.com/about/versions/13/) device |                                                                                     ❌                                                                                     |
| **[DirectX](https://developer.nvidia.com/directx/)** |                                       ❌                                       |                                                ❌                                                 |                                           ❌                                            |                                      ❌                                      |                                                                                     ❌                                                                                     |
|   **[Metal](https://developer.apple.com/metal/)**    |                                       ❌                                       |                                                ❌                                                 |                                           ❌                                            |                                      ❌                                      | [Apple Silicon](https://bg.wikipedia.org/wiki/Apple_Silicon/) ([M1 Chip](https://en.wikipedia.org/wiki/Apple_M1/)) / [Apple A13](https://en.wikipedia.org/wiki/Apple_A13) |
|    **[OpenGL](https://www.khronos.org/opengl/)**     |                                       ❌                                       |                                                ❌                                                 |                                           ❌                                            |                                      ❌                                      |                                                                                     ❌                                                                                     |

<br>

## 💫 Features

```c++
// TODO: Add to this section :D
```

<br>

## 📄 Documentation

Documentation has not yet been published. The website that will be hosting it, however, is already done. Someone just needs to take the time to write and upload it, but this will be postponed for when a stable version of the project has been released, which is scheduled for the distant future.

<br>

## 🤓️ About

**Frameworks used:**

Within Engine:
* [entt](https://github.com/skypjack/entt) - Efficient entity component system, utilized for scenes.
* [stb](https://github.com/nothings/stb/) - An all-round outputImage loader.
* [KTX Software](https://github.com/KhronosGroup/KTX-Software) - Used to compress texture assets.
* [ShaderConnect](https://github.com/NikichaTV/ShaderConnect/tree/sierra) - A special branch of [NikichaTV](https://github.com/NikichaTV)'s framework for shader cross-compilation.

Within API:
* [GLM](https://github.com/g-truc/glm/) - Provides the engine with a powerful set of tools for solving linear algebra problems.
* [spdlog](https://github.com/gabime/spdlog) - Used for fast, asynchronous debug-only logging.
* [Vulkan](https://www.vulkan.org/) - Allows for incredibly fast rendering on a wide variety of platforms.
* [VMA](https://gpuopen.com/vulkan-memory-allocator/) - For optimal memory management of [Vulkan](https://www.vulkan.org/) objects.
* [Metal](https://developer.apple.com/metal/) - Being their native rendering API, it is used for fast, hardware-accelerated rendering on [Apple](https://www.apple.com) platforms.
* [ios-cmake](https://github.com/leetal/ios-cmake) - Essential for building the engine on iOS.
* [ImGui](https://github.com/ocornut/imgui) - Used in the ImGui UI extension.
* [My Brain](https://ih1.redbubble.net/templateOutputImage.528192883.5730/st,small,845x845-pad,1000x1000,f8f8f8.u9.jpg) - There is not much left of it, actually...

<br>

**Software used:**

* [JetBrains CLion](https://www.jetbrains.com/clion/) - Primary IDE of the project.
* [JetBrains Rider](https://www.jetbrains.com/rider/) - Another IDE which was used to create the .NET 6.0 prototype version of the engine.
* [Xcode](https://developer.apple.com/xcode/) - Used to deploy the iOS version on mobile [Apple](https://www.apple.com) devices.
* [Android Studio](https://developer.android.com/studio/) - Deployment of [Android](https://www.android.com) version.
* [Blender](https://www.blender.org/) - Managing and testing 3D models.
* [Trello](https://trello.com/b/RMYtZPOg/sierra-engine/) - For pretending to have an organized list of things to implement next.

<br>

---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

<p align="center" id="LineCounter">Total lines of code: 25,407</p>
<p align="center" id="LastUpdated">Last updated: 15/05/2024 </p>

-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------