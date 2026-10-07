cmake_minimum_required(VERSION 3.20)
project(Emuflix LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)
find_package(SDL2 REQUIRED)

add_executable(Emuflix
    src/main.cpp
    src/resources.qrc
    
    src/app/EmuflixApp.cpp

    src/ui/MainWindow.cpp
    src/ui/UIController.cpp
    src/ui/SettingsWindow.cpp
    src/ui/AddFolderDialog.cpp
    src/ui/SetupWindow.cpp
    src/ui/GameCardWidget.cpp
    src/ui/ControllerService.cpp

    src/library/LibraryService.cpp
    src/library/GameLauncher.cpp
    src/library/EmulatorManager.cpp
    src/library/EmulatorInstaller.cpp
    
    src/settings/SettingsManager.cpp

    src/data/DatabasePath.cpp
    src/data/SQLiteDatabase.cpp
    src/data/SQLiteLibrary.cpp
    src/data/SQLiteSettings.cpp
)

target_include_directories(Emuflix PRIVATE src)
target_link_libraries(Emuflix PRIVATE Qt6::Widgets Qt6::Sql SDL2::SDL2)

add_custom_command(TARGET Emuflix POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/src/data/schema.sql
            $<TARGET_FILE_DIR:Emuflix>/schema.sql
)