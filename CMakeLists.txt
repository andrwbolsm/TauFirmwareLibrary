cmake_minimum_required(VERSION 3.23)

project(AvionicProject C)

add_executable(AvionicFirmware)
target_compile_definitions(AvionicFirmware PRIVATE projCOVERAGE_TEST=0)

target_sources(AvionicFirmware PRIVATE
    app/src/main.c
    app/src/tasks/read_barometer_task.c
    bsp/src/hooks/freertos_hooks.c
    bsp/src/sensors.c

    #FreeRTOS source files
    FreeRTOS/tasks.c
    FreeRTOS/list.c
    FreeRTOS/queue.c
    FreeRTOS/timers.c
    FreeRTOS/portable/MSVC-MingW/port.c
    FreeRTOS/portable/MemMang/heap_1.c
)

target_include_directories(AvionicFirmware PRIVATE
    app/inc
    app/inc/tasks
    bsp/inc
    FreeRTOS
    FreeRTOS/include
    FreeRTOS/portable/MSVC-MingW
)

target_link_libraries(AvionicFirmware PRIVATE winmm)