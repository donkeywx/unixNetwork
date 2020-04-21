CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(net)

SET(CMAKE_CXX_COMPILER "/usr/bin/clang++")
SET(CMAKE_VERBOSE_MAKEFILE ON)
# -g -fsanitize=address 
SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O2 -std=c++11 -Wall -Wno-builtin-macro-redefined -Wno-deprecated -Werror -Wno-unused-function") 
SET(LIB_SRC
    net/socket.cpp
    net/tcpConn.cpp
    net/tcpServer.cpp
    net/httpServer.cpp

    net/http/http11_parser.cc
    net/http/httpclient_parser.cc
    net/http/httpRequest.cpp
    net/http/httpResponse.cpp
    net/http/httpParse.cpp
    net/http/httpSession.cpp
    net/http/servlet.cpp
)
SET(LIB_LIB
    common
    libgo
    dl
    
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

ADD_EXECUTABLE(testTcpServer  tests/testTcpServer.cpp)
ADD_DEPENDENCIES(testTcpServer common)
TARGET_LINK_LIBRARIES(testTcpServer ${LIB_LIB})

ADD_EXECUTABLE(testHttpServer  tests/testHttpServer.cpp)
ADD_DEPENDENCIES(testHttpServer common)
TARGET_LINK_LIBRARIES(testHttpServer ${LIB_LIB})