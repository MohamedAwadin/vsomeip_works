cmake_minimum_required(VERSION 3.5)
project(vsomeip_example)

set(EXAMPLE_CONFIG_FILES
    "vsomeip.json"
)

find_package(vsomeip3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread)

include_directories(
    ${Boost_INCLUDE_DIR}
    ${VSOMEIP_INCLUDE_DIRS}
)

add_executable(server server_main.cpp server_simple.hpp)
target_link_libraries(server vsomeip3 ${Boost_LIBRARIES})

add_executable(client client_main.cpp client_simple.hpp)
target_link_libraries(client vsomeip3 ${Boost_LIBRARIES})