[![Stand With Ukraine](https://raw.githubusercontent.com/vshymanskyy/StandWithUkraine/main/banner2-direct.svg)](https://bit.ly/3OMysM8)

# Sierra Engine [![License](https://img.shields.io/github/license/NikichaTV/SierraEngine.svg)](https://github.com/NikichaTV/SierraEngine/blob/master/LICENSE)

![Sierra](Media/SierraLogoTextBackground.png?raw=true "Sierra")

<br>

# What is Sierra Engine?

An **open-source** rendering engine based on **[C++17](https://en.cppreference.com/w/cpp/17/)**, which aims to provide users with an all-in-one development kit, suited for any graphics-intensive tasks (such as video games, 3D editors, etc.). Built on top of the **[Vulkan](https://www.vulkan.org/)** and [Metal](https://developer.apple.com/metal/) rendering APIs, it provides support for **Windows 7-11**, **macOS**, **Linux**, **Android** and **iOS**, but is planned to also feature other native APIs (like **[DirectX](https://developer.nvidia.com/directx)**) and support consoles.

<br>

## 🛠️ Building and Running the Engine

**Compatible compilers:**

These are the compilers, which are tested and confirmed to be able to build the project. Some other compilers may still be used, though there could be issues.

* [![MSVC Compiler Status](https://img.shields.io/badge/MSVC-2019\/2022-blue.svg)](https://visualstudio.microsoft.com/vs/)
* [![Clang LLVM Compiler Status](https://img.shields.io/badge/Clang-LLVM-blue.svg)](https://clang.llvm.org/)
* [![Clang Apple Compiler Status](https://img.shields.io/badge/Clang-Apple-blue.svg)](https://developer.apple.com/xcode/)
* [![GCC Compiler Status](https://img.shields.io/badge/GCC-Unix-blue.svg)](https://gcc.gnu.org/)

<br>

**Requirements:**

In order to build it, you first must make sure you have the following installed (some of these modules may come with your IDE of choice):

<br>

* **[Git](https://git-scm.com/downloads/)**
* A C/C++ compiler (preferably **[CLang](https://clang.llvm.org)** or **[MSVC](https://visualstudio.microsoft.com/downloads/)**)
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

And there you have it! You can now create your very own application or play around with the engine itself.

<br>

## 💫 Features

```c++
// TODO: Add to this section :D
```

## 📄 Documentation

Wondering how to use the engine? You can check out the online documentation [here](https://sierra.nikichatv.com/Documentation/).

<br>

## 🔮 What's to come?

- [ ] HDR
- [ ] PBR
- [ ] Culling
- [ ] Threading
- [ ] Instancing
- [ ] C# Scripting
- [ ] Transparency
- [ ] Custom Gizmos
- [ ] Debug Renderer
- [ ] Shadow Mapping
- [ ] Virtual Texturing
- [x] Arena Allocator
- [ ] Post-Processing
- [ ] Indirect Drawing
- [x] Early Depth Pass
- [ ] Scene Serialization
- [ ] Advanced Anti-Aliasing Techniques

<br>

## 🤓️ About

**Frameworks used:**

* [GLM](https://github.com/g-truc/glm/) - Provides the engine with a powerful set of tools for solving linear algebra problems.
* [spdlog](https://github.com/gabime/spdlog) - Used for fast, asynchronous debug-only logging.
* [Vulkan](https://www.vulkan.org/) - Allows for incredibly fast rendering on a wide variety of platforms.
* [VMA](https://gpuopen.com/vulkan-memory-allocator/) - For optimal memory management of [Vulkan](https://www.vulkan.org/) objects.
* [Metal](https://developer.apple.com/metal/) - Being their native rendering API, it is used for fast, hardware-accelerated rendering on [Apple](https://www.apple.com) platforms.
* [metal-cpp](https://developer.apple.com/metal/cpp/) - Convenient C++ layer for [Metal](https://developer.apple.com/metal/), made by [Apple](https://www.apple.com). 
* [ios-cmake](https://github.com/leetal/ios-cmake) - Essential for building the engine on iOS.
* [whereami](https://github.com/gpakosz/whereami/) - Temporarily needed to retrieve binary folder, where shaders will be located until a project system is in place.
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

<p align="center" id="LineCounter">Total lines of code: 19,897</p>
<p align="center" id="LastUpdated">Last updated: 04/01/2024 </p>

-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------