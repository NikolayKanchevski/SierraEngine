import sys
import os

def FileToByteArray(filePath: str) -> None:
    if not os.path.exists(filePath):
        print(f'Cannot convert file [{ filePath }] to byte array, as it does not exist!')
        return

    # Add every byte to string
    file = open(filePath, 'rb')
    output = f'constexpr std::array<uint8, { os.path.getsize(filePath) }> DATA {{ {", ".join(f"{ hex(byte) }" for byte in file.read()) } }};'
    file.close()

    print(output)

if __name__ == '__main__':
    if len(sys.argv) < 1 + 1:
        print('Usage: <input_file_path>')
    else:
        FileToByteArray(sys.argv[1])