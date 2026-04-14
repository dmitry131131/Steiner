cmake_minimum_required(VERSION 4.1.0)
project(Steiner)

set(TARGET steiner)
set(TEST_TARGET steiner_test)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${TARGET})
# add_executable(${TEST_TARGET})

# # Enable testing
# enable_testing()

# Install dependencies
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM.cmake) 

find_package(GTest REQUIRED)    # TODO Fix to CPM code

CPMAddPackage(
    NAME CLI11
    GIT_REPOSITORY "https://github.com/CLIUtils/CLI11"
    VERSION 2.5.0
)

CPMAddPackage(
    NAME nlohmann_json
    GIT_REPOSITORY "https://github.com/nlohmann/json.git"
    VERSION 3.12.0
)

# Link Libraries
target_link_libraries(${TARGET} PRIVATE CLI11::CLI11 nlohmann_json::nlohmann_json)

# target_link_libraries(${TEST_TARGET} PRIVATE GTest::GTest GTest::Main)
# target_link_libraries(${TEST_TARGET} PRIVATE cpuid CLI11::CLI11)

# Include directory
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
# target_include_directories(${TEST_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Compile options
target_compile_options(${TARGET} PUBLIC
    -Wall 
    -Wextra 
    $<$<CONFIG:DEBUG>:-g -fsanitize=address>
    $<$<CONFIG:RELEASE>:-O3>
)

# Link options
target_link_options(${TARGET} PRIVATE -lpthread -lstdc++fs
    $<$<CONFIG:DEBUG>: -fsanitize=address>
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)

set_target_properties(${TARGET}
                      PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

install(TARGETS ${TARGET}
        RUNTIME DESTINATION bin)