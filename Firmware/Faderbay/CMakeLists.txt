cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME Faderbay)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # App layer
    App/app_state.c
    App/fader_control.c
    App/midi_control.c
    App/ui_control.c
    # Driver layer
    Drivers/fader_driver.c
    Drivers/mux_driver.c
    Drivers/midi_driver.c
    Drivers/display_driver.c
    Drivers/encoder_driver.c
    Drivers/button_driver.c
    # Resource Manager layer
    ResourceMgr/scheduler.c
    ResourceMgr/adc.c
    ResourceMgr/spi.c
    ResourceMgr/uart.c
    ResourceMgr/gpio.c
    ResourceMgr/timer.c
    ResourceMgr/syscalls.c
    # Config
    Config/config.c
    # u8g2 — ST7920 SPI full buffer mode
    Lib/u8g2/csrc/u8g2_setup.c
    Lib/u8g2/csrc/u8g2_d_setup.c
    Lib/u8g2/csrc/u8g2_d_memory.c
    Lib/u8g2/csrc/u8g2_buffer.c
    Lib/u8g2/csrc/u8g2_hvline.c
    Lib/u8g2/csrc/u8g2_line.c
    Lib/u8g2/csrc/u8g2_box.c
    Lib/u8g2/csrc/u8g2_circle.c
    Lib/u8g2/csrc/u8g2_font.c
    Lib/u8g2/csrc/u8g2_fonts.c
    Lib/u8g2/csrc/u8g2_bitmap.c
    Lib/u8g2/csrc/u8g2_intersection.c
    Lib/u8g2/csrc/u8g2_ll_hvline.c
    Lib/u8g2/csrc/u8g2_message.c
    Lib/u8g2/csrc/u8g2_polygon.c
    Lib/u8g2/csrc/u8x8_string.c
    Lib/u8g2/csrc/u8x8_setup.c
    Lib/u8g2/csrc/u8x8_u8toa.c
    Lib/u8g2/csrc/u8x8_8x8.c
    Lib/u8g2/csrc/u8x8_display.c
    Lib/u8g2/csrc/u8x8_fonts.c
    Lib/u8g2/csrc/u8x8_byte.c
    Lib/u8g2/csrc/u8x8_gpio.c
    Lib/u8g2/csrc/u8x8_cad.c
    Lib/u8g2/csrc/u8x8_d_st7920.c
    # NanoLog
    Lib/NanoLog/nlog.c
)

# Suppress all warnings in vendored libraries — they are not our code to fix
set(VENDORED_SOURCES
    Lib/u8g2/csrc/u8g2_setup.c
    Lib/u8g2/csrc/u8g2_d_setup.c
    Lib/u8g2/csrc/u8g2_d_memory.c
    Lib/u8g2/csrc/u8g2_buffer.c
    Lib/u8g2/csrc/u8g2_hvline.c
    Lib/u8g2/csrc/u8g2_line.c
    Lib/u8g2/csrc/u8g2_box.c
    Lib/u8g2/csrc/u8g2_circle.c
    Lib/u8g2/csrc/u8g2_font.c
    Lib/u8g2/csrc/u8g2_fonts.c
    Lib/u8g2/csrc/u8g2_bitmap.c
    Lib/u8g2/csrc/u8g2_intersection.c
    Lib/u8g2/csrc/u8g2_ll_hvline.c
    Lib/u8g2/csrc/u8g2_message.c
    Lib/u8g2/csrc/u8g2_polygon.c
    Lib/u8g2/csrc/u8x8_string.c
    Lib/u8g2/csrc/u8x8_setup.c
    Lib/u8g2/csrc/u8x8_u8toa.c
    Lib/u8g2/csrc/u8x8_8x8.c
    Lib/u8g2/csrc/u8x8_display.c
    Lib/u8g2/csrc/u8x8_fonts.c
    Lib/u8g2/csrc/u8x8_byte.c
    Lib/u8g2/csrc/u8x8_gpio.c
    Lib/u8g2/csrc/u8x8_cad.c
    Lib/u8g2/csrc/u8x8_d_st7920.c
    Lib/NanoLog/nlog.c
)
set_source_files_properties(${VENDORED_SOURCES} PROPERTIES
    COMPILE_OPTIONS "-w"
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    .
    App
    Drivers
    ResourceMgr
    Config
    Lib/u8g2/csrc
    Lib/NanoLog
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Strict warning flags — do not add -fanalyzer here (HAL noise)
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
    -Wall -Wextra -Wpedantic
    -Wshadow -Wundef -Wcast-align -Wpointer-arith
    -Wstrict-prototypes -Wmissing-prototypes
    -Wformat=2 -Wnull-dereference
    # -Wstack-usage threshold relaxed from 512 to 1500 because NanoLog's vsnprintf
    # legitimately uses ~1300 B. Functions exceeding 1500 B should be reviewed.
    # ISR callbacks must never log (would push 1.3 kB onto IRQ stack).
    -Wstack-usage=1500
    $<$<CONFIG:Debug>:-Og -g3 -DDEBUG>
    $<$<CONFIG:Release>:-Os -flto -DNDEBUG>
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
