cmake_minimum_required(VERSION 3.20)
project(Lab1Graphics LANGUAGES CXX)

# ---------- SETTINGS ----------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SLANG_VERSION "2025.19.1")
if(WIN32)
    set(SLANG_OS "windows-x86_64")
elseif(APPLE)
    set(SLANG_OS "macos-aarch64")
elseif(UNIX)
    set(SLANG_OS "linux-x86_64")
endif()

include(FetchContent) 
FetchContent_Populate(slang_zip URL https://github.com/shader-slang/slang/releases/download/v${SLANG_VERSION}/slang-${SLANG_VERSION}-${SLANG_OS}.zip QUIET)
message(STATUS "Slang dir: ${slang_zip_SOURCE_DIR}")
add_library(slang SHARED IMPORTED GLOBAL)
if(WIN32)
    set_target_properties(slang PROPERTIES
        IMPORTED_LOCATION ${slang_zip_SOURCE_DIR}/Dependencies/Libraries/slang.dll
        IMPORTED_IMPLIB ${slang_zip_SOURCE_DIR}/Dependencies/Libraries/slang.lib
        INTERFACE_INCLUDE_DIRECTORIES ${slang_zip_SOURCE_DIR}/include
    )
elseif(APPLE)
    set_target_properties(slang PROPERTIES
        IMPORTED_LOCATION ${slang_zip_SOURCE_DIR}/Dependencies/Libraries/libslang.dylib
        INTERFACE_INCLUDE_DIRECTORIES ${slang_zip_SOURCE_DIR}/include
    )
elseif(UNIX)
    set_target_properties(slang PROPERTIES
        IMPORTED_LOCATION ${slang_zip_SOURCE_DIR}/Dependencies/Libraries/libslang.so
        INTERFACE_INCLUDE_DIRECTORIES ${slang_zip_SOURCE_DIR}/include
    )
endif()
add_library(slang::slang ALIAS slang) # optional: for slang::slang

# ---------- SOURCE COLLECTION ----------
file(GLOB_RECURSE SRC_FILES
    "${CMAKE_SOURCE_DIR}/*.cpp"
    "${CMAKE_SOURCE_DIR}/*.h"
)

# Exclude Dependencies directory files
list(FILTER SRC_FILES EXCLUDE REGEX "${CMAKE_SOURCE_DIR}/Dependencies/.*")

# ---------- EXECUTABLE ----------
add_executable(Lab1Graphics ${SRC_FILES})

# ---------- INCLUDE PATHS ----------
target_include_directories(Lab1Graphics PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/Dependencies/Code
    ${SLANG_SOURCE_DIR}  # Slang headers from its own directory
)

# ---------- PREPROCESSOR DEFINES ----------
target_compile_definitions(Lab1Graphics PRIVATE
    GLEW_STATIC
)

# ---------- LINK LIBRARIES ----------
link_directories(${CMAKE_SOURCE_DIR}/Dependencies/Libraries)

target_link_libraries(Lab1Graphics PRIVATE
    slang
    glfw3
    opengl32
    user32
    gdi32
    shell32
    glew32s
    winmm
)

# ---------- COMPILER WARNINGS ----------
if (MSVC)
    target_compile_options(Lab1Graphics PRIVATE /W4 /permissive-)
endif()

# ---------- POST-BUILD: Copy Slang DLL ----------
if(WIN32)
    add_custom_command(TARGET Lab1Graphics POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:slang>
        $<TARGET_FILE_DIR:Lab1Graphics>
        COMMENT "Copying Slang DLL to output directory"
    )
endif()

# ---------- INSTALL (optional) ----------
install(TARGETS Lab1Graphics DESTINATION bin)