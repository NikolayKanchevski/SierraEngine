# Sierra Engine

<p align="center">
    <img src="README-Media/SierraLogoTextBackground.png" alt="SierraEngineTextLogo">
</p>

<br>

# What is Sierra Engine?

An **open-source** game/rendering engine, which aims to provide users with an all-in-one development kit, suited for any graphics-intesive tasks (such as video games, 3D editors, etc.). It is based solely on the **[Vulkan](https://www.vulkan.org/)** rendering API, which provides suppot for **Windows 7-11**, **macOS**, **Linux**, **Android**, **iOS**, and **[more](https://en.wikipedia.org/wiki/Vulkan#Cross_platform)**, but is planned to also feature native APIs (like **[DirectX](https://developer.nvidia.com/directx)** and **[Metal](https://developer.apple.com/metal/)**) to further improve its already-incredible performance

<br>

## 🛠️ Building and Running the Engine

**Requirements:**

In order to build it successfully, you must first make sure you have the following installed (some of these modules may come with your IDE of choice):

<br>

* A C/C++ compiler (preferably **[CLang](https://clang.llvm.org)** or **[MSVC](https://visualstudio.microsoft.com/downloads/)**)
* **[CMake](https://cmake.org/download/)** 3.20 or higher
* **[Ninja](https://ninja-build.org)** build tool
* The **[VulkanSDK](https://vulkan.lunarg.com)**
* **[Python](https://www.python.org/downloads/)**

<br>

**Installation:**
Firstly, clone the repository in your favourite IDE <b><a href="https://visualstudio.microsoft.com">Visual Studio</a></b>, <b><a href="https://www.jetbrains.com/clion/">CLion</a></b>, etc.). Next, you will have to run a Python script what will configure the project for you. Navigate over to the folder where you cloned the repo, run the command below, hit build, and pray to God that the project compiles. 🤞

<br>

```bat
$ python3 Scripts/UpdateProject.py --Release
```
*Note: You can replace the **--Release** argument with **--Debug**, depending on which configuration you want to be configured.*

<br>

## 💫 Features

```c++
// TODO: Add to this section :D
```

## 📄 Documentation

Wondering how to use the engine? You can check out the online documentation [here](https://nikicha.gitbook.io/sierra-engine-dotnet/) (it was written for the .NET version, but a new website for the C++ version is being worked on), or you can also toy around with the example projects given in the repository, as most of the code is thorougly explained with comments. Regardless - have fun reading, mate! 🤓

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
- [ ] Virtual Texture
- [ ] Arena Allocator
- [ ] Post-Processing
- [ ] Indirect Drawing
- [ ] Early Depth Pass
- [ ] Scene Serialization
- [ ] Advanced Anti-Aliasing Techniques

<br>

## 🤓️ About

**Frameworks used:**

* [Vulkan](https://www.vulkan.org/) - For both cross-platform and pefromant-friendly rendering.
* [Volk](https://github.com/zeux/volk/tree/master/) - A meta-loader for [Vulkan](https://www.vulkan.org/) that reduces CPU overhead and simplifies the loading of extensions.
* [VMA](https://gpuopen.com/vulkan-memory-allocator/) - For easier and more optimal memory allocation and distribution of [Vulkan](https://www.vulkan.org/) objects.
* [Shaderc](https://github.com/google/shaderc/) - For the shader compilation and optimization at runtime.
* [GLFW](https://github.com/glfw/glfw/) - For creating window interface and connecting it to the **Vulkan** renderer.
* [GLM](https://github.com/g-truc/glm/) - For all advanced mathematical operations and objects within the engine.
* [ImGui](https://github.com/ocornut/imgui/) - For the user interface implemented.
* [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo/) - For gizmos in the scene view.
* [EnTT](https://github.com/skypjack/entt/) - For the easy-to-use and incredibly perfomant [entity component system](https://en.wikipedia.org/wiki/Entity_component_system).
* [Assimp](https://github.com/assimp/assimp/) - For the loading of all kinds of 3D model formats (.obj, .fbx, .dae, etc.).
* [Stb](https://github.com/nothings/stb) - For loading image data from all kinds of image formats (.jpg, .png, etc.).
* [FMT](https://fmt.dev/latest/index.html) - For fast and easy string formatting.
* [RareCpp](https://github.com/TheNitesWhoSay/RareCpp/) - For reflections and serialization.
* [Infoware](https://github.com/ThePhD/infoware/) - For the extraction of various system-and-hardware-specific data.
* [Whereami](https://github.com/gpakosz/whereami/) - For the retrieval of the compiled executable's location within the file system.
* [TBB](https://www.intel.com/content/www/us/en/developer/tools/oneapi/onetbb.html#gs.m2vcgt/) - For easier and guaranteed-to-be-supported threading.
* [My Brain](https://ih1.redbubble.net/image.528192883.5730/st,small,845x845-pad,1000x1000,f8f8f8.u9.jpg) - There is not much left of it, actually...

<br>

**Software used:**

* [JetBrains CLion](https://www.jetbrains.com/clion/) - A **cross-platform** IDE used to develop the final C++ version.
* [JetBrains Rider](https://www.jetbrains.com/rider/) - Another **cross-platform** IDE but for C# which was used to create the .NET 6.0 prototype version of the engine.
* [Blender](https://www.blender.org/) - For the testing of 3D models and textures functionality.
* [Trello](https://trello.com/b/RMYtZPOg/sierra-engine/) - For pretending to have an organized list of things to implement next.

<br>

---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

<p align="center" id="LinesCounter">Total lines of code: 18,033</p>
<p align="center" id="LastUpdated">Last updated: 15/05/2023</p>

---------------------------------------------------------------------------------------------------------------------------------------------------------------------------