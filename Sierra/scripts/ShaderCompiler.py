import os
import sys
import enum
import ctypes
import shutil
import platform

SHADER_COMPILER_VERSION_MAJOR: int = 1
SHADER_COMPILER_VERSION_MINOR: int = 2
SHADER_COMPILER_VERSION_PATCH: int = 0

CURRENT_DIRECTORY_PATH: str = os.path.dirname(os.path.realpath(__file__)).replace('\\', '/') + '/'

class ShaderFileHeader(ctypes.Structure):
    _fields_ = [
        ('spvMemorySize',                   ctypes.c_ulonglong),
        ('macosMetalLibMemorySize',         ctypes.c_ulonglong),
        ('iosMetalLibMemorySize',           ctypes.c_ulonglong),
        ('iosSimulatorMetalLibMemorySize',  ctypes.c_ulonglong),
        ('dxilMemorySize',                  ctypes.c_ulonglong)
    ]

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
    compilerFilePath: str = CURRENT_DIRECTORY_PATH + 'Platform/'
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
        targetLanguages = ['spir-v', 'dxil']
    elif operatingSystem == OperatingSystem.macOS:
        targetLanguages = ['spir-v', 'macos-metallib', 'ios-metallib', 'ios-simulator-metallib']
    elif operatingSystem == OperatingSystem.Linux:
        targetLanguages = ['spir-v']

    # Run compile command
    temporaryShaderDirectoryPath: str = f'{ outputShaderDirectoryPath }{ inputShaderFileName }{ inputShaderFileExtension }.temporary/'
    command: str = f'{ compilerFilePath } { inputShaderFilePath } glsl { inputShaderType } { temporaryShaderDirectoryPath }'
    for language in targetLanguages:
        os.system(f'{ command } { language }')

    # Fill out shader header
    header: ShaderFileHeader = ShaderFileHeader()
    blob: bytearray = bytearray()
    if 'spir-v' in targetLanguages:
        shaderFilePath: str = temporaryShaderDirectoryPath + 'shader.spv'
        header.spvMemorySize = os.path.getsize(shaderFilePath)

        file = open(shaderFilePath, 'rb')
        blob += file.read()
        file.close()
    if 'macos-metallib' in targetLanguages:
        shaderFilePath: str = temporaryShaderDirectoryPath + 'shader.macos.metallib'
        header.macosMetalLibMemorySize = os.path.getsize(shaderFilePath)

        file = open(shaderFilePath, 'rb')
        blob += file.read()
        file.close()
    if 'ios-metallib' in targetLanguages:
        shaderFilePath: str = temporaryShaderDirectoryPath + 'shader.ios.metallib'
        header.iosMetalLibMemorySize = os.path.getsize(shaderFilePath)

        file = open(shaderFilePath, 'rb')
        blob += file.read()
        file.close()
    if 'ios-simulator-metallib' in targetLanguages:
        shaderFilePath: str = temporaryShaderDirectoryPath + 'shader.ios-simulator.metallib'
        header.iosSimulatorMetalLibMemorySize = os.path.getsize(shaderFilePath)

        file = open(shaderFilePath, 'rb')
        blob += file.read()
        file.close()
    if 'dxil' in targetLanguages:
        shaderFilePath: str = temporaryShaderDirectoryPath + 'shader.dxil'
        header.dxilMemorySize = os.path.getsize(shaderFilePath)

        file = open(shaderFilePath, 'rb')
        blob += file.read()
        file.close()

    # Pack shaders in one file
    shaderFile = open(outputShaderDirectoryPath + f'{ inputShaderFileName }{ inputShaderFileExtension }.shader', 'wb+')
    shaderFile.write(bytearray(header))
    shaderFile.write(blob)

    # Remove temporary directory
    shutil.rmtree(temporaryShaderDirectoryPath)

if __name__ == '__main__':
    if len(sys.argv) < 2 + 1:
        print('Usage: <input_shader_file_path> <output_shader_directory_path>')
    else:
        CompileShader(sys.argv[1], sys.argv[2])