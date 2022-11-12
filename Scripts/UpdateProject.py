#!/usr/bin/env python3

import os
import shutil
import platform
import sys
import time

ROOT_DIRECTORY = "../"
OUTPUT_DIRECTORY = ""

DLL_DIRECTORY = ROOT_DIRECTORY + "Core/Dynamic Link Libraries/Windows/"
TEXTURE_DIRECTORY = ROOT_DIRECTORY + "Core/Rendering/Textures/"
MODEL_DIRECTORY = ROOT_DIRECTORY + "Core/Rendering/Models/"
FONT_DIRECTORY = ROOT_DIRECTORY + "Core/Rendering/Fonts/"

SHADERS_DIRECTORY = ROOT_DIRECTORY + "Core/Rendering/Shading/Shaders/"
SHADER_FILE_EXTENSIONS = [".glsl", ".vert", ".frag"]

def Main():
    # Check if required arguments are provided
    if len(sys.argv) <= 1:
        print("Error: You must run the scripts with either a --Debug or --Release argument!")
        return
    else:
        # Check if the required arguments are valid and set the output folder accordingly
        if sys.argv[1] != "--Debug" and sys.argv[1] != "--Release":
            print(f"Error: Unrecognized argument: { sys.argv[1] }!")
            return
        elif sys.argv[1] == "--Debug":
            OUTPUT_DIRECTORY = ROOT_DIRECTORY + ("cmake-build-debug/Debug/" if platform.system() == "Windows" else "cmake-build-debug/")
        else:
            OUTPUT_DIRECTORY = ROOT_DIRECTORY + ("cmake-build-release/Release/" if platform.system() == "Windows" else "cmake-build-debug/")

        # Check if too many arguments are provided
        if (len(sys.argv) >= 3):
            print("Error: Too many arguments specified!")
            return

    try:
        RemoveDirectory(OUTPUT_DIRECTORY + "Shaders/")
    except:
        pass

    # Create the required directories in the output folder
    CreateDirectory(OUTPUT_DIRECTORY + "Shaders/")

    # Copy all required files
    CopyFolder(TEXTURE_DIRECTORY, OUTPUT_DIRECTORY + "Textures/")
    CopyFolder(MODEL_DIRECTORY, OUTPUT_DIRECTORY + "Models/")
    CopyFolder(FONT_DIRECTORY, OUTPUT_DIRECTORY + "Fonts/")
    CopyFolder(SHADERS_DIRECTORY, OUTPUT_DIRECTORY + "Shaders/")

    # Compile shaders
    CompileShaders(OUTPUT_DIRECTORY)

    # Show success message
    print("Success!")
        

def CompileShaders(OUTPUT_DIRECTORY):
    operatingSystem = platform.system()

    # Find and compile every shader
    for file in os.listdir(SHADERS_DIRECTORY):
        fileExtension = file[file.index("."):]
        if (fileExtension in SHADER_FILE_EXTENSIONS):
            if operatingSystem == "Windows":
                CompileWindowsShaders(os.path.join(SHADERS_DIRECTORY, file), OUTPUT_DIRECTORY)
            else:
                CompileUnixShaders(os.path.join(SHADERS_DIRECTORY, file), OUTPUT_DIRECTORY)


def CompileWindowsShaders(shaderFilePath, OUTPUT_DIRECTORY):
    shaderFileName = shaderFilePath[shaderFilePath.rindex("/") + 1:]

    command = f"..\Core\Rendering\Shading\Compilers\glslc.exe { shaderFilePath } -o { OUTPUT_DIRECTORY }Shaders\{ shaderFileName }.spv"

    os.system(command)


def CompileUnixShaders(shaderFilePath, OUTPUT_DIRECTORY):
    shaderFileName = shaderFilePath[shaderFilePath.rindex("/") + 1:]

    command = f"../Core/Rendering/Shading/Compilers/glslc { shaderFilePath } -o { OUTPUT_DIRECTORY }Shaders/{ shaderFileName }.spv"

    os.system(command)


def CopyFile(file, destination):
    shutil.copy(file, destination)


def CopyFolder(folder, destination):
    shutil.copytree(folder, destination, dirs_exist_ok=True)


def CreateDirectory(DIRECTORY_TO_CREATE):
    os.makedirs(DIRECTORY_TO_CREATE, exist_ok=True)


def RemoveDirectory(DIRECTORY_TO_REMOVE):
    shutil.rmtree(DIRECTORY_TO_REMOVE)


def DirectoryExists(DIRECTORY_TO_CHECK):
    return os.path.isdir(DIRECTORY_TO_CHECK) == False

Main()