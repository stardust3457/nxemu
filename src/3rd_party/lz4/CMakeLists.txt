add_library(lz4 STATIC
    lz4.c
    lz4hc.c
    lz4frame.c
    xxhash.c
)

target_include_directories(lz4 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_property(TARGET lz4 PROPERTY POSITION_INDEPENDENT_CODE ON)

if(NOT TARGET lz4::lz4)
    add_library(lz4::lz4 ALIAS lz4)
endif()
