cmake_minimum_required(VERSION 3.9)

project(beeping_input)

set(CMAKE_C_STANDARD 11)

add_executable(beeping_input main.c recogniser.c to_morse_encoder.c from_morse_encoder.c)

# libsoundio
include(cmake/FindSoundIo.cmake)
target_include_directories(beeping_input PUBLIC ${SOUNDIO_INCLUDE_DIR})
target_link_libraries(beeping_input ${SOUNDIO_LIBRARY})

# kissfft
find_package(kissfft CONFIG REQUIRED COMPONENTS SHARED)
target_link_libraries(beeping_input kissfft::kissfft-float)