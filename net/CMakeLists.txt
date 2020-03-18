CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(net)

SET(CMAKE_CXX_COMPILER "/usr/bin/clang++")
SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O0 -g -fsanitize=address -std=c++11 -Wall -Wno-builtin-macro-redefined -Wno-deprecated -Werror -Wno-unused-function") 
SET(LIB_SRC
    ttcp/ttcp_blocking.cpp
    ttcp/common.cpp
)
SET(LIB_LIB
    common
    boost_program_options
)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

LINK_DIRECTORIES(/usr/local/boost/lib)
INCLUDE_DIRECTORIES(/usr/local/boost/include)


ADD_LIBRARY(common SHARED ${LIB_SRC})

ADD_EXECUTABLE(ttcp  ttcp/main.cpp)
ADD_DEPENDENCIES(ttcp common)
TARGET_LINK_LIBRARIES(ttcp ${LIB_LIB})
