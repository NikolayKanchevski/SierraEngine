[![Stand With Ukraine](https://raw.githubusercontent.com/vshymanskyy/StandWithUkraine/main/banner2-direct.svg)](https://bit.ly/3OMysM8)

# Sierra Engine [![License](https://img.shields.io/github/license/NikichaTV/SierraEngine.svg)](https://github.com/NikichaTV/SierraEngine/blob/master/LICENSE)

![Sierra](Media/SierraLogoTextBackground.png?raw=true "Sierra")

<br>

# What is Sierra Engine?

An **open-source** rendering engine based on **[C++ 20](https://en.cppreference.com/w/cpp/20/)**, which aims to provide users with an all-in-one development kit, suited for any graphics-intensive tasks (such as video games, 3D editors, etc.). Built on top of the **[Vulkan](https://www.vulkan.org/)** and [Metal](https://developer.apple.com/metal/) rendering APIs, it provides support for **Windows 7-11**, **macOS**, **Linux**, **Android** and **iOS**, and is planned to also feature other native APIs (like **[DirectX](https://developer.nvidia.com/directx)**), as well as support consoles.

<br>

## 🛠️ Building and Running the Engine

**Compatible compilers:**

The following compilers have been tested and confirmed to be able to successfully build the project. Other ones may still be used, though there could be building issues.

* [![MSVC Compiler Status](https://img.shields.io/badge/MSVC-2019\/2022-blue.svg)](https://visualstudio.microsoft.com/vs/)
* [![Clang LLVM Compiler Status](https://img.shields.io/badge/Clang-LLVM-blue.svg)](https://clang.llvm.org/)
* [![Clang Apple Compiler Status](https://img.shields.io/badge/Clang-Apple-blue.svg)](https://developer.apple.com/xcode/)
* [![GCC Compiler Status](https://img.shields.io/badge/GCC-Unix-blue.svg)](https://gcc.gnu.org/)

<br>

**Requirements:**

In order to build it, you first must make sure you have the following installed (some of these modules may come with your IDE of choice):

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

Detailed information on supported devices/systems can be found [here](Sierra/requirements.md).

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
* [stb](https://github.com/nothings/stb/) - An all-round image loader.
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
* [My Brain](https://ih1.redbubble.net/templateImage.528192883.5730/st,small,845x845-pad,1000x1000,f8f8f8.u9.jpg) - There is not much left of it, actually...

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

<p align="center" id="LineCounter">Total lines of code: 25,746</p>
<p align="center" id="LastUpdated">Last updated: 07/04/2024 </p>

-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------