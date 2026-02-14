add_library(drivers_bts7960
    src/BTS7960Driver.cxx
)

target_include_directories(drivers_bts7960
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(drivers_bts7960
    PUBLIC
        hal
        motordc
)
