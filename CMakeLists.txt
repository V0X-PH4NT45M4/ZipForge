cmake_minimum_required(VERSION 3.15)  # Adjust version as needed
project(MyProject LANGUAGES CXX)      # Set project name

# --- Configure Compiler Options ---
set(CMAKE_CXX_STANDARD 17)            # C++17 (or higher)
set(CMAKE_CXX_STANDARD_REQUIRED ON)    # Enforce standard
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # For clangd/LSP

# --- Automatically Find Sources/Headers ---
file(GLOB_RECURSE SOURCES "src/*.cpp")  # All .cpp files in src/
file(GLOB_RECURSE HEADERS "include/*.hpp")  # All .hpp files in include/

# --- Create Executable ---
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# --- Include Directories ---
target_include_directories(${PROJECT_NAME} PRIVATE include/)

# --- Link Libraries (if needed) ---
# target_link_libraries(${PROJECT_NAME} PRIVATE some_lib)
