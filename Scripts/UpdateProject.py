#!/usr/bin/env python3

import os
import sys
import tempfile
import datetime
import subprocess
from sys import platform

CURRENT_DIRECTORY: str = os.path.dirname(os.path.realpath(__file__)).replace('\\', '/') + '/'
ENGINE_ROOT_DIRECTORY: str = CURRENT_DIRECTORY + '../'

def Main() -> None:
    UpdateReadMe()
    CompileShaders()

def UpdateReadMe() -> None:
    SOURCE_FILE_EXTENSIONS: [str] = ['.cpp', '.h', '.cs', '.py', '.cmake', '.glsl', '.vert', '.frag', 'comp', '.geom', '.tesc', '.tese', '.sh', '.bat']

    # Define initial line count
    linesOfCode: int = 0

    # Loop through all project files and check if the current file is a source file
    for subdirectory in ['Scripts/', 'Source/']:
        for root, dirs, files in os.walk(ENGINE_ROOT_DIRECTORY + subdirectory):
            for file in files:
                for extension in SOURCE_FILE_EXTENSIONS:
                    if file.endswith(extension):
                        with open(os.path.join(root, file), 'r') as fp:
                            for count, line in enumerate(fp):
                                pass
                            linesOfCode += count + 1
                            fp.close()

    with open(ENGINE_ROOT_DIRECTORY + 'README.md', 'r+', encoding='utf-8') as file:
        # Count README.md's lines as well (gotta look like there's a lot of code, innit :D) and write total count & date
        for count, line in enumerate(file):
            pass
        linesOfCode += count + 1
        file.seek(0)

        # Get old README.md file data
        readMeData: str = file.read()
        file.seek(0)

        index: int = readMeData.index('<p align="center" id="LineCounter">')
        readMeData = readMeData[0:index]

        # Get current date
        now = datetime.datetime.now()
        updated: str = f'{now.day:02}/{now.month:02}/{now.year:02}'

        # Apply date and line count changes
        linesOfCodeString: str = f'{linesOfCode:,}'
        linesOfCodeLine: str = f'<p align="center" id="LineCounter">Total lines of code: { linesOfCodeString }</p>\n'
        lastUpdatedLine: str = f'<p align="center" id="LastUpdated">Last updated: { updated } </p>\n'

        # Get new data and write to README.md file
        newReadMeData = readMeData + linesOfCodeLine + lastUpdatedLine + '\n' + ('-' * 171)
        file.write(newReadMeData)
        file.close()

def CompileShaders() -> None:
    # Get path to compiler
    SHADER_COMPILER_PATH: str = ENGINE_ROOT_DIRECTORY + 'Source/Core/Rendering/Shader Compilers/'
    if platform == "win32":
        SHADER_COMPILER_PATH += 'glslc-win.exe'
    elif platform == "darwin":
        SHADER_COMPILER_PATH += 'glslc-osx'
    elif platform == "linux" or platform == "linux2":
        SHADER_COMPILER_PATH += 'glslc-linux'
    else:
        print("Automatic SPIR-V shader compilation is not supported on your system! Returning...")
        return

    if len(sys.argv) != 2:
        print("Usage: <relative_project_path>")
        return

    # Get project path
    PROJECT_PATH: str = CURRENT_DIRECTORY + sys.argv[1]
    SHADER_EXTENSIONS: [str] = ['.glsl', '.vert', '.frag', '.comp', '.geom', '.tesc', '.tese']
    for root, dirs, files in os.walk(PROJECT_PATH + 'Shaders/'):
        for file in files:
            for extension in SHADER_EXTENSIONS:
                if file.endswith(extension):
                    PreprocessShader(os.path.join(root, file), SHADER_COMPILER_PATH, PROJECT_PATH)

def PreprocessShader(shaderFilePath: str, shaderCompilerFilePath: str, projectPath: str) -> None:
    # Load shader and handle includes
    shaderFile = open(shaderFilePath, 'r')
    shaderData = shaderFile.read()
    shaderFile.close()

    # Check if shader needs compilation
    if not '/* !COMPILE_TO_BINARY */' in shaderData:
        return

    shaderFileName: str = shaderFilePath[shaderFilePath.rfind('/') + 1:]
    print(f'Compiling shader [{shaderFileName}].')

    # Resolve includes
    includeIndex = shaderData.find('#include "')
    while includeIndex != -1:
        # Check if #include is commented out
        searchIndex: int = includeIndex
        includeCommentedOut: bool = False

        breakOutOfInnerLoop: bool = False
        while not breakOutOfInnerLoop:
            if shaderData[searchIndex] == '\n' or searchIndex == 0:
                breakOutOfInnerLoop = True
            elif (shaderData[searchIndex] == '/' and shaderData[searchIndex - 1] == '/') or (shaderData[searchIndex] == '*' and shaderData[searchIndex - 1] == '/'):
                includeCommentedOut = True
                breakOutOfInnerLoop = True
            else:
                searchIndex -= 1

        # Find end of include statement
        includeSize: int = 10
        while shaderData[includeIndex + includeSize] != '"':
            includeSize += 1

        # Get and paste include data
        includedShaderPath = shaderFilePath[:shaderFilePath.rindex('/') + 1] + shaderData[includeIndex + 10:includeIndex + includeSize]
        includedShaderData: str = ''

        # Check if included file exists
        if not os.path.exists(includedShaderPath):
            print(f'Could not include [{includedShaderPath}] in shader [{shaderFilePath}]! Verify the file exists!')
        else:
            if not includeCommentedOut:
                includedShaderData = open(includedShaderPath, 'r').read()

        # Paste included file
        shaderData = shaderData[:includeIndex] + includedShaderData + shaderData[includeIndex + includeSize + 1:]

        # Continue loop
        includeIndex = shaderData.find('#include "')

    # Write temporary shader
    temporaryShaderPath: str = tempfile.gettempdir().replace('\\', '/') + '/' + shaderFileName
    temporaryShaderFile = open(temporaryShaderPath, 'w+', encoding='utf8')
    temporaryShaderFile.write(shaderData)
    temporaryShaderFile.close()

    # Compile final shader
    command: str = f'"{shaderCompilerFilePath}" "{temporaryShaderPath}" -o "{projectPath}{shaderFilePath[shaderFilePath.find("Shaders"):]}.spv"'
    subprocess.call(command, shell=True)

    # Delete temporary shader
    os.remove(temporaryShaderPath)

Main()
