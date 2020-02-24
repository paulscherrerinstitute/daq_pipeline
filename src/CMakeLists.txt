set(BINARY ${CMAKE_PROJECT_NAME})

include_directories("/usr/local/include")
link_directories("/usr/local/lib")

include_directories("external")

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp *.cc)

set(SOURCES ${SOURCES})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O3")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -O3")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-pg")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_library(${BINARY}_lib STATIC ${SOURCES})

add_executable(${BINARY} ${SOURCES})
target_link_libraries(${BINARY} zmq)

IF (APPLE)
    target_link_libraries(${BINARY} cassandra)
ELSE (UNIX)
    target_link_libraries(${BINARY} dse)
ENDIF (APPLE)
