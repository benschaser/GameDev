cmake_minimum_required(VERSION 3.20)
project(sdl)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-std=c++17 -g -Wall -Wextra)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

add_library(gamelib
  graphics.cpp
  player.cpp
  world.cpp
  tilemap.cpp
  camera.cpp
  physics.cpp
  fsm.cpp
  command.cpp
  engine.cpp
  settings.cpp
  randomness.cpp
  sprite.cpp
  animatedsprite.cpp
  audio.cpp
  level.cpp
  quadtree.cpp
  combat.cpp
  entity.cpp
  enemy.cpp
  enemytype.cpp
  quadtree.cpp
)

target_include_directories(gamelib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS})
target_link_libraries(gamelib PUBLIC SDL2::SDL2 SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer)

add_executable(main main.cpp)
target_link_libraries(main PUBLIC gamelib)

add_executable(test_qt test_qt.cpp)
target_link_libraries(test_qt PUBLIC gamelib)

add_executable(animate_qt animate_quadtree.cpp)
target_link_libraries(animate_qt PUBLIC gamelib)

add_executable(performance_qt perf_quadtree.cpp)
target_link_libraries(performance_qt PUBLIC gamelib)


add_executable(test_quadtree test_quadtree.cpp)
target_link_libraries(test_quadtree PUBLIC gamelib)

