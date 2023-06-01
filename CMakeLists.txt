cmake_minimum_required(VERSION 3.0)

project(AmosGB28181 C CXX)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/Bin)
set(CXX_FLAGS -g -Wall -std=c++11)

include_directories(./Include)


aux_source_directory(./Src/SIP SIP_SRCS)
# aux_source_directory(./SRC/Test TEST_SRCS)
aux_source_directory(./Src/Tools TOOLS_SRCS)
aux_source_directory(./Src/Network NETWORK_SRCS)

add_executable(TestSvr ./Src/Test/TestSIPServer.cpp ${SIP_SRCS} ${TOOLS_SRCS} ${NETWORK_SRCS})

target_link_libraries(TestSvr pthread crypto ssl)