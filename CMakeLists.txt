cmake_minimum_required(VERSION 3.19)
project(NAME_Radar LANGUAGES CXX)

# ==============================================================================
# МАКСИМАЛЬНА ОПТИМІЗАЦІЯ РОЗМІРУ (LTO)
# ==============================================================================
# Вмикаємо міжпроцедурну оптимізацію (Link Time Optimization).
# Вона змушує компілятор викинути весь мертвий/невикористаний код з Qt.
# Збірка триватиме трохи довше, але розмір .exe буде мінімальним!
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT error)
if(ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(WARNING "IPO/LTO не підтримується: ${error}")
endif()

# Кажемо MSVC оптимізувати розмір файлу (/O1 або /O2, для релізу Qt і так ставить O2)
if(MSVC)
    add_compile_options("$<$<CONFIG:Release>:/O2>")
endif()

# ==============================================================================
# БЛОК 1: СТАТИЧНА ЗБІРКА C++ RUNTIME (Тільки для компілятора MSVC)
# Вшиваємо базові бібліотеки C++ в екзешник, щоб не вимагало VCRUNTIME140.dll
# ==============================================================================
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# ==============================================================================
# БЛОК 2: ПІДКЛЮЧЕННЯ QT ТА ВНУТРІШНІХ ПАПОК
# ==============================================================================
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Sql)
qt_standard_project_setup()

# Кажемо компілятору: "Шукай заголовні файли (.h) у папці src".
# Це дозволяє писати #include "mainwindow.h" замість #include "src/mainwindow.h"
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(PROJECT_HEADERS
    src/mainwindow.h
    src/centralwidget.h
    src/StructUSE.h
    src/jsonsettings.h
    src/searchnames.h
    src/sortfilebydate.h
    src/heavyworkthread.h
    src/highlightlineedit.h
    src/convertorform.h
    src/Convertor/convertormanager.h
    src/Convertor/hashutils.h
    src/Convertor/filescanner.h
    src/Convertor/libreofficeconverter.h
    src/Convertor/DadaBase/databasemanager.h
    src/Convertor/convertorworker.h
)

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/centralwidget.cpp
    src/jsonsettings.cpp
    src/searchnames.cpp
    src/sortfilebydate.cpp
    src/heavyworkthread.cpp
    src/highlightlineedit.cpp
    src/convertorform.cpp
    src/Convertor/convertormanager.cpp
    src/Convertor/hashutils.cpp
    src/Convertor/filescanner.cpp
    src/Convertor/libreofficeconverter.cpp
    src/Convertor/DadaBase/databasemanager.cpp
    src/Convertor/convertorworker.cpp
)

set(PROJECT_FORMS
    src/mainwindow.ui
)

set(PROJECT_RESOURCES
    src/app.rc
)

# ==============================================================================
# БЛОК 4: СТВОРЕННЯ EXECUTABLE ТА ЛІНКОВКА QT
# ==============================================================================
qt_add_executable(NAME_Radar WIN32
    ${PROJECT_HEADERS}
    ${PROJECT_SOURCES}
    ${PROJECT_FORMS}
    ${PROJECT_RESOURCES}
)

target_link_libraries(NAME_Radar
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Sql
)

# ==============================================================================
# БЛОК 5: РОБОТА З THIRDPARTY (АВТОМАТИЧНЕ КОПІЮВАННЯ)
# ==============================================================================
# Вказуємо шлях до всієї папки thirdparty
set(THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")

# Тепер ми копіюємо не вміст LibreOffice, а повністю всю папку thirdparty.
# Після збірки поруч з NAME_Radar.exe з'явиться папка "thirdparty".
add_custom_command(TARGET NAME_Radar POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${THIRDPARTY_DIR}"
    "$<TARGET_FILE_DIR:NAME_Radar>/thirdparty"
    COMMENT "Копіювання папки thirdparty поруч з exe..."
)

# ==============================================================================
# БЛОК 6: НАЛАШТУВАННЯ ДЕПЛОЮ (СТВОРЕННЯ ПАПКИ Deploy_Release В КОРЕНІ ПРОЄКТУ)
# ==============================================================================
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    # Змінили CMAKE_BINARY_DIR на CMAKE_SOURCE_DIR
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/Deploy_Release" CACHE PATH "Папка для фінального релізу" FORCE)
endif()

# Інсталюємо екзешник
install(TARGETS NAME_Radar RUNTIME DESTINATION .)

# Кажемо CMake: "Візьми папку thirdparty і поклади її в корінь deploy_release"
# (Увага: відсутність слеша після THIRDPARTY_DIR важлива, щоб скопіювалась сама папка, а не лише її вміст)
install(DIRECTORY "${THIRDPARTY_DIR}" DESTINATION .)
