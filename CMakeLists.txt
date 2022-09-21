cmake_minimum_required(VERSION 3.19)
project(blocky)

include(FindOpenGL)

find_package(PkgConfig REQUIRED)
pkg_check_modules(libgeek REQUIRED libgeek)
pkg_check_modules(sdl2 sdl2)
pkg_check_modules(nlohmann_json nlohmann_json)

set(CMAKE_CXX_STANDARD 17)

SET(FLAGS_COMMON "-Wall -Werror")
SET(CMAKE_CXX_FLAGS_DEBUG "${FLAGS_COMMON} -O0 -g -fsanitize=address -fsanitize-address-use-after-scope -fno-omit-frame-pointer")
SET(CMAKE_CXX_FLAGS_RELEASE  "${FLAGS_COMMON} -O3")

include_directories(${CMAKE_SOURCE_DIR}/src)

add_definitions(${libgeek_CFLAGS} ${sdl2_CFLAGS} -DGL_SILENCE_DEPRECATION -DGL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED ${nlohmann_json_CFLAGS})
add_executable(blocky
        src/main.cpp
        src/texture.cpp
        src/blockmodel.cpp
        src/chunk.cpp
        src/world.cpp
        src/perlin.cpp
        src/blocky.h
        src/shader.h
        src/shader.cpp
        src/ray.h
        src/ray.cpp
        src/engine.h
        src/engine.cpp
        src/blocky.cpp
        src/overlay.cpp
        src/overlay.h
        src/utils.h
        src/player.cpp
        src/player.h
        src/blocks.h
        src/overlays/crosshair.cpp
        src/overlays/crosshair.h
        src/overlays/infooverlay.cpp src/overlays/infooverlay.h src/overlays/statusbar.cpp src/overlays/statusbar.h)
target_link_libraries(blocky ${OPENGL_LIBRARIES} ${sdl2_LDFLAGS} ${libgeek_LDFLAGS} ${nlohmann_json_LDFLAGS})
