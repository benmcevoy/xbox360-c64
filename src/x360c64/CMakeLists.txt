set(projectName "x360c64")
set(FAMILY rp2040)
set(BOARD pico_sdk)
set(TINYUSB_FAMILY_PROJECT_NAME_PREFIX "tinyusb_host_")

add_executable(${projectName}
        xinput_host.c
        hid_app.c
        main.c
        )

# configure UART - 8/9 for the waveshare ro2040-zero
# target_compile_definitions(${projectName} PRIVATE
#         PICO_DEFAULT_UART_TX_PIN=8
#         PICO_DEFAULT_UART_RX_PIN=9
# )

pico_enable_stdio_usb(${projectName} 0)
pico_enable_stdio_uart(${projectName} 1)

# Make sure TinyUSB can find tusb_config.h
target_include_directories(${projectName} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

# pull in common dependencies
target_link_libraries(${projectName} PUBLIC 
        pico_stdlib
        pico_bootsel_via_double_reset
        hardware_pwm 
        hardware_dma 
        tinyusb_board
        tinyusb_host
        tinyusb_device
        tinyusb_pico_pio_usb
        pico_multicore
        pico_unique_id )

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(${projectName})
