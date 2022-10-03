#!/usr/bin/env python3
import datetime
from fileinput import filename
import os

DIRECTORIES = ["../Core", "../Engine", "../Scripts"]
FILE_EXTENSIONS = [".cpp",  ".h", ".py", ".glsl", ".vert", ".frag", ".sh", ".bat"]

now = datetime.datetime.now()
lastUpdated = f"{now.day:02}/{now.month:02}/{now.year:02}"


def Main():
    linesOfCode = 0

    # For each directory to check
    for directory in DIRECTORIES:
        # Scan every file inside
        for root, dirs, files in os.walk(directory + "/"):
            for file in files:
                # Check if the extension is inside the target ones
                for extension in FILE_EXTENSIONS:
                    if file.endswith(extension):
                        fileName = str(os.path.join(root, file))
                        with open(fileName, 'r') as fp:
                            for count, line in enumerate(fp):
                                pass

                        # Update lines of code count and print a message to indicate the current file's count
                        linesOfCode += count + 1
                        print(f"{fileName} consists of: {count + 1} lines")


    # Load and count README.md's lines
    with open('../README.md', 'r') as file:
        for count, line in enumerate(file):
            pass
        linesOfCode += count + 1

    # Print README.md's line count
    print(f"\nTotal lines of code: { linesOfCode }.")

    # Update the README.md's description to have the current date and the just-calculated lines of code count
    newReadMeData = ""
    with open('../README.md', 'r') as file:
        readMeData = file.read()
        index = readMeData.index("<p align=\"center\" id=\"LinesCounter\">")
        readMeData = readMeData[0:index]

        linesOfCodeLine = f"<p align=\"center\" id=\"LinesCounter\">Total lines of code: {linesOfCode}</p>\n"
        lastUpdatedLine = f"<p align=\"center\" id=\"LastUpdated\">Last updated: {lastUpdated}</p>\n"

        newReadMeData = readMeData + linesOfCodeLine + lastUpdatedLine + "\n" + ("-" * 171)


    # Write to the README.md
    with open('../README.md', 'w') as file:
        file.write(newReadMeData)


Main()
