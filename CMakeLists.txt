cmake_minimum_required(VERSION 4.0)
project(OsurukEngine)

set(CMAKE_CXX_STANDARD 14)

# Windows: statik linkleme (DLL ihtiyacını ortadan kaldırır)
if(WIN32 AND MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
endif()

if(APPLE)
    # Build Apple Silicon (arm64) only for now to match your current dependencies
    set(CMAKE_OSX_ARCHITECTURES "arm64")
    # Set a reasonable minimum macOS version (adjust if you need to support older versions)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0")
endif()

# OpenGL
find_package(OpenGL REQUIRED)

include_directories(OsurukEngine)
include_directories(OsurukEngine/Engine)
include_directories(ThirdParty/imgui)

# ----------------------------------------
# 1️⃣ GLEW (statik, kendi kaynak dosyasından)
# ----------------------------------------
add_definitions(-DGLEW_STATIC)
include_directories(ThirdParty/GLEW/include)

# ----------------------------------------
# 2️⃣ GLFW (add_subdirectory yöntemi)
# ----------------------------------------
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw-3.4)

# ----------------------------------------
# 3️⃣ Lua
# ----------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/lua-5.4.8)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty)

file(GLOB ENGINE_SOURCES OsurukEngine/Engine/*.cpp)
file(GLOB IMGUI_SOURCES ThirdParty/imgui/*.cpp)

# ----------------------------------------
# 4️⃣ Uygulama kaynakları
# ----------------------------------------
if(APPLE)
    add_executable(OsurukEngine MACOSX_BUNDLE
            OsurukEngine/main.cpp
            ThirdParty/GLEW/glew.c
            ${ENGINE_SOURCES}
            ${IMGUI_SOURCES}
            ${BULLET_SOURCES}
    )
elseif(WIN32)
    add_executable(OsurukEngine WIN32
            OsurukEngine/main.cpp
            ThirdParty/GLEW/glew.c
            ${ENGINE_SOURCES}
            ${IMGUI_SOURCES}
            ${BULLET_SOURCES}
    )
else()
    add_executable(OsurukEngine
            OsurukEngine/main.cpp
            ThirdParty/GLEW/glew.c
            ${ENGINE_SOURCES}
            ${IMGUI_SOURCES}
            ${BULLET_SOURCES}
    )
endif()


# ----------------------------------------
# 5️⃣ Bağlanacak kütüphaneler
# ----------------------------------------
target_link_libraries(OsurukEngine
        PRIVATE
        OpenGL::GL      # Bazı CMake sürümlerinde OpenGL::OpenGL olabilir
        glfw
        lua
)

# 6️⃣ exaaa

# macOS bundle içine assetleri kopyala
if(APPLE)
    add_custom_command(TARGET OsurukEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:OsurukEngine>/../Resources"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/OsurukEngine/Textures"
                "$<TARGET_FILE_DIR:OsurukEngine>/../Resources/Textures"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/OsurukEngine/Shaders"
                "$<TARGET_FILE_DIR:OsurukEngine>/../Resources/Shaders"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/OsurukEngine/Models"
                "$<TARGET_FILE_DIR:OsurukEngine>/../Resources/Models"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/OsurukEngine/Worlds"
                "$<TARGET_FILE_DIR:OsurukEngine>/../Resources/Worlds"

        VERBATIM
    )
elseif(WIN32)
    add_custom_command(TARGET OsurukEngine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:OsurukEngine>/Resources"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/OsurukEngine/Textures"
            "$<TARGET_FILE_DIR:OsurukEngine>/Resources/Textures"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/OsurukEngine/Shaders"
            "$<TARGET_FILE_DIR:OsurukEngine>/Resources/Shaders"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/OsurukEngine/Models"
            "$<TARGET_FILE_DIR:OsurukEngine>/Resources/Models"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/OsurukEngine/Worlds"
            "$<TARGET_FILE_DIR:OsurukEngine>/Resources/Worlds"

            VERBATIM
    )
endif()

# Projenin kökü: ${CMAKE_SOURCE_DIR}
# Sizde Textures: ${CMAKE_SOURCE_DIR}/OsurukEngine/Textures
# Platforma göre ASSET_DIR: macOS bundle için ../Resources, diğerlerinde kaynak ağacı
if(APPLE)
    add_compile_definitions(ASSET_DIR="../Resources")
elseif(WIN32)
    add_compile_definitions(ASSET_DIR="Resources")
else()
    add_compile_definitions(ASSET_DIR="${CMAKE_SOURCE_DIR}/OsurukEngine")
endif()



# Embed third-party dylibs into the .app and fix install names (macOS)
if(APPLE)
    # Ensure runtime search paths look inside the app bundle's Frameworks
    set_target_properties(OsurukEngine PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "@executable_path/../Frameworks;@loader_path/../Frameworks"
    )

    # Target to embed third-party dylibs into the .app and fix install names
    add_custom_target(FixBundle
        COMMAND ${CMAKE_COMMAND} -DAPP=$<TARGET_BUNDLE_DIR:BerkushsPlayground> -DSEARCH_DIRS=/opt/homebrew/lib\;/usr/local/lib\;${CMAKE_BINARY_DIR} -P ${CMAKE_SOURCE_DIR}/cmake/MacBundleFixup.cmake
        DEPENDS OsurukEngine
        COMMENT "Embedding dylibs into .app (BundleUtilities/fixup_bundle)"
        VERBATIM
    )

    # Automatically fix bundle and ad-hoc sign on Release builds
    add_custom_command(TARGET OsurukEngine POST_BUILD
        # Only in Release: run fixup_bundle (equivalent to FixBundle target)
        COMMAND $<$<CONFIG:Release>:${CMAKE_COMMAND}> $<$<CONFIG:Release>:-DAPP=$<TARGET_BUNDLE_DIR:OsurukEngine>> $<$<CONFIG:Release>:-DSEARCH_DIRS=/opt/homebrew/lib\;/usr/local/lib\;${CMAKE_BINARY_DIR}> $<$<CONFIG:Release>:-P> $<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/cmake/MacBundleFixup.cmake>
        # Only in Release: recursively sign all nested items with the same identity (ad-hoc by default). Note: omit hardened runtime to avoid library validation TeamID mismatch on ad-hoc.
        COMMAND $<$<CONFIG:Release>:${CMAKE_COMMAND}> $<$<CONFIG:Release>:-DAPP=$<TARGET_BUNDLE_DIR:OsurukEngine>> $<$<CONFIG:Release>:-DIDENTITY=-> $<$<CONFIG:Release>:-P> $<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/cmake/MacRecursiveSign.cmake>
        COMMENT "Release: fix bundle deps and recursively ad-hoc sign .app (no hardened runtime)"
        VERBATIM
    )
endif()