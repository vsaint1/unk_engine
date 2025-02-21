import os
import struct
import sys
import time

ROOT_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ASSETS_FOLDER = os.path.join(ROOT_DIR, "assets")
PAK_FILE = os.path.join(ASSETS_FOLDER, "assets.pak")


def package_assets(version="0.0.1", name="assets"):
    if not os.path.exists(ASSETS_FOLDER):
        print(f"[ERROR] - Assets folder '{ASSETS_FOLDER}' not found.")
        return

    index = {}

    print(f"[WARNING]: make sure to change the `source` field in the TILED exported to match tile_set path.")
    timer_start = time.time()

    with open(PAK_FILE, "wb") as pak:
        pak.write(struct.pack("I", 1))  
        pak.write(struct.pack("I", 0))  

        for root, _, files in os.walk(ASSETS_FOLDER):
            for file in files:
                # if file == "assets.pak":
                #     continue  

                filepath = os.path.join(root, file)
                relative_path = os.path.relpath(filepath, ASSETS_FOLDER)

                with open(filepath, "rb") as f:
                    file_size_mb = os.path.getsize(filepath) / 1000
                    print(f"[INFO] - Cooking {relative_path}, size {file_size_mb:.2f} mb")	
                    data = f.read()

                start = pak.tell()
                pak.write(data)
                end = pak.tell()
                index[relative_path] = (start, end - start)

        index_start = pak.tell()

        for path, (start, size) in index.items():
            path_encoded = path.encode("utf-8")
            pak.write(struct.pack("I", len(path_encoded)))
            pak.write(path_encoded)
            pak.write(struct.pack("II", start, size))

        index_size = pak.tell() - index_start
        pak.seek(4)
        pak.write(struct.pack("I", index_size))

    timer_end = time.time()
    timer_ms = (timer_end - timer_start) * 1000

    print(f"[INFO] - Cook completed, took {timer_ms:.2f} ms")


def extract_file(target_path, output_folder="extracted"):
    if not os.path.exists(PAK_FILE):
        print(f"[ERROR] - PAK file '{PAK_FILE}' not found.")
        return None

    with open(PAK_FILE, "rb") as pak:
        index_size = struct.unpack("I", pak.read(4))[0]
        pak.seek(-index_size, 2)

        while pak.tell() < pak.seek(0, 2):
            path_len = struct.unpack("I", pak.read(4))[0]
            path = pak.read(path_len).decode("utf-8")
            start, size = struct.unpack("II", pak.read(8))

            if path == target_path:
                pak.seek(start)
                data = pak.read(size)

                output_path = os.path.join(output_folder, target_path)
                os.makedirs(os.path.dirname(output_path), exist_ok=True)

                with open(output_path, "wb") as out_file:
                    out_file.write(data)

                print(f"[INFO] - Extracted '{target_path}' to '{output_path}'.")
                return data

        print(f"[WARNING] - File '{target_path}' not found in the package.")
        return None


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("[INFO] -Usage: python pak_tool.py [package|extract <file_path>]")
        sys.exit(1)

    command = sys.argv[1]

    if command == "package":
        package_assets()
    elif command == "extract" and len(sys.argv) == 3:
        extract_file(sys.argv[2])
    else:
        print("[ERROR] - Invalid command or missing parameters.")
