set(BINARY ${CMAKE_PROJECT_NAME})

include_directories("/usr/local/include")
link_directories("/usr/local/lib")

include_directories("external")

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp *.cc)

set(SOURCES ${SOURCES})

add_library(${BINARY}_lib STATIC ${SOURCES})

add_executable(${BINARY} ${SOURCES})
target_link_libraries(${BINARY} zmq cassandra)

