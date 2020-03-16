# General QT settings
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

#QT path setup for Windows
if (CMAKE_SYSTEM_NAME STREQUAL Windows)
    set(QT_VER "5.14.1")
    set(QT_PATH "c:/Qt/Qt${QT_VER}/${QT_VER}/msvc2017_64")
    list(APPEND CMAKE_PREFIX_PATH ${QT_PATH})
endif ()

# Grouping project files
source_group("Sources" REGULAR_EXPRESSION ".*\\.cpp")
source_group("Headers" REGULAR_EXPRESSION ".*\\.h")
source_group("Qt" REGULAR_EXPRESSION ".*\\.(qml|conf|qrc)")

# Configure GUI_TYPE
set(GUI_TYPE "")
if (APPLE)
    SET(GUI_TYPE MACOSX_BUNDLE)
elseif (WIN32)
    set(GUI_TYPE WIN32)
endif ()

# QT deploy for Windows (add libraries, plugins, etc.)
function(qt_deploy PROJECT)
    if (CMAKE_SYSTEM_NAME STREQUAL Windows)
        # see: https://blog.nathanosman.com/2017/11/24/using-windeployqt-with-cpack.html
        string(TOLOWER ${CMAKE_BUILD_TYPE} QT_DEPLOY_MODE)
        add_custom_command(TARGET ${PROJECT} POST_BUILD
                COMMENT "Deploying Qt for Windows: ${QT_DEPLOY_MODE}"
                COMMAND "${CMAKE_COMMAND}" -E
                env PATH="${QT_PATH}/bin"
                "windeployqt.exe"
                --${QT_DEPLOY_MODE}
                --qmldir ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
                --no-compiler-runtime
                --verbose 0
                \"$<TARGET_FILE:${PROJECT_NAME}>\"
                )
    endif ()
endfunction()
