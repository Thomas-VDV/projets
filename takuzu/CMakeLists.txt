cmake_minimum_required(VERSION 2.8.12)

project(takuzu C)

enable_testing()
include(CTest)

set(CMAKE_C_FLAGS "-std=c99 -g -Wall")

## find SDL2
include(SDL2.cmake)
message(STATUS "SDL2 include dir: ${SDL2_ALL_INC}")
message(STATUS "SDL2 all libraries: ${SDL2_ALL_LIBS}")

link_directories(${CMAKE_SOURCE_DIR})
include_directories(${SDL2_ALL_INC})

add_library(game game.c game_aux.c queue.c game_tools.c)

add_executable(game_text game_text.c)
target_link_libraries(game_text game)

add_executable(game_test_emtexier game_test_emtexier.c)
target_link_libraries(game_test_emtexier game)

add_executable(game_test_tvieiradoval game_test_tvieiradoval.c)
target_link_libraries(game_test_tvieiradoval game)

add_executable(game_test_aupasquet game_test_aupasquet.c)
target_link_libraries(game_test_aupasquet game)

add_executable(game_solve game_solve.c)
target_link_libraries(game_solve game)

add_executable(game_sdl main.c game_sdl.c)
target_link_libraries(game_sdl game ${SDL2_ALL_LIBS} m) 

add_test(test_aupasquet_dummy ./game_test_aupasquet dummy)
add_test(test_aupasquet_game_new_empty ./game_test_aupasquet game_new_empty)
add_test(test_aupasquet_game_delete ./game_test_aupasquet game_delete)
add_test(test_aupasquet_game_get_number ./game_test_aupasquet game_get_number)
add_test(test_aupasquet_game_is_empty ./game_test_aupasquet game_is_empty)
add_test(test_aupasquet_game_check_move ./game_test_aupasquet game_check_move)
add_test(test_aupasquet_game_restart ./game_test_aupasquet game_restart)
add_test(test_aupasquet_game_default ./game_test_aupasquet game_default)
add_test(test_aupasquet_game_nb_rows ./game_test_aupasquet game_nb_rows)
add_test(test_aupasquet_game_nb_cols ./game_test_aupasquet game_nb_cols)
add_test(test_aupasquet_game_new_empty_ext ./game_test_aupasquet game_new_empty_ext)
add_test(test_aupasquet_game_new_ext ./game_test_aupasquet game_new_ext)
add_test(test_aupasquet_game_undo ./game_test_aupasquet game_undo)
add_test(test_aupasquet_game_redo ./game_test_aupasquet game_redo)

add_test(test_emtexier_dummy ./game_test_emtexier dummy)
add_test(test_emtexier_game_copy ./game_test_emtexier game_copy)
add_test(test_emtexier_game_set_square ./game_test_emtexier game_set_square)
add_test(test_emtexier_game_get_next_square ./game_test_emtexier game_get_next_square)
add_test(test_emtexier_game_is_immutable ./game_test_emtexier game_is_immutable)
add_test(test_emtexier_game_play_move ./game_test_emtexier game_play_move)
add_test(test_emtexier_game_default_solution ./game_test_emtexier game_default_solution)
add_test(test_emtexier_game_is_unique ./game_test_emtexier game_is_unique)
add_test(test_emtexier_game_is_wrapping ./game_test_emtexier game_is_wrapping)
add_test(test_emtexier_game_load ./game_test_emtexier game_load)
add_test(test_emtexier_game_save ./game_test_emtexier game_save)
add_test(test_emtexier_game_solve ./game_test_emtexier game_solve)
add_test(test_emtexier_game_nb_solutions ./game_test_emtexier game_nb_solutions)

add_test(test_tvieiradoval_dummy ./game_test_tvieiradoval dummy)
add_test(test_tvieiradoval_game_new ./game_test_tvieiradoval game_new)
add_test(test_tvieiradoval_game_equal ./game_test_tvieiradoval game_equal)
add_test(test_tvieiradoval_game_get_square ./game_test_tvieiradoval game_get_square)
add_test(test_tvieiradoval_game_get_next_number ./game_test_tvieiradoval game_get_next_number)
add_test(test_tvieiradoval_game_is_over ./game_test_tvieiradoval game_is_over)
add_test(test_tvieiradoval_game_has_error ./game_test_tvieiradoval game_has_error)
add_test(test_tvieiradoval_game_print ./game_test_tvieiradoval game_print)


#file copy
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/default.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/solution.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/empty_file.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/4x4_0.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/4x4_1.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/4x4_2.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/4x4_72.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/4x4_90.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})



