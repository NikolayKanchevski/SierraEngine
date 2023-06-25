#!/usr/bin/env python3

import os
import tempfile
import platform
import sys
from os.path import exists

ROOT_DIRECTORY: str = '../'
SHADER_TYPES: list[str] = [ '.vert', '.frag', '.geom', '.tese', '.tesc', '.comp' ]

def CompileWindowsShaders(shaderFilePath: str, outputDirectory: str, compilerPath: str):
    # Run Windows compiler
    shaderFileName: str = shaderFilePath[shaderFilePath.rfind('/') + 1:]
    command: str = f'{ compilerDirectory } { shaderFilePath } -o { outputDirectory }\\{ shaderFileName }.spv'
    os.system(command)


def CompileUnixShaders(shaderFilePath: str, outputDirectory: str, compilerDirectory: str):
    # Run Unix compiler
    shaderFileName: str = shaderFilePath[shaderFilePath.rfind('/') + 1:]
    command: str = f'{ compilerDirectory } { shaderFilePath } -o { outputDirectory }/{ shaderFileName }.spv'
    os.system(command)

def CompileShader(shaderPath: str, outputDirectory: str, compilerDirectory: str):
    # Load shader code
    shaderCode: str = open(shaderPath, 'r').read()

    # Get shader file info
    shaderDirectory: str = shaderPath[:shaderPath.rfind("/") + 1]
    shaderName: str = shaderPath[shaderPath.rfind("/") + 1:]
    shaderType: str = shaderPath[shaderPath.rfind('.'):]

    if not shaderType in SHADER_TYPES:
        print(f'Cannot compile shader with unknown type of [{ shaderType }]! Make sure it is one of the following: { str(SHADER_TYPES)[1:-1]  }!')
        exit(-1)

    # Load #include-s
    includeIndex: int = shaderCode.find('#include')
    includedShaderPaths: list[str] = []

    # For each #include
    commentStartIndex: int = -1
    while includeIndex != -1:
        # Check if line is commented
        searchIndex: int = includeIndex
        while True:
            if shaderCode[searchIndex] == '\n' or searchIndex == 0:
                break

            if shaderCode[searchIndex] == '/' and shaderCode[searchIndex - 1] == '/':
                commentStartIndex = searchIndex -1
                break

            searchIndex -= 1

        # Find start of included shader path
        startIncludeIndex: int = includeIndex
        while shaderCode[startIncludeIndex] != '"':
            startIncludeIndex += 1
        startIncludeIndex += 1

        # Find end of included shader path
        endIncludeIndex: int = startIncludeIndex + 1
        while shaderCode[endIncludeIndex] != '"':
            endIncludeIndex += 1

        # If line is commented out
        if commentStartIndex != -1:
            shaderCode = shaderCode[:commentStartIndex] + shaderCode[endIncludeIndex + 1:]
            includeIndex = shaderCode.find('#include')
            continue

        # Get included shader path and name
        includedShaderPath: str = shaderCode[startIncludeIndex:endIncludeIndex]
        includedShaderName: str = includedShaderPath[includedShaderPath.rfind("/") + 1:]

        # Check if included file is of .glsl format and exists
        absoluteIncludedShaderPath: str = shaderDirectory + includedShaderPath

        # Check if shader has already been included
        if not absoluteIncludedShaderPath in includedShaderPaths:
            includedShaderPaths.append(absoluteIncludedShaderPath)

            if includedShaderPath.find('.glsl') == -1 or not exists(absoluteIncludedShaderPath):
                print(f'Error: Could not include [{ includedShaderName }] in shader [{ shaderName }]!')
                exit(-1)

            # Format included shader code
            includedShaderCode: str = open(absoluteIncludedShaderPath, 'r').read()
            includedShaderCode.replace('#version', '//')

            # Append read shader data to original shader code
            shaderCode = shaderCode[:includeIndex] + includedShaderCode + shaderCode[endIncludeIndex + 1:]

            # Keep the recursion going
            includeIndex = shaderCode.find('#include')

        else:
            # Remove #include statement and keep the recursion going
            shaderCode = shaderCode[:includeIndex] + shaderCode[endIncludeIndex + 1:]
            includeIndex = shaderCode.find('#include')

    # Create temporary shader file with the new code
    temporaryShaderPath = tempfile.gettempdir() + '/' + shaderName
    open(temporaryShaderPath, 'w+').write(shaderCode)

    # Make directories to output folder
    if not os.path.exists(outputDirectory):
        os.makedirs(outputDirectory)

    # Compile temporary shader
    operatingSystem = platform.system()
    if operatingSystem == 'Windows':
        CompileWindowsShaders(temporaryShaderPath, outputDirectory, compilerDirectory + '/glslc.exe')
    else:
        CompileUnixShaders(temporaryShaderPath, outputDirectory, compilerDirectory + '/glslc')

    # Delete temporary shader
    os.remove(temporaryShaderPath)


def Main():
    # Check for legal usage
    if len(sys.argv) < 3:
        print('Usage: <path_to_shader_to_compile> <output_directory>.')
        return

    # Get passed arguments
    shaderPath: str = sys.argv[1]
    outputDirectory: str = sys.argv[2]

    # Get output folder
    if outputDirectory == '--Debug':
        outputDirectory = ROOT_DIRECTORY + ('cmake-build-debug/Debug/' if platform.system() == 'Windows' else 'cmake-build-debug/Shaders/')
    elif outputDirectory == '--Release':
        outputDirectory = ROOT_DIRECTORY + ('cmake-build-release/Release/' if platform.system() == 'Windows' else 'cmake-build-release/Shaders/')
    else:
        outputDirectory = ''

    # Compile shader
    CompileShader(shaderPath, outputDirectory, '../Core/Rendering/Shading/Compilers/')


if __name__ == "__main__":
    Main()