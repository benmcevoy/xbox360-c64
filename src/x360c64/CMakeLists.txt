set(projectName "x360c64")
set(FAMILY rp2040)
set(BOARD pico_sdk)
set(TINYUSB_FAMILY_PROJECT_NAME_PREFIX "tinyusb_host_")

add_executable(${projectName})

target_sources(${projectName} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/xinput_host.c
        ${CMAKE_CURRENT_LIST_DIR}/hid_app.c
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        )

# configure UART
pico_enable_stdio_usb(${projectName} 0)
pico_enable_stdio_uart(${projectName} 1)

# Make sure TinyUSB can find tusb_config.h
target_include_directories(${projectName} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

# pull in common dependencies
target_link_libraries(${projectName} PUBLIC 
        pico_stdlib
        hardware_dma 
        tinyusb_host 
        tinyusb_board
        pico_multicore
        pico_unique_id )


# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(${projectName})
