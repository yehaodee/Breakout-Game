cmake_minimum_required(VERSION 3.10)
project(breakout)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-DWIN32_LEAN_AND_MEAN)

add_subdirectory(enet)

include_directories(${CMAKE_SOURCE_DIR}/raylib/include ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/enet/include)
link_directories(${CMAKE_SOURCE_DIR}/raylib/lib)

add_executable(breakout
    main.cpp
    Game.cpp
    GameServer.cpp
    GameClient.cpp
    GameSingle.cpp
    Ball.cpp
    Paddle.cpp
    Brick.cpp
    GameObject.cpp
    PhysicalObject.cpp
    VisualObject.cpp
    RectangleObject.cpp
    PowerUp.cpp
    PowerUpEffect.cpp
    Particle.cpp
    Network.cpp
)

target_link_libraries(breakout PRIVATE
    raylib
    enet
    opengl32
    gdi32
    winmm
)