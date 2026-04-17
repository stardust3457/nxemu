cmake_minimum_required(VERSION 3.18)

add_library(network STATIC
    announce_multiplayer_session.cpp
    network.cpp
    packet.cpp
    room.cpp
    room_member.cpp
    verify_user.cpp
)

target_include_directories(network PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(network PUBLIC yuzu_common enet::enet PRIVATE fmt::fmt)
target_compile_definitions(network PRIVATE NOMINMAX)
