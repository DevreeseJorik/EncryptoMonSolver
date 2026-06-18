cmake_minimum_required(VERSION 3.16)
project(EncryptoMonSolver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)

find_package(Threads REQUIRED)

add_executable(encryptomon.exe
    src/EncryptoMon.cpp
    src/Gen4RNG.cpp
    src/OptimizedSolver.cpp
    src/SaveCorruptionSolver.cpp
    src/main.cpp
    src/structs.cpp
)

target_include_directories(encryptomon.exe PRIVATE src)
target_compile_options(encryptomon.exe PRIVATE -O3)
target_link_libraries(encryptomon.exe PRIVATE Threads::Threads)
