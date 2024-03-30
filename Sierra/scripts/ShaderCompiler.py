import os
import sys
import enum
import platform

SHADER_COMPILER_VERSION_MAJOR: int = 1
SHADER_COMPILER_VERSION_MINOR: int = 1
SHADER_COMPILER_VERSION_PATCH: int = 0

CURRENT_DIRECTORY: str = os.path.dirname(os.path.realpath(__file__)).replace('\\', '/') + '/'

class OperatingSystem(enum.Enum):
    Undefined = 0
    Windows   = 1
    macOS     = 2
    Linux     = 3

def CompileShader(inputShaderFilePath: str, outputShaderDirectoryPath: str) -> None:
    # Determine operating system
    operatingSystem: OperatingSystem = OperatingSystem.Undefined
    if platform.system() == 'Windows':
        operatingSystem = OperatingSystem.Windows
    elif platform.system() == 'Darwin':
        operatingSystem = OperatingSystem.macOS
    elif platform.system() == 'Linux':
        operatingSystem = OperatingSystem.Linux
    else:
        print('Error: Could not determine operating system!')
        return

    # Get path to ShaderConnect compiler
    compilerFilePath: str = CURRENT_DIRECTORY + 'Platform/'
    if operatingSystem == OperatingSystem.Windows:
        compilerFilePath += 'Windows/'
    elif operatingSystem == OperatingSystem.Linux:
        compilerFilePath += 'Linux/'
    elif operatingSystem == OperatingSystem.macOS:
        compilerFilePath += 'macOS/'
    compilerFilePath += f'ShaderConnect-{ SHADER_COMPILER_VERSION_MAJOR }.{ SHADER_COMPILER_VERSION_MINOR }.{ SHADER_COMPILER_VERSION_PATCH }'
    if operatingSystem == OperatingSystem.Windows:
        compilerFilePath += '.exe'

    # Determine shader type
    inputShaderType: str = ''
    inputShaderFileName, inputShaderFileExtension = os.path.splitext(os.path.basename(inputShaderFilePath))
    if inputShaderFileExtension == '.vert':
        inputShaderType = 'vertex'
    elif inputShaderFileExtension == '.frag':
        inputShaderType = 'fragment'
    elif inputShaderFileExtension == '.comp':
        inputShaderType = 'compute'

    # Determine target languages
    targetLanguages: list[str] = []
    if operatingSystem == OperatingSystem.Windows:
        targetLanguages = ['spir-v', 'hlsl', 'dxil']
    elif operatingSystem == OperatingSystem.macOS:
        targetLanguages = ['spir-v', 'macos-metallib', 'ios-metallib', 'ios-simulator-metallib', 'macos-metalsl', 'ios-metalsl']
    elif operatingSystem == OperatingSystem.Linux:
        targetLanguages = ['spir-v']

    # Run compile command
    command: str = f'{ compilerFilePath } { inputShaderFilePath } glsl { inputShaderType } { outputShaderDirectoryPath }{ inputShaderFileName }{ inputShaderFileExtension }.shader/ '
    for language in targetLanguages:
        os.system(command + language)

if len(sys.argv) < 2 + 1:
    print('Usage: <input_shader_file_path> <output_shader_directory_path>')
else:
    CompileShader(sys.argv[1], sys.argv[2])