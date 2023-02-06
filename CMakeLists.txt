cmake_minimum_required(VERSION 3.20)
project(sdl)

add_compile_options(-std=c++17 -g -Wall -Wextra)

# find_library(SDL2_INCLUDES libSDL2.dylib)
set(SDL2_INCLUDES "/opt/homebrew/include")
set(SDL2_LIBRARIES "/opt/homebrew/lib/libSDL2.dylib")

add_library(gamelib
  graphics.cpp
  player.cpp
  world.cpp
)
target_include_directories(gamelib PUBLIC ${SDL2_INCLUDES})

add_executable(main main.cpp)
target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${SDL2_INCLUDES})
target_link_libraries(main PUBLIC gamelib ${SDL2_LIBRARIES})