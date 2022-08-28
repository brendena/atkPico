add_library(atk_pico_lib INTERFACE)
target_include_directories(atk_pico_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(atk_pico_lib INTERFACE hardware_pio)
target_sources(atk_pico_lib INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/atkPico.c
    ${CMAKE_CURRENT_LIST_DIR}/atkPico.h
    ${CMAKE_CURRENT_LIST_DIR}/atk.pio)
pico_generate_pio_header(atk_pico_lib ${CMAKE_CURRENT_LIST_DIR}/atk.pio)
