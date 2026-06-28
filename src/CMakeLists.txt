cmake_minimum_required(VERSION 3.16)

project(StudentTaskManager VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 公共业务代码库：保存任务类、任务管理和工具函数。
add_library(task_core
    Task.cpp
    TaskManager.cpp
    Utils.cpp
)
target_include_directories(task_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Win32 原生 GUI 版本入口，不依赖 Qt。
add_executable(task_manager_gui WIN32
    win_gui_main.cpp
    WinGui.h
    WinGui.cpp
    app.manifest
)
target_link_libraries(task_manager_gui PRIVATE task_core comctl32)
target_compile_definitions(task_manager_gui PRIVATE UNICODE _UNICODE)

# MinGW 默认寻找 WinMain；本项目入口是 Unicode 版本 wWinMain，
# 因此使用 MinGW 构建 GUI 时需要加 -municode。
if(MINGW)
    target_link_options(task_manager_gui PRIVATE -municode)
endif()
