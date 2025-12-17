cmake_minimum_required(VERSION 3.19)
project(color_client LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(${PROJECT_NAME}
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)


set(OPENCV_INCLUDE "dep/include")
set(OPENCV_LIBRARY "dep/lib")
include_directories(${OPENCV_INCLUDE})
target_link_directories(${PROJECT_NAME} PRIVATE ${OPENCV_LIBRARY})
message(${OPENCV_LIBRARY})

find_package(Qt6 REQUIRED COMPONENTS Core5Compat)



target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Core5Compat
)


# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#     target_link_libraries(${PROJECT_NAME} PRIVATE opencv_world4120d)
# elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
target_link_libraries(${PROJECT_NAME} PRIVATE opencv_world480 opencv_img_hash480)
# endif()
file(GLOB DEP_BIN_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/dep/bin/*.dll"
)
message(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY ${DEP_BIN_FILES} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
qt_generate_deploy_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
