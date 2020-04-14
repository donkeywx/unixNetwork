CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(net)

SET(CMAKE_CXX_COMPILER "/usr/bin/clang++")
SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O0 -g -fsanitize=address -std=c++11 -Wall -Wno-builtin-macro-redefined -Wno-deprecated -Werror -Wno-unused-function") 
SET(LIB_SRC
    net/socket.cpp
    net/tcpConn.cpp
    net/tcpServer.cpp
)
SET(LIB_LIB
    libgo
    dl
    common
    boost_program_options
    pthread
)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

LINK_DIRECTORIES(/usr/local/boost/lib)
INCLUDE_DIRECTORIES(/usr/local/boost/include)


ADD_LIBRARY(common SHARED ${LIB_SRC})


ADD_EXECUTABLE(testLibgo  tests/testLibgo.cpp)
ADD_DEPENDENCIES(testLibgo common)
TARGET_LINK_LIBRARIES(testLibgo ${LIB_LIB})


ADD_EXECUTABLE(testSocket  tests/testSocket.cpp)
ADD_DEPENDENCIES(testSocket common)
TARGET_LINK_LIBRARIES(testSocket ${LIB_LIB})