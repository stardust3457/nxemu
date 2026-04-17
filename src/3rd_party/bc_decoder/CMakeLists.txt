add_library(bc_decoder STATIC
    bc_decoder.cpp
)

target_include_directories(bc_decoder PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_property(TARGET bc_decoder PROPERTY POSITION_INDEPENDENT_CODE ON)
