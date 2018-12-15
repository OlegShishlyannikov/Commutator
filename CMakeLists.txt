# Set Cmake minimum required version #
cmake_minimum_required( VERSION 3.6 )

# Set project name #
project( commutator_v1.0 )

# System settings #
set( CMAKE_SYSTEM_NAME "Generic" CACHE INTERNAL "CMake system name")
set( CMAKE_SYSTEM_PROCESSOR "arm" CACHE INTERNAL "CMake processor name" )
set( CMAKE_VERBOSE_MAKEFILE OFF )
enable_language( ASM )
enable_language( C )
enable_language( CXX )

# Toolchain #
set( CMAKE_ASM_COMPILER "/usr/bin/arm-none-eabi-as" CACHE INTERNAL "CMake asm compiler" )
set( CMAKE_C_COMPILER "/usr/bin/arm-none-eabi-gcc" CACHE INTERNAL "CMake c compiler" )
set( CMAKE_CXX_COMPILER "/usr/bin/arm-none-eabi-g++" CACHE INTERNAL "CMake cxx compiler" )
set( CMAKE_OBJCOPY_UTIL "/usr/bin/arm-none-eabi-objcopy" CACHE INTERNAL "CMake objcopy util" )
set( CMAKE_OBJDUMP_UTIL "/usr/bin/arm-none-eabi-objdump" CACHE INTERNAL "CMake objdump util" )
set( CMAKE_SIZE_UTIL "/usr/bin/arm-none-eabi-size" CACHE INTERNAL "CMake size util" )
set( STLINK_INFO_UTIL "/usr/local/bin/st-info" CACHE INTERNAL "Stlink info util" )
set( STLINK_FLASH_UTIL "/usr/local/bin/st-flash" CACHE INTERNAL "Stlink flash util" )
set( CMAKE_GDB_BACKEND "/usr/bin/arm-none-eabi-gdb" CACHE INTERNAL "CMake gdb backend" )
set( CMAKE_GDB_SERVER "/usr/local/bin/st-util" CACHE INTERNAL "CMake gdb server" )
set( CMAKE_GDB_FRONTEND "/usr/bin/cgdb" CACHE INTERNAL "CMake gdb frontend" )

message( "CMake ASM compiler is ${CMAKE_ASM_COMPILER}" )
message( "CMake C compiler is ${CMAKE_C_COMPILER}" )
message( "CMake CXX compiler is ${CMAKE_CXX_COMPILER}" )

# Flags #
set( CMAKE_ASM_FLAGS "-mthumb -mcpu=cortex-m3" CACHE INTERNAL "CMake asm flags" )
set( CMAKE_C_FLAGS "-Os -pipe -mthumb -mcpu=cortex-m3 -std=gnu11 -ffunction-sections -fomit-frame-pointer -fdata-sections -fno-builtin -Wl,--gc-sections" CACHE INTERNAL "CMake c flags" )
set( CMAKE_CXX_FLAGS "-Os -pipe -mthumb -mcpu=cortex-m3 -std=c++11 -flto -ffunction-sections -fomit-frame-pointer -fdata-sections -fno-builtin -Wl,--gc-sections" CACHE INTERNAL "CMake cxx flags" )
set( CMAKE_EXE_LINKER_FLAGS "-Os -pipe -mthumb -mcpu=cortex-m3 -flto -ffunction-sections -fomit-frame-pointer -fdata-sections -fno-builtin -Wl,--gc-sections -Wl,--no-wchar-size-warning --specs=nano.specs --specs=nosys.specs -Wl,-Map,${CMAKE_PROJECT_NAME}.map" CACHE INTERNAL "CMake exe linker flags" )

# Linker scripts #
set( LINKER_SCRIPT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ldscripts )
set( LDMEM_SCRIPT ${LINKER_SCRIPT_DIR}/mem.ld )
set( LDLIB_SCRIPT ${LINKER_SCRIPT_DIR}/libs.ld )
set( LDSECTIONS_SCRIPT ${LINKER_SCRIPT_DIR}/sections.ld )

# Mcu device #
set( MCU_DEVICE STM32F10X_XL )
set( HSE_VALUE 8000000 )

