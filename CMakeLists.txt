cmake_minimum_required(VERSION 3.25)

project(color_client LANGUAGES CXX)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

# 编译器设置
if(MSVC)
    add_compile_options("/Zc:__cplusplus")
    add_compile_options("/permissive-")
    add_compile_options("/W4")
    add_compile_options("/MP")
endif()

# 禁用自动处理
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)

# 设置 Qt 路径
set(QT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/dep/qt)
set(QT_INCLUDE_DIR ${QT_ROOT}/include)
set(QT_LIB_DIR ${QT_ROOT}/lib)
set(QT_BIN_DIR ${QT_ROOT}/bin)
set(QT_OTHERS_DIR ${QT_ROOT}/others)

# 设置 OpenCV 路径
set(OpenCV_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/dep/opencv)
set(OpenCV_INCLUDE_DIR ${OpenCV_ROOT}/include)
set(OpenCV_LIB_DIR ${OpenCV_ROOT}/lib)
set(OpenCV_BIN_DIR ${OpenCV_ROOT}/bin)

# 查找文件
file(GLOB UI_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/*.ui")
file(GLOB MOC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/*.cpp")

# message(STATUS "Found UI files: ${UI_FILES}")
# message(STATUS "Found MOC files: ${MOC_FILES}")
# message(STATUS "Found SRC files: ${SRC_FILES}")

# 手动设置包含目录
include_directories(
    # QT 相关头文件
    ${QT_INCLUDE_DIR}
    ${QT_INCLUDE_DIR}/QtCore
    ${QT_INCLUDE_DIR}/QtWidgets
    ${QT_INCLUDE_DIR}/QtGui
    # 编译后的 ui_*.h 文件会放到输出目录
    ${CMAKE_CURRENT_BINARY_DIR}
    # 需要编译的 qt gui头文件
    ${CMAKE_CURRENT_SOURCE_DIR}/include  # 添加 moc 目录
    # OpenCV 相关头文件
    ${OpenCV_INCLUDE_DIR}
)

# 添加lib目录
link_directories(
    ${QT_LIB_DIR}
    ${OpenCV_LIB_DIR}
)

set(GENERATED_UI_FILES "")
set(GENERATED_MOC_FILES "")

# 检查并设置 UI 工具路径
if(WIN32 AND EXISTS "${QT_BIN_DIR}/uic.exe" AND EXISTS "${QT_BIN_DIR}/moc.exe")
    set(UIC_EXECUTABLE "${QT_BIN_DIR}/uic.exe")
    set(MOC_EXECUTABLE "${QT_BIN_DIR}/moc.exe")
    
    # 处理 UI 文件
    foreach(ui_file ${UI_FILES})
        get_filename_component(file_name ${ui_file} NAME_WE)  # 使用 NAME_WE 获取不带扩展名的文件名
        set(ui_output_file "${CMAKE_CURRENT_BINARY_DIR}/ui_${file_name}.h")
        add_custom_command(
            OUTPUT ${ui_output_file}
            COMMAND ${UIC_EXECUTABLE} ${ui_file} -o ${ui_output_file}
            DEPENDS ${ui_file}
            COMMENT "Generating UI header from ${file_name}.ui"
        )
        message(STATUS "UI: ${ui_file} -> ${ui_output_file}")
        list(APPEND GENERATED_UI_FILES ${ui_output_file})
    endforeach()
    
    # 处理 MOC 文件
    foreach(moc_file ${MOC_FILES})
        get_filename_component(file_name ${moc_file} NAME_WE)  # 使用 NAME_WE 获取不带扩展名的文件名
        set(moc_output_file "${CMAKE_CURRENT_BINARY_DIR}/moc_${file_name}.cpp")
        add_custom_command(
            OUTPUT ${moc_output_file}
            COMMAND ${MOC_EXECUTABLE} ${moc_file} -o ${moc_output_file}
            DEPENDS ${moc_file}
            COMMENT "Generating MOC source from ${file_name}.h"
        )
        message(STATUS "MOC: ${moc_file} -> ${moc_output_file}")
        list(APPEND GENERATED_MOC_FILES ${moc_output_file})
    endforeach()
    
    # 合并所有生成的文件
    set(GENERATED_FILES ${GENERATED_UI_FILES} ${GENERATED_MOC_FILES})
    
else()
    message(WARNING "Qt tools not found. You need to provide generated files manually")
    set(GENERATED_FILES "")
endif()

# 添加可执行文件
add_executable(${PROJECT_NAME}
    main.cpp
    ${SRC_FILES}
    ${GENERATED_FILES}  # 只添加生成的文件，不添加 UI 和 MOC 源文件
)

# 链接 Qt 库
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6Core
    Qt6Widgets
    Qt6Gui
    opencv_world480
)

# 复制 DLL 文件（Windows）
if(WIN32)
    message(STATUS "Copying Qt DLLs from ${QT_BIN_DIR} to ${CMAKE_BUILD_TYPE}")
    # 安装 Qt DLLs
    file(GLOB QT_DLLS "${QT_BIN_DIR}/*.dll")
    install(FILES ${QT_DLLS}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>
    )
    # 安装其他QT 环境依赖
    file(GLOB QT_OTHERS "${QT_OTHERS_DIR}/*")
    install(DIRECTORY ${QT_OTHERS}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>
    )
    
    # 安装 OpenCV DLLs
    file(GLOB OpenCV_DLLS "${OpenCV_BIN_DIR}/*.dll")
    install(FILES ${OpenCV_DLLS}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>
    )
endif()

