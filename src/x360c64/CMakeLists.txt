set(projectName "x360c64")
set(FAMILY rp2040)
set(BOARD pico_sdk)
set(TINYUSB_FAMILY_PROJECT_NAME_PREFIX "tinyusb_host_")
#set(PICO_PIO_USB_PATH ${CMAKE_CURRENT_LIST_DIR}/pio_usb)

add_executable(${projectName})

# pico_generate_pio_header(${projectName} ${PICO_PIO_USB_PATH}/usb_tx.pio)
# pico_generate_pio_header(${projectName} ${PICO_PIO_USB_PATH}/usb_rx.pio)

target_sources(${projectName} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/xinput_host.c
        ${CMAKE_CURRENT_LIST_DIR}/hid_app.c
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        # ${PICO_PIO_USB_PATH}/pio_usb.c
        # ${PICO_PIO_USB_PATH}/pio_usb_device.c
        # ${PICO_PIO_USB_PATH}/pio_usb_host.c
        # ${PICO_PIO_USB_PATH}/usb_crc.c
        )

# configure UART
target_compile_definitions(${projectName} PRIVATE
        PICO_DEFAULT_UART_TX_PIN=16
        PICO_DEFAULT_UART_RX_PIN=17
)

pico_enable_stdio_usb(${projectName} 0)
pico_enable_stdio_uart(${projectName} 1)

# Make sure TinyUSB can find tusb_config.h
target_include_directories(${projectName} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

# pull in common dependencies
target_link_libraries(${projectName} PUBLIC 
        pico_stdlib
        hardware_dma 
        hardware_pio
        tinyusb_host 
        tinyusb_board
        pico_multicore
        pico_unique_id )


# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(${projectName})
