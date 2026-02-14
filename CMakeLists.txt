cmake_minimum_required(VERSION 3.16)
project(robot_fw LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(packages/HAL)
add_subdirectory(packages/MotorDC)
add_subdirectory(packages/Drivers/BTS7960)
