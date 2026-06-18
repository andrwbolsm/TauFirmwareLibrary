cmake_minimum_required(VERSION 3.23)

project(AvionicProject C)

add_executable(AvionicFirmware)
target_compile_definitions(AvionicFirmware PRIVATE projCOVERAGE_TEST=0)

target_sources(AvionicFirmware PRIVATE
    app/src/main.c
    tasks/src/read_barometer_task.c
    pal/src/sensors.c
    drivers/bmp280/src/bmp280_driver.c
    drivers/bmp280/src/bmp280_mock.c
    drivers/bmp280/src/bmp280.c

    #FreeRTOS source files
    os/pc/freertos_hooks.c
    os/FreeRTOS/tasks.c
    os/FreeRTOS/list.c
    os/FreeRTOS/queue.c
    os/FreeRTOS/timers.c
    os/FreeRTOS/portable/MSVC-MingW/port.c
    os/FreeRTOS/portable/MemMang/heap_1.c
)

target_include_directories(AvionicFirmware PRIVATE
    app/inc
    os/FreeRTOS
    os/FreeRTOS/include
    tasks/inc
    pal/inc
    drivers/bmp280/inc
)

option(PC "Build for PC simulation" OFF)

# Run it for PC simulation only target possible for the moment
if(PC)
    target_include_directories(AvionicFirmware PRIVATE
        os/FreeRTOS/portable/MSVC-MingW
        os/pc
        bsp/inc/pc 
    )

    target_sources(AvionicFirmware PRIVATE
        bsp/src/pc/udp.c
    )

    target_link_libraries(AvionicFirmware PRIVATE winmm ws2_32)
    target_compile_definitions(AvionicFirmware PRIVATE DEBUG_PC)
endif()