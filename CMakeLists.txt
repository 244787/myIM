cmake_minimum_required(VERSION 3.0)
project(tcp)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

#配置头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR} /base)
include_directories(${PROJECT_SOURCE_DIR} /test)
link_directories(usr/lib64)
file(GLOB SRC_LIST ./base/*.cpp ./base/*.h  ./server/*.cpp)
#set(SRC_LIST    ./base/*.cpp ./base/*.h ./server/main.c pp)
set(CMAKE_CXX_STANDARD 17)
add_executable(tcp ${SRC_LIST})
#aux_source_directory( . SRC_LIST)
target_link_libraries(tcp muduo_net muduo_base pthread jsoncpp )
target_link_libraries(tcp libmysqlclient.so)


