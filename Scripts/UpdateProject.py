#!/usr/bin/env python3

import os
import shutil
import platform
import sys
from os.path import exists
import ReadMeCalculator

ROOT_DIRECTORY = "../"
OUTPUT_DIRECTORY = ""

DLL_DIRECTORY = ROOT_DIRECTORY + "Core/Dynamic Link Libraries/Windows/"
TEXTURE_DIRECTORY = ROOT_DIRECTORY + "Core/Rendering/Textures/"
MODEL_DIRECTORY = ROOT_DIRECTORY + "Core/Rendering/Models/"
FONT_DIRECTORY = ROOT_DIRECTORY + "Core/Rendering/Fonts/"

SHADERS_DIRECTORY = ROOT_DIRECTORY + "Core/Rendering/Shading/Shaders/"
TEMPORARY_SHADER_DIRECTORY = SHADERS_DIRECTORY + "Temp/"
SHADER_FILE_EXTENSIONS = [".vert", ".frag"]

def Main():
    # Check if required arguments are provided
    if len(sys.argv) <= 1:
        print("Error: You must run the scripts with either a --Debug or --Release argument!")
        return
    else:
        global OUTPUT_DIRECTORY

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

    # Create temporary shader directory
    CreateDirectory(TEMPORARY_SHADER_DIRECTORY)

    # Create the required directories in the output folder
    CreateDirectory(OUTPUT_DIRECTORY + "Shaders/")

    # Copy all required files
    CopyFolder(TEXTURE_DIRECTORY, OUTPUT_DIRECTORY + "Textures/")
    CopyFolder(MODEL_DIRECTORY, OUTPUT_DIRECTORY + "Models/")
    CopyFolder(FONT_DIRECTORY, OUTPUT_DIRECTORY + "Fonts/")

    # Compile shaders
    try:
        CompileShaders()
    except:
        pass

    # Remove temporary shader directory
    RemoveDirectory(TEMPORARY_SHADER_DIRECTORY)

    # Show success message
    print("Success!")

    # Calculate lines of code
    calculator = ReadMeCalculator.ReadMeCalculator(False)
        

def CompileShaders():

    # Find and compile every shader
    for file in os.listdir(SHADERS_DIRECTORY):
        fileExtensionIndex = file.find(".")
        if fileExtensionIndex == 0:
            continue
        
        fileExtension = file[fileExtensionIndex:]
        if (fileExtension in SHADER_FILE_EXTENSIONS):
            PreCompileShader(os.path.join(SHADERS_DIRECTORY, file))
            

def PreCompileShader(shaderFilePath):
    # Load shader data and check if #include is done
    shaderData = open(shaderFilePath, "r").read()
    shaderFileName = shaderFilePath[shaderFilePath.rindex("/") + 1:]
    includeIndex = shaderData.find("#include")

    # For each #include
    while includeIndex != -1 and not (includeIndex > 1 and shaderData[includeIndex - 1] == "/" and shaderData[includeIndex - 2] == "/"):            
        # Get the starting position of include
        startIndex = includeIndex
        while shaderData[startIndex] != "\"" and not (shaderData[startIndex] == "\\" and shaderData[startIndex + 1] == "n"):
            startIndex += 1

        startIndex += 1

        # Count end position of include
        endIndex = startIndex
        while shaderData[endIndex] != "\"" and not (shaderData[endIndex] == "\\" and shaderData[endIndex + 1] == "n"):
            endIndex += 1

        includedShader = shaderData[startIndex:endIndex]

        # Check if included file is of .glsl format and exists
        if includedShader.find(".glsl") == -1 or not exists(SHADERS_DIRECTORY + includedShader):
            print(f"Error: Cannot include [{ includedShader }] in shader [{ shaderFileName }]!")
            exit(-1)

        # Read included shader
        includedShaderData = open(SHADERS_DIRECTORY + includedShader, "r").read()
        includedShaderData = includedShaderData.replace("#version", "//")

        # Append read shader data to original shader
        shaderData = shaderData[:includeIndex] + shaderData[endIndex + 1:]
        shaderData = shaderData[:includeIndex] + includedShaderData + shaderData[includeIndex:]
        
        # Check if any more includes are done
        includeIndex = shaderData.find("#include")

    # Create temporary shader file with the newly "compiled" code
    temporaryShaderPath = TEMPORARY_SHADER_DIRECTORY + shaderFileName
    temporaryShaderFile = open(temporaryShaderPath, "x")
    temporaryShaderFile.write(shaderData)
    temporaryShaderFile.close()

    # Compile temporary shader
    operatingSystem = platform.system()
    if operatingSystem == "Windows":
        CompileWindowsShaders(temporaryShaderPath)
    else:
        CompileUnixShaders(temporaryShaderPath)


def CompileWindowsShaders(shaderFilePath):
    shaderFileName = shaderFilePath[shaderFilePath.rindex("/") + 1:]

    command = f"..\Core\Rendering\Shading\Compilers\glslc.exe { shaderFilePath } -o { OUTPUT_DIRECTORY }Shaders\{ shaderFileName }.spv"

    os.system(command)


def CompileUnixShaders(shaderFilePath):
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