# Add some flags #
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D${MCU_DEVICE} -DHSE_VALUE=${HSE_VALUE} -DUSE_STDPERIPH_DRIVER" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D${MCU_DEVICE} -DHSE_VALUE=${HSE_VALUE} -DUSE_STDPERIPH_DRIVER" )
set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -D${MCU_DEVICE} -T${LDMEM_SCRIPT} -T${LDLIB_SCRIPT} -T${LDSECTIONS_SCRIPT}" )

# Project sources & headers #
set( PROJECT_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc CACHE INTERNAL "Project headers dir" )
file( GLOB PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.s ${CMAKE_CURRENT_SOURCE_DIR}/src/*.S )

# FatFs sources & headers #
set( FATFS_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/fatfs/inc CACHE INTERNAL "Fatfs headers dir" )
file( GLOB FATFS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/fatfs/src/*.c ${CMAKE_CURRENT_SOURCE_DIR}/fatfs/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/fatfs/src/*.cxx ${CMAKE_CURRENT_SOURCE_DIR}/fatfs/src/*.s ${CMAKE_CURRENT_SOURCE_DIR}/fatfs/src/*.S )

# HAL sources & headers #
set( HAL_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/stdperiph/inc CACHE INTERNAL "HAL headers dir" )
file( GLOB HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/stdperiph/src/*.c ${CMAKE_CURRENT_SOURCE_DIR}/stdperiph/src/*.cxx ${CMAKE_CURRENT_SOURCE_DIR}/stdperiph/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/stdperiph/src/*.s ${CMAKE_CURRENT_SOURCE_DIR}/stdperiph/src/*.S )

# CMSIS sources & headers #
set( CMSIS_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmsis/inc CACHE INTERNAL "CMSIS headers dir" )
file( GLOB CMSIS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cmsis/src/*.c ${CMAKE_CURRENT_SOURCE_DIR}/cmsis/src/*.cxx ${CMAKE_CURRENT_SOURCE_DIR}/cmsis/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/cmsis/src/*.s ${CMAKE_CURRENT_SOURCE_DIR}/cmsis/src/*.S )

# RTOS sources & headers #
set( RTOS_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rtos/inc CACHE INTERNAL "RTOS headers dir" )
file( GLOB RTOS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/rtos/src/*.c ${CMAKE_CURRENT_SOURCE_DIR}/rtos/src/*.cxx ${CMAKE_CURRENT_SOURCE_DIR}/rtos/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/rtos/src/*.s ${CMAKE_CURRENT_SOURCE_DIR}/rtos/src/*.S )

# Configure files #
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/clang_complete.in ${CMAKE_CURRENT_SOURCE_DIR}/.clang_complete )

# Include directories #
include_directories( ${PROJECT_HEADERS_DIR} ${HAL_HEADERS_DIR} ${CMSIS_HEADERS_DIR} ${RTOS_HEADERS_DIR} ${FATFS_HEADERS_DIR} )

# Add executables #
add_executable( ${CMAKE_PROJECT_NAME}.elf ${PROJECT_SOURCES} ${HAL_SOURCES} ${CMSIS_SOURCES} ${FATFS_SOURCES} ${RTOS_SOURCES} )

# Convert .elf to .hex and .bin #
add_custom_command( TARGET ${CMAKE_PROJECT_NAME}.elf POST_BUILD COMMAND ${CMAKE_OBJCOPY_UTIL} ARGS -Oihex ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.hex )
add_custom_command( TARGET ${CMAKE_PROJECT_NAME}.elf POST_BUILD COMMAND ${CMAKE_OBJCOPY_UTIL} ARGS -Obinary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin )

# Show .elf and .hex sizes #
add_custom_command( TARGET ${CMAKE_PROJECT_NAME}.elf POST_BUILD COMMAND ${CMAKE_SIZE_UTIL} ${CMAKE_PROJECT_NAME}.elf )
add_custom_command( TARGET ${CMAKE_PROJECT_NAME}.elf POST_BUILD COMMAND ${CMAKE_SIZE_UTIL} ${CMAKE_PROJECT_NAME}.hex )

# Some custom targets #
add_custom_target( flash COMMAND ${STLINK_FLASH_UTIL} write ${CMAKE_PROJECT_NAME}.bin 0x08000000 )

add_custom_target( dump COMMAND ${CMAKE_OBJDUMP_UTIL} -t ${CMAKE_PROJECT_NAME}.elf )
add_custom_target( size COMMAND ${CMAKE_SIZE_UTIL} ${CMAKE_PROJECT_NAME}.elf )
