add_library(hal
    src/GpioOut.cxx
    src/GpioIn.cxx
    src/LinuxPwm.cxx
)

target_include_directories(hal
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBGPIOD REQUIRED libgpiod)

target_include_directories(hal
    PUBLIC
        ${LIBGPIOD_INCLUDE_DIRS}
)

target_link_libraries(hal
    PUBLIC
        ${LIBGPIOD_LIBRARIES}
)
