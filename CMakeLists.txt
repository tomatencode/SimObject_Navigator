cmake_minimum_required(VERSION 3.5)

project(SimObject_Navigator VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)#
#
include(FetchContent) # once in the project to include the module
#
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
)
FetchContent_MakeAvailable(nlohmann_json)
#
if(DEFINED BUILD_WITH_EXPERIMENTAL_WEB_API)
    #
    if(EMSCRIPTEN)

    else()
        FetchContent_Declare(
        asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        # GIT_TAG        asio-1-18-2
        )
        #
        FetchContent_MakeAvailable(asio)
    endif()
endif()
#
# For Windows MSVC, remove on Linux
#
set(CMAKE_PREFIX_PATH "C:/Dev/Qt/6.5.1/msvc2019_64")

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        #Flow_sheet_graph/Flow_sheet_graph.cpp
        Unit_Tab/Unit_Tab.cpp
        Generic_cli_tool/Generic_cli_tool_factory.cpp
        Generic_cli_tool/Generic_cli_tool.cpp
        Connector_value_table_model.cpp
        utilities.cpp
)
#
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SimObject_Navigator
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET SimObject_Navigator APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(SimObject_Navigator SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(SimObject_Navigator
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(SimObject_Navigator PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(SimObject_Navigator PRIVATE nlohmann_json)
# target_compile_definitions(SimObject_Navigator PRIVATE CONFIG+=debug)

if(EMSCRIPTEN)
    set(compile_options "SHELL: -g -gsource-map")
    target_link_options(SimObject_Navigator PRIVATE ${compile_options})
    target_compile_options(SimObject_Navigator PRIVATE ${compile_options})
endif()

if(DEFINED BUILD_WITH_EXPERIMENTAL_WEB_API)
    if(${BUILD_WITH_EXPERIMENTAL_WEB_API})
        #target_sources(SimObject_Navigator PRIVATE Eval_functional_model_net.cpp)
        #target_compile_definitions(SimObject_Navigator PRIVATE BUILD_WITH_EXPERIMENTAL_WEB_API)
        # target_link_libraries(SimObject_Navigator PRIVATE asio::asio) # doesnt work
        if(EMSCRIPTEN)
            #add_link_options("-s EXPORTED_RUNTIME_METHODS='[\"specialHTMLTargets\"]'")
            # set_target_properties(SimObject_Navigator PROPERTIES LINK_FLAGS "-s EXPORTED_RUNTIME_METHODS=['specialHTMLTargets']")
        else()
            message(STATUS "Asio Dir: ${asio_SOURCE_DIR}")
            target_include_directories(SimObject_Navigator PRIVATE ${asio_SOURCE_DIR}/asio/include)
        endif()
    endif()
endif()

set_target_properties(SimObject_Navigator PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS SimObject_Navigator
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SimObject_Navigator)
endif()
