#!/usr/bin/env python3

import os
import shutil
import platform
import sys

ROOT_DIRECTORY = "../"
OUTPUT_DIRECTORY = ""

DLL_DIRECTORY = ROOT_DIRECTORY + "Core/Dynamic Link Libraries/Windows/"
TEXTURE_DIRECTORY = ROOT_DIRECTORY + "Core/Rendering/Textures/"
MODEL_DIRECTORY = ROOT_DIRECTORY + "Core/Rendering/Models/"
FONT_DIRECTORY = ROOT_DIRECTORY + "Core/Rendering/Fonts/"

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
            OUTPUT_DIRECTORY = ROOT_DIRECTORY + "cmake-build-debug/"
        else:
            OUTPUT_DIRECTORY = ROOT_DIRECTORY + "cmake-build-release/"

        # Check if too many arguments are provided
        if (len(sys.argv) >= 3):
            print("Error: Too many arguments specified!")
            return

    # Create the required directories in the output folder
    CreateDirectory(OUTPUT_DIRECTORY + "Shaders/")

    # Copy all required files
    CopyFolder(TEXTURE_DIRECTORY, OUTPUT_DIRECTORY + "Textures/")
    CopyFolder(MODEL_DIRECTORY, OUTPUT_DIRECTORY + "Models/")
    CopyFolder(FONT_DIRECTORY, OUTPUT_DIRECTORY + "Fonts/")

    # Compile shaders
    CompileShaders(OUTPUT_DIRECTORY)

    # Show success message
    print("Success!")
        

def CompileShaders(OUTPUT_DIRECTORY):
    operatingSystem = platform.system()
    if operatingSystem == "Windows":
        CompileWindowsShaders(OUTPUT_DIRECTORY)
    else:
        CompileUnixShaders(OUTPUT_DIRECTORY)


def CompileWindowsShaders(OUTPUT_DIRECTORY):
    command = f"..\Core\Rendering\Shading\Compilers\glslc.exe { ROOT_DIRECTORY }Core\Rendering\Shading\Shaders\shader.vert -o { OUTPUT_DIRECTORY }Shaders\shader.vert.spv"

    os.system(command)

    command = f"..\Core\Rendering\Shading\Compilers\glslc.exe { ROOT_DIRECTORY }Core\Rendering\Shading\Shaders\shader.frag -o { OUTPUT_DIRECTORY }Shaders\shader.frag.spv"

    os.system(command)


def CompileUnixShaders(OUTPUT_DIRECTORY):
    command = f"{ ROOT_DIRECTORY }Core/Rendering/Shading/Compilers/glslc { ROOT_DIRECTORY }Core/Rendering/Shading/Shaders/shader.vert -o { OUTPUT_DIRECTORY }Shaders/shader.vert.spv\n"
    command+= f"{ ROOT_DIRECTORY }Core/Rendering/Shading/Compilers/glslc { ROOT_DIRECTORY }Core/Rendering/Shading/Shaders/shader.frag -o { OUTPUT_DIRECTORY }Shaders/shader.frag.spv"

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