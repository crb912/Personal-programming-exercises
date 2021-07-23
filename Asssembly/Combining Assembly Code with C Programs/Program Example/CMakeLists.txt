cmake_minimum_required(VERSION 3.17)
project(ProgramExample)

enable_language(C ASM)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE
        c_with_assembly_code.c
        fix_assembly.s
        )
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

add_executable(cpp ${SOURCE})
