import sys
import os
import re
from pathlib import Path

def GenerateFileFromFile(filePath: str, inputFilePath: str, symbols: dict[str, str]) -> None:
    # Generate file data
    fileData: str = GetGenerateFileDataFromFile(inputFilePath, symbols)
    os.makedirs(Path(filePath).parent, exist_ok=True)
    with open(filePath, 'w+') as file:
        file.write(fileData)

def GetGenerateFileDataFromFile(filePath: str, symbols: dict[str, str]) -> str:
    # Load file
    fileData: str = ''
    with open(filePath, 'r') as file:
        # Save initial file data
        fileData = file.read()
        for symbol in symbols:
            fileData = fileData.replace('${' + symbol + '}', symbols[symbol])
        file.close()

        # Log all missing symbols
        for symbol in re.findall(r'\$\{[^{}]*\}', fileData):
            print(f'Warning: Missing symbol [{symbol}]')

        # Remove all missing symbols
        fileData = re.sub(r'\$\{[^{}]*\}', '', fileData)

    return fileData

if __name__ == '__main__':
    if len(sys.argv) < 3 + 1:
        print('Usage: <relative_file_path> <relative_input_file_path> <symbols_dictionary>')
    else:
        GenerateFileFromFile(sys.argv[1], sys.argv[2], dict(pair.split('=') for pair in sys.argv[3].split(',')))