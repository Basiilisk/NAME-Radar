cmake_minimum_required(VERSION 3.19)
project(NAME_Radar LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Sql)

qt_standard_project_setup()

qt_add_executable(NAME_Radar
    WIN32
    app.rc
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    centralwidget.h centralwidget.cpp
    StructUSE.h
    jsonsettings.h jsonsettings.cpp
    searchnames.h searchnames.cpp
    sortfilebydate.h sortfilebydate.cpp
    heavyworkthread.h heavyworkthread.cpp
    highlightlineedit.h highlightlineedit.cpp

    Convertor/convertormanager.h
    Convertor/convertormanager.cpp
    Convertor/hashutils.h
    Convertor/hashutils.cpp
    Convertor/filescanner.h Convertor/filescanner.cpp

    Convertor/libreofficeconverter.h Convertor/libreofficeconverter.cpp

    Convertor/DadaBase/databasemanager.h Convertor/DadaBase/databasemanager.cpp

)

target_link_libraries(NAME_Radar
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Sql
)

include(GNUInstallDirs)

install(TARGETS NAME_Radar
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET NAME_Radar
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
