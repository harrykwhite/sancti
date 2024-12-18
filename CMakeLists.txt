cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(sancti)

add_subdirectory(zeta_framework_3)

find_package(glfw3 CONFIG REQUIRED)

add_executable(sancti
	src/entry.cpp
	src/game.cpp
	src/sprites.cpp
	src/title_screen.cpp
	src/world.cpp
	src/enemy_types.cpp
	src/enemy_ents.cpp
	src/wanderer_enemy.cpp
	src/psycho_enemy.cpp

	src/game.h
	src/asset_indexes.h
	src/sprites.h
	src/title_screen.h
	src/world.h
	src/enemies.h
)

target_include_directories(sancti PRIVATE
    zeta_framework_3/zeta_framework_3/include
    zeta_framework_3/zf3_common/include
	zeta_framework_3/vendor/glad/include
)

target_link_libraries(sancti PRIVATE zeta_framework_3 zf3_common glfw)

target_compile_definitions(sancti PRIVATE GLFW_INCLUDE_NONE)

add_dependencies(sancti zf3_asset_packer)

add_custom_command(TARGET sancti POST_BUILD
	COMMAND $<TARGET_FILE:zf3_asset_packer> ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}
)
