cmake_minimum_required(VERSION 3.0)
project(tcp)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

#配置头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR} /base)
include_directories(${PROJECT_SOURCE_DIR} /test)
#link_directories()
set(SRC_LIST  tcp tcpSession.cpp TcpSession.h main.cc)
set(CMAKE_CXX_STANDARD 11)
add_executable(tcp ${SRC_LIST})
#aux_source_directory( . SRC_LIST)
#add_library()
target_link_libraries(tcp muduo_net muduo_base pthread)


