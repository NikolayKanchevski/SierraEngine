#!/usr/bin/env python3

import os
import re
import sys
import tempfile
import datetime
import subprocess
from sys import platform

CURRENT_DIRECTORY_PATH: str = os.path.dirname(os.path.realpath(__file__)).replace('\\', '/') + '/'
ENGINE_ROOT_DIRECTORY_PATH: str = CURRENT_DIRECTORY_PATH + '../../'

def UpdateReadMe() -> None:
    SOURCE_FILE_EXTENSIONS: [str] = ['.cpp', '.h', '.mm', '.cs', '.py', '.glsl', '.hlsl', '.msl' '.cmake', '.txt']

    # Define initial line count
    linesOfCode: int = 0

    # Loop through all project files and check if the current file is a source file
    for subdirectory in ['Sierra/', 'SierraEngine/']:
        for root, dirs, files in os.walk(ENGINE_ROOT_DIRECTORY_PATH + subdirectory):
            for file in files:
                for extension in SOURCE_FILE_EXTENSIONS:
                    if 'bin' in root or 'vendor' in root or 'config' in root or 'Default' in root or any(re.compile(r'\.[a-zA-Z]').match(item) for item in root.split('/')):
                        continue

                    if str(file).endswith(extension):
                        with open(os.path.join(root, file), 'r') as file:
                            for count, line in enumerate(file):
                                pass
                            linesOfCode += count + 1

                            # print(str(file) + ": " + str(count + 1))
                            file.close()

    with open(ENGINE_ROOT_DIRECTORY_PATH + 'README.md', 'r+') as file:
        # Count README.md's lines as well (gotta look like there's a lot of code, innit :D) and write total count & date
        for count, line in enumerate(file):
            pass
        linesOfCode += count + 1
        file.seek(0)

        # Get old README.md file data
        readMeData: str = file.read()

        index: int = readMeData.index('<p id="LineCounter" align="center">')
        readMeData = readMeData[0:index]

        # Get current date
        now = datetime.datetime.now()
        updated: str = f'{now.day:02}/{now.month:02}/{now.year:02}'

        # Apply date and line count changes
        linesOfCodeString: str = f'{linesOfCode:,}'
        linesOfCodeLine: str = f'<p id="LineCounter" align="center">Total lines of code: { linesOfCodeString }</p>\n'
        lastUpdatedLine: str = f'<p id="LastUpdated" align="center">Last updated: { updated } </p>\n'

        # Get new data and write to README.md file
        newReadMeData = readMeData + linesOfCodeLine + lastUpdatedLine + '\n' + ('-' * 171)
        file.seek(0)
        file.write(newReadMeData)
        file.close()

if __name__ == '__main__':
    UpdateReadMe()