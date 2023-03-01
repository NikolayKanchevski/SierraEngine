<h1 align="center">Sierra Engine</h1>
<h6 align="center">By: <a>Nikolay Kanchevski</a></h6>
<br>

<p>
A little <strong>open-source</strong> rendering/game engine, written in C++, which features some of the most common game programming techniques built-in. 
It is based on the <b><a href="https://www.vulkan.org/">Vulkan</a></b> rendering API, which provides support for 
<b>Windows 7-11</b>, <b>macOS</b>, <b>Linux</b>, <b>Android</b>, <b>iOS</b>, <b>tvOS</b> and other operating systems,
though the engine has only been tested on <b>Windows 10, 11</b> and <b>macOS Monterey</b>. All of its features are listed below.
</p>

<br>

## 🛠️ Installing, Building, and Running the Engine<br>

Clone the repository in your IDE of choice (<i>Visual Studio</i>, <i>CLion</i>, etc.) Now, you will have to run a Python script what will configure the project for you. Obviously, this will require your machine to have <b><a href="https://www.python.org/downloads/">Python 3</a></b> installed, so make sure you have that. Navigate over to the folder where you cloned the repo and run the following two commands in your command prompt or terminal:
```bat
$ cd Scripts/
$ python3 UpdateProject.py --Release
```

You can then proceed and run the program in your IDE. Make sure you are in <b>RELEASE</b> mode, because Debug mode will not be configured by the script. However, if you are feeling like working on the project yourself, feel free to change the <i>--Release</i> argument with <i>--Debug</i>, which will configure it for Debug mode.

<br>

## [+] Features will one day be here...

<br>

##  🤓️  About

<h4>Information on the project:</h4>
<br>
<p>
    Frameworks used: 
    <ul>
        <li><a href="https://www.vulkan.org/">Vulkan</a> - For both cross-platform and pefromant-friendly rendering.</li>
        <li><a href="https://github.com/google/shaderc">Shaderc</a> - For the shader compilation and optimization at <runtime></runtime>.</li>
        <li><a href="https://gpuopen.com/vulkan-memory-allocator/">VMA</a> - For easier and more optimal memory allocation and distribution of <a href="https://www.vulkan.org/">Vulkan</a> objects.</li>
        <li><a href="https://github.com/glfw/glfw">GLFW</a> - For creating window interface and connecting it to the <b>Vulkan</b> renderer.</li>
        <li><a href="https://github.com/g-truc/glm">GLM</a> - For all advanced mathematical operations and objects within the engine.</li>
        <li><a href="https://github.com/ocornut/imgui">ImGui</a> - For the user interface implemented.</li>
        <li><a href="https://github.com/CedricGuillemet/ImGuizmo">ImGuizmo</a> - For gizmos in the scene view.</li>
        <li><a href="https://github.com/skypjack/entt">EnTT</a> - For the easy-to-use and incredibly perfomant <a href="https://en.wikipedia.org/wiki/Entity_component_system">entity component system</a>.</li>
        <li><a href="https://github.com/assimp/assimp">Assimp</a> - For the loading of all kinds of 3D model formats (.obj, .fbx, .dae, etc.).</li>
        <li><a href="https://github.com/nothings/stb">Stb</a> - For loading image data from all kinds of image formats (.jpg, .png, etc.).</li>
        <li><a href="https://fmt.dev/latest/index.html">FMT</a> - For fast and easy string formatting.</li>
        <!--<li><a href="https://github.com/TheNitesWhoSay/RareCpp">RareCpp</a> - For reflections and serialization.</li>-->
        <li><a href="https://github.com/ThePhD/infoware">Infoware</a> - For the extraction of various system-and-hardware-specific data.</li>
        <li><a href="https://github.com/gpakosz/whereami">Whereami</a> - For the retrieval of the compiled executable's location within the file system.</li>
        <li><a href="https://www.intel.com/content/www/us/en/developer/tools/oneapi/onetbb.html#gs.m2vcgt">TBB</a> - For easier and guaranteed-to-be-supported threading.</li>
        <li><a href="https://ih1.redbubble.net/image.528192883.5730/st,small,845x845-pad,1000x1000,f8f8f8.u9.jpg">My Brain</a> - There is not much left of it, actually...</li>
    </ul>
    <br>
    Software used: 
    <ul>
        <li><a href="https://www.jetbrains.com/clion/">JetBrains CLion</a> - A <b>cross-platform</b> IDE used to develop the final C++ version.</li>
        <li><a href="https://www.jetbrains.com/rider/">JetBrains Rider</a> - Another <b>cross-platform</b> IDE but for C# which was used to create the .NET 6.0 prototype version of the engine.</li>
        <li><a href="https://www.blender.org/">Blender</a> - For the testing of 3D models and textures functionality.</li>
        <li><a href="https://trello.com/b/RMYtZPOg/sierra-engine">Trello</a> - For pretending to have an organized list of things to implement next.</li>
    </ul>
</p>

---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

<p align="center" id="LinesCounter">Total lines of code: 17,362</p>
<p align="center" id="LastUpdated">Last updated: 01/03/2023</p>

---------------------------------------------------------------------------------------------------------------------------------------------------------------------------