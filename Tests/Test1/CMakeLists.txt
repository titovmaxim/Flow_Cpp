cmake_minimum_required(VERSION 3.7)
set(PROJECT_NAME "Test1")

set(CMAKE_CXX_STANDARD 14)
project(${PROJECT_NAME} LANGUAGES CXX)
file(GLOB_RECURSE SRC CONFIGURE_DEPENDS "*.h" "*.cpp" "*.c")

# Add QT support
include(../../Qt.cmake)
file(GLOB_RECURSE SRC CONFIGURE_DEPENDS
        "*.h" "*.cpp" "*.c" "*.ui" "*.qml" "*.qrc" "*.conf"
        "../../Libraries/Unicore.Library/*.h")

add_executable(${PROJECT_NAME} ${SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

# Linux add GLIBC
if (UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE rt)
endif ()

# QT components, etc.
find_package(Qt5 COMPONENTS Core Quick REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Quick)

# QML Debug
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

# QT deploy (add libraries, plugins, etc.)
qt_deploy(${PROJECT_NAME})
