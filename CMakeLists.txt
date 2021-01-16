cmake_minimum_required(VERSION 3.11)
project(untitled1)

set(CMAKE_CXX_STANDARD 14)

list(APPEND CMAKE_PREFIX_PATH "/root/vcpkg/installed/x64-linux")
set(CMAKE_TOOLCHAIN_FILE "/root/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(VCPKG_TARGET_TRIPLET "x64-linux")
set(VCMAKE_PREFIX_PATH "/root/vcpkg/installed/x64-linux")

find_package(GTest CONFIG REQUIRED)
find_package(AWSSDK CONFIG COMPONENTS core dynamodb kinesis s3 REQUIRED)
include_directories(${AWSSDK_INCLUDE_DIRS})
add_executable(untitled1 main.cpp)
target_link_libraries(untitled1 PRIVATE GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main ${AWSSDK_LIBRARIES})
