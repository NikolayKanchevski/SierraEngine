#!/usr/bin/env python3

import os
import shutil
import sys
import datetime
import platform
from CompileShader import CompileShader

CMAKE_CALL: bool = len(sys.argv) > 2 and sys.argv[2] == '--CMakeCall'
ROOT_DIRECTORY: str = os.getcwd() + '/Scripts/' if CMAKE_CALL else os.getcwd() + '/'
SHADERS_DIRECTORY: str = ROOT_DIRECTORY + '../Core/Rendering/Shading/Shaders/'

COUNT_SUBDIRECTORIES: list[str] = ['../Core/', '../Engine/', '../Scripts/']
COUNT_FILE_EXTENSIONS: list[str] = ['.cpp',  '.h', '.py', '.cs', '.glsl', '.vert', '.frag', '.glsl' '.sh', '.bat', '.txt']
DEBUG_COUNT: bool = False

def Main():
    if len(sys.argv) < 2:
        print('Usage: <configuration> (--Debug or --Release), --CMakeCall (optional, only to be passed if script is called from CMake).')
        exit(-1)

    # Compile shaders to SPIR-V
    CompileShaders()

    # Update README.md line count and date
    UpdateReadMe()

    # Show success message
    if not CMAKE_CALL:
        print('Success!')

def CompileShaders():
    # Get output folder
    outputDirectory: str = sys.argv[1]
    if outputDirectory == '--Debug':
        outputDirectory = ROOT_DIRECTORY + '../' + ('cmake-build-debug/Debug/' if platform.system() == 'Windows' else 'cmake-build-debug/Shaders')
    elif outputDirectory == '--Release':
        outputDirectory = ROOT_DIRECTORY + '../' + ('cmake-build-release/Release/' if platform.system() == 'Windows' else 'cmake-build-release/Shaders')
    else:
        outputDirectory = ''

    # For each shader
    for root, dirs, files in os.walk(SHADERS_DIRECTORY):
        for file in files:
            filePath: str = str(os.path.join(root, file))
            shaderOutputPath: str = outputDirectory + filePath[filePath.index('Shaders/') + 7:filePath.rindex('/')] + '/'
            try:
                if '/* !COMPILE_TO_BINARY */' in open(filePath, 'r').read():
                    # Compile binary shader and delete original one
                    CompileShader(filePath, shaderOutputPath, ROOT_DIRECTORY + '../Core/Rendering/Shading/Compilers')
                    os.remove(shaderOutputPath + file)
                    pass
                else:
                    os.makedirs(os.path.dirname(shaderOutputPath), exist_ok=True)
                    shutil.copy(filePath, shaderOutputPath)
            except:
                os.makedirs(os.path.dirname(shaderOutputPath), exist_ok=True)
                shutil.copy(filePath, shaderOutputPath)

def UpdateReadMe():
    # Define initial line count
    linesOfCode: int = 0

    # For each directory to check
    for subdirectory in COUNT_SUBDIRECTORIES:
        # Scan every file inside
        for root, dirs, files in os.walk(ROOT_DIRECTORY + subdirectory):
            for file in files:
                # Check if the extension is inside the target ones
                for extension in COUNT_FILE_EXTENSIONS:
                    if file.endswith(extension):
                        filePath: str = str(os.path.join(root, file))
                        with open(filePath, 'r') as fp:
                            for count, line in enumerate(fp):
                                pass

                        # Update lines of code count and print a message to indicate the current file's count
                        linesOfCode += count + 1
                        if DEBUG_COUNT:
                            print(f'{ filePath } consists of: { count + 1 } lines')

    # Load and count README.md's lines
    with open(ROOT_DIRECTORY + '../README.md', 'r') as file:
        for count, line in enumerate(file):
            pass
        linesOfCode += count + 1

    # Print README.md's line count
    if DEBUG_COUNT:
        print(f'\nTotal lines of code: { linesOfCode }.')

    # Format lines of code as a separated number string
    linesOfCodeString: str = f'{ linesOfCode:,}'

    # Update the README.md's description to have the current date and the just-calculated lines of code count
    newReadMeData: str = ''
    with open(ROOT_DIRECTORY + '../README.md', 'r') as file:
        # Get old README.md file data
        readMeData = file.read()
        index = readMeData.index('<p align="center" id="LinesCounter">')
        readMeData = readMeData[0:index]

        # Get current date
        now = datetime.datetime.now()
        updated: str = f'{now.day:02}/{now.month:02}/{now.year:02}'

        # Apply date and line count changes
        linesOfCodeLine: str = f'<p align="center" id="LinesCounter">Total lines of code: { linesOfCodeString }</p>\n'
        lastUpdatedLine: str = f'<p align="center" id="LastUpdated">Last updated: { updated } </p>\n'

        newReadMeData = readMeData + linesOfCodeLine + lastUpdatedLine + '\n' + ('-' * 171)

    # Write to the README.md
    with open(ROOT_DIRECTORY + '../README.md', 'w') as file:
        file.write(newReadMeData)


Main()