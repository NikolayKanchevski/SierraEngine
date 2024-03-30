## 📔 Minimal Requirements

Minimal system requirements for every supported platform of the [Sierra API](https://github.com/NikichaTV/SierraEngine/tree/master/Sierra/) are listed here. Built on top of it, the whole [Sierra Engine]([Sierra API](https://github.com/NikichaTV/SierraEngine/) shares the exact same requirements. *Do note that the following information is a subject to change.*

<br>

## Base System Requirements

Complying with the following requirements guarantees that at least one graphics API backend is supported, hence the [Sierra API](https://github.com/NikichaTV/SierraEngine/tree/master/Sierra/) is supported as well. However, the usage of the only available API will be enforced.   

### Minimum Operating System Version

|    **[Windows](www.microsoft.com/en-us/windows/)**    | **[Linux](https://www.linux.org/)** |                        **[macOS](www.apple.com/macos/)**                        |             **[Android](https://www.android.com/)**              |          **[iOS](www.apple.com/ios/)**           |
|:-----------------------------------------------------:|:-----------------------------------:|:-------------------------------------------------------------------------------:|:----------------------------------------------------------------:|:------------------------------------------------:|
| [Windows 7](https://bg.wikipedia.org/wiki/Windows_7/) | Any [Linux](https://www.linux.org/) | [macOS 10.13.0 (High Sierra)](https://en.wikipedia.org/wiki/MacOS_High_Sierra/) | [Android 13.0](https://developer.android.com/about/versions/13/) | [iOS 13.0](https://en.wikipedia.org/wiki/IOS_13) |

### Minimal GPU

|                      [AMD](https://www.amd.com/en.html/)                      |                             [NVIDIA](https://www.nvidia.com/en-us/)                              |             [Intel](https://www.intel.com/content/www/us/en/homepage.html)             |                   **[Android](https://www.android.com/)**                   |                                                                     [Apple]([Apple](www.apple.com/))                                                                      |
|:-----------------------------------------------------------------------------:|:------------------------------------------------------------------------------------------------:|:--------------------------------------------------------------------------------------:|:---------------------------------------------------------------------------:|:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|
| Radeon HD 7970 ([GCN](https://en.wikipedia.org/wiki/Graphics_Core_Next/) 1.0) | GeForce GeForce GT 630 OEM ([Kepler](https://en.wikipedia.org/wiki/Kepler_(microarchitecture)/)) | HD Graphics 2000 [Skylake](https://en.wikipedia.org/wiki/Skylake_(microarchitecture)/) | Any [Android 13.0](https://developer.android.com/about/versions/13/) device | [Apple Silicon](https://bg.wikipedia.org/wiki/Apple_Silicon/) ([M1 Chip](https://en.wikipedia.org/wiki/Apple_M1/)) / [Apple A13](https://en.wikipedia.org/wiki/Apple_A13) |

<br>

## Graphics API Support

Below are listed the minimal requirements for every graphics API backend separately. Some systems might support multiple APIs, meaning that their respective user would be free to change their rendering backend at any time.

### Minimum Graphics API Version

|                                                      | **[Windows](www.microsoft.com/en-us/windows/)** | **[Linux](https://www.linux.org/)** | **[macOS](www.apple.com/macos/)** | **[Android](https://www.android.com/)** | **[iOS](www.apple.com/ios/)** |
|:----------------------------------------------------:|:-----------------------------------------------:|:-----------------------------------:|:---------------------------------:|:---------------------------------------:|:-----------------------------:|
|        **[Vulkan](https://www.vulkan.org/)**         |                       1.2                       |                 1.2                 |                 ❌                 |                   1.2                   |               ❌               |
| **[DirectX](https://developer.nvidia.com/directx/)** |                        ❌                        |                  ❌                  |                 ❌                 |                    ❌                    |               ❌               |
|   **[Metal](https://developer.apple.com/metal/)**    |                        ❌                        |                  ❌                  |                3.0                |                    ❌                    |              3.0              |
|    **[OpenGL](https://www.khronos.org/opengl/)**     |                        ❌                        |                  ❌                  |                 ❌                 |                    ❌                    |               ❌               |

### Minimal GPU (per Graphics API)

|                                                      |                      [AMD](https://www.amd.com/en.html/)                      |                             [NVIDIA](https://www.nvidia.com/en-us/)                              |             [Intel](https://www.intel.com/content/www/us/en/homepage.html)             |                   **[Android](https://www.android.com/)**                   |                                                                          [Apple](www.apple.com/)                                                                          |
|:----------------------------------------------------:|:-----------------------------------------------------------------------------:|:------------------------------------------------------------------------------------------------:|:--------------------------------------------------------------------------------------:|:---------------------------------------------------------------------------:|:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|
|        **[Vulkan](https://www.vulkan.org/)**         | Radeon HD 7970 ([GCN](https://en.wikipedia.org/wiki/Graphics_Core_Next/) 1.0) | GeForce GeForce GT 630 OEM ([Kepler](https://en.wikipedia.org/wiki/Kepler_(microarchitecture)/)) | HD Graphics 2000 [Skylake](https://en.wikipedia.org/wiki/Skylake_(microarchitecture)/) | Any [Android 13.0](https://developer.android.com/about/versions/13/) device |                                                                                     ❌                                                                                     |
| **[DirectX](https://developer.nvidia.com/directx/)** |                                       ❌                                       |                                                ❌                                                 |                                           ❌                                            |                                      ❌                                      |                                                                                     ❌                                                                                     |
|   **[Metal](https://developer.apple.com/metal/)**    |                                       ❌                                       |                                                ❌                                                 |                                           ❌                                            |                                      ❌                                      | [Apple Silicon](https://bg.wikipedia.org/wiki/Apple_Silicon/) ([M1 Chip](https://en.wikipedia.org/wiki/Apple_M1/)) / [Apple A13](https://en.wikipedia.org/wiki/Apple_A13) |
|    **[OpenGL](https://www.khronos.org/opengl/)**     |                                       ❌                                       |                                                ❌                                                 |                                           ❌                                            |                                      ❌                                      |                                                                                     ❌                                                                                     |