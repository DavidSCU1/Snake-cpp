cmake_minimum_required(VERSION 3.16)
project(TestGameDirect)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Svg)

qt6_standard_project_setup()

# 添加资源文件
qt6_add_resources(RESOURCES resources.qrc)

# 创建可执行文件
qt6_add_executable(TestGameDirect
    test_game_direct.cpp
    gamewidget.cpp
    snake.cpp
    food.cpp
    networkmanager.cpp
    multiplayergamemanager.cpp
    singleplayergamemanager.cpp
    ${RESOURCES}
)

# 链接Qt库
target_link_libraries(TestGameDirect Qt6::Core Qt6::Widgets Qt6::Svg)

# 设置输出目录
set_target_properties(TestGameDirect PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)