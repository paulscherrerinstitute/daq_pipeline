set(BINARY ${CMAKE_PROJECT_NAME})

include_directories("/usr/local/include")
include_directories("external")

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp *.cc)

set(SOURCES ${SOURCES})

add_executable(${BINARY} ${SOURCES})
target_link_libraries(${BINARY} zmq cassandra)

add_library(${BINARY}_lib STATIC ${SOURCES})