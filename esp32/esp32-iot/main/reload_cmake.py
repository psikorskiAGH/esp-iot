import os
from pathlib import Path

CMAKE_TEMPALTE = """\
idf_component_register(
    SRCS {}
    INCLUDE_DIRS ""
    REQUIRES esp_http_server
    REQUIRES esp_wifi
    REQUIRES nvs_flash
    REQUIRES esp_adc
    REQUIRES driver
)
"""

def main():
    app_path = Path.cwd().joinpath("main").resolve()
    assert app_path.is_dir(), f"'app_path' not a folder ('{app_path}')"

    cmakelists_path = app_path.joinpath("CMakeLists.txt")
    assert cmakelists_path.is_file(), f"'cmakelists_path' not a file ('{cmakelists_path}')"
    
    sources = app_path.rglob("*.cpp")
    listed_sources = " ".join(f'"{s.relative_to(app_path)}"' for s in sources).replace("\\", "/")
    cmake_data = CMAKE_TEMPALTE.format(listed_sources)
    with open(cmakelists_path, 'w') as f:
        f.write(cmake_data)
    print("Done")

if __name__ == "__main__":
    main()