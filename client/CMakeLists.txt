cmake_minimum_required(VERSION 3.08)
project(client)

set(CMAKE_CXX_STANDARD 14)

add_executable(client main.cpp 
    client_provider.cpp 
    device_provider.cpp 
    track_names_provider.cpp 
    udp_client.cpp)

target_link_libraries(client pthread)