cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_CXX_STANDARD 17)
project(serial_test VERSION 0.1.0 LANGUAGES C CXX)

# 设置目标文件生成位置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

# 查找 Boost 库并指定 thread 组件
find_package(Boost REQUIRED COMPONENTS thread)

# 添加 infantry 子目录
add_subdirectory(infantry)

# 添加根目录的源文件
aux_source_directory(. _SOURCES)
# 添加 infantry 子目录的所有源文件
aux_source_directory(infantry _SUB_SOURCES)

# 添加头文件包含路径
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/infantry
    ${Boost_INCLUDE_DIRS}
)

# 创建可执行文件 serial_test，并链接 Boost 线程库
add_executable(serial_test main.cpp ${_SOURCES} ${_SUB_SOURCES})
target_link_libraries(serial_test Boost::thread)
