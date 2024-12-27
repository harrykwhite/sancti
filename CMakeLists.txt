cmake_minimum_required(VERSION 3.28)

project(sancti)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED YES)

add_subdirectory(zf4)

find_package(glfw3 CONFIG REQUIRED)

add_executable(sancti
	src/entry.c
	src/sprites.c
	src/ents/ent_types.c
	src/ents/player_ent.c
	src/ents/enemy_ent.c
	src/ents/hitbox_ent.c
	src/scenes/scene_types.c
	src/scenes/title_screen.c
	src/scenes/world.c

	src/asset_indexes.h
	src/sprites.h
	src/ents/ent_types.h
	src/scenes/scene_types.h
	src/scenes/title_screen.h
	src/scenes/world.h
)

target_include_directories(sancti PRIVATE
    zf4/zf4/include
    zf4/zf4_common/include
	zf4/vendor/glad/include
)

target_link_libraries(sancti PRIVATE zf4 zf4_common glfw)

target_compile_definitions(sancti PRIVATE GLFW_INCLUDE_NONE)

add_dependencies(sancti zf4_asset_packer)

add_custom_command(TARGET sancti POST_BUILD
	COMMAND $<TARGET_FILE:zf4_asset_packer> ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}
)
