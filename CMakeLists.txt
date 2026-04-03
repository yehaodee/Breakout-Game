cmake_minimum_required(VERSION 3.10)
project(breakout)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 手动指定 raylib 路径
set(RAYLIB_INCLUDE_DIR D:/raylib/include)
set(RAYLIB_LIB_DIR D:/raylib/lib)

include_directories(${RAYLIB_INCLUDE_DIR})
link_directories(${RAYLIB_LIB_DIR})

add_executable(breakout
    main.cpp
    Game.cpp
    Ball.cpp
    Paddle.cpp
    Brick.cpp
    GameObject.cpp
    PhysicalObject.cpp
    VisualObject.cpp
    RectangleObject.cpp
)

target_link_libraries(breakout PRIVATE
    raylib
    opengl32
    gdi32
    winmm
)
