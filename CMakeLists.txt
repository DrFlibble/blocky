cmake_minimum_required(VERSION 3.19)
project(blocky)

include(FindOpenGL)
include(FindGLU)

find_package(PkgConfig REQUIRED)
pkg_check_modules(libgeek REQUIRED libgeek)
pkg_check_modules(sdl2 sdl2)

set(CMAKE_CXX_STANDARD 17)

SET(CMAKE_CXX_FLAGS_DEBUG "${FLAGS_COMMON} -O0 -g -fsanitize=address -fsanitize-address-use-after-scope -fno-omit-frame-pointer")

add_definitions(${libgeek_CFLAGS} ${sdl2_CFLAGS} -DGL_SILENCE_DEPRECATION -DGL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED)
add_executable(blocky src/main.cpp src/texture.cpp src/blockmodel.cpp src/chunk.cpp src/world.cpp src/perlin.cpp)
target_link_libraries(blocky ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES} ${GLU_LIBARIES} ${sdl2_LDFLAGS} ${libgeek_LIBRARIES})
