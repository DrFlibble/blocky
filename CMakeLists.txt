cmake_minimum_required(VERSION 3.14)
project(blocky)

include(FindOpenGL)

find_package(PkgConfig REQUIRED)
pkg_check_modules(libgeek REQUIRED libgeek)
pkg_check_modules(libfrontier REQUIRED libfrontier)
pkg_check_modules(sdl2 sdl2)
pkg_check_modules(nlohmann_json nlohmann_json)

set(CMAKE_CXX_STANDARD 17)

SET(FLAGS_COMMON "-Wall -Werror")
SET(CMAKE_CXX_FLAGS_DEBUG "${FLAGS_COMMON} -O0 -g -fsanitize=address -fsanitize-address-use-after-scope -fno-omit-frame-pointer")
SET(CMAKE_CXX_FLAGS_RELEASE  "${FLAGS_COMMON} -O3")

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include)

add_definitions(
        ${libgeek_CFLAGS}
        ${libfrontier_CFLAGS}
        ${sdl2_CFLAGS}
        ${nlohmann_json_CFLAGS}
        -DGL_SILENCE_DEPRECATION -DGL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
)

add_library(brick SHARED
        include/brick/texture.h
        include/brick/perlin.h
        include/brick/ray.h
        include/brick/engine.h
        include/brick/overlay.h
        include/brick/shader.h
        include/brick/baseentity.h
        src/libbrick/texture.cpp
        src/libbrick/model.cpp
        src/libbrick/perlin.cpp
        src/libbrick/ray.cpp
        src/libbrick/engine.cpp
        src/libbrick/overlay.cpp
        src/libbrick/shader.cpp
        src/libbrick/baseentity.cpp
        src/libbrick/utils.h
        )
target_link_libraries(
        brick
        ${OPENGL_LIBRARIES}
        ${sdl2_LDFLAGS}
        ${libgeek_LDFLAGS}
)

add_executable(blocky
        src/blocky/main.cpp
        src/blocky/models/blockmodel.cpp
        src/blocky/chunk.cpp
        src/blocky/world.cpp
        src/blocky/blocky.h
        src/blocky/blockyshaders.h
        src/blocky/blockyshaders.cpp
        src/blocky/blocky.cpp
        src/blocky/player.cpp
        src/blocky/player.h
        src/blocky/blocks.h
        src/blocky/models/model.h
        src/blocky/overlays/crosshair.cpp
        src/blocky/overlays/crosshair.h
        src/blocky/overlays/infooverlay.cpp
        src/blocky/overlays/infooverlay.h
        src/blocky/overlays/statusbar.cpp
        src/blocky/overlays/statusbar.h
        src/blocky/entity.cpp
        src/blocky/entity.h
        src/blocky/models/spheremodel.cpp
        src/blocky/models/spheremodel.h
        src/blocky/spheremob.cpp
        src/blocky/spheremob.h
        src/blocky/overlays/menu.cpp
        src/blocky/overlays/menu.h)
target_link_libraries(
        blocky
        brick
        ${OPENGL_LIBRARIES}
        ${sdl2_LDFLAGS}
        ${libgeek_LDFLAGS}
        ${libfrontier_LDFLAGS}
        ${nlohmann_json_LDFLAGS})

add_executable(bott
        src/bott/main.cpp
        src/bott/simpleshader.cpp
        src/bott/terrainchunk.cpp
        src/bott/grassshader.h src/bott/grassshader.cpp src/bott/terrain.h src/bott/terrain.cpp src/bott/player.h src/bott/player.cpp src/bott/bott.cpp)
target_link_libraries(
        bott
        brick
        ${OPENGL_LIBRARIES}
        ${sdl2_LDFLAGS}
        ${libgeek_LDFLAGS}
        ${libfrontier_LDFLAGS}
        ${nlohmann_json_LDFLAGS})
