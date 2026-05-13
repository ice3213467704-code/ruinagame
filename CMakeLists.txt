cmake_minimum_required(VERSION 3.19)

project(towergame LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 这里加入了 Multimedia 模块
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Multimedia LinguistTools)

qt_standard_project_setup()

qt_add_executable(towergame
    main.cpp
    widget.cpp
    widget.h
)

# 这里链接了多媒体库
target_link_libraries(towergame PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Multimedia
)

qt_add_translations(towergame
    TARGET towergame
    TS_FILES towergame_zh_CN.ts
)

install(TARGETS towergame
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(towergame)