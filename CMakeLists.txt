cmake_minimum_required(VERSION 3.10)

project("HillClimbRacing Trainer" VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(
        SOURCES
        main.cpp
        resource.rc
)
add_executable(HCRTrainer ${SOURCES})
target_link_libraries(
        HCRTrainer
        user32
        gdi32
        kernel32
        psapi
)

set_target_properties(HCRTrainer PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
