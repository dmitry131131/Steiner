set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Steiner.cpp 
            ${CMAKE_CURRENT_SOURCE_DIR}/Parser.cpp)

target_sources(${TARGET} PRIVATE ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
# target_sources(${TEST_TARGET} PRIVATE ${SOURCES})