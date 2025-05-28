cmake_minimum_required(VERSION 3.19)
project(Bank LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network)
find_package(CURL REQUIRED)  # ← добавляем поиск libcurl

qt_standard_project_setup()

qt_add_executable(Bank
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    backend.h backend.cpp
    homewindow.h homewindow.cpp homewindow.ui
    createwallet.h createwallet.cpp createwallet.ui
    sendform.h sendform.cpp sendform.ui
    history.h history.cpp history.ui


)

target_link_libraries(Bank
    PRIVATE
        Qt::Core
        Qt::Widgets
        CURL::libcurl   # ← добавляем libcurl в линковку
        Qt::Network

)

include(GNUInstallDirs)

install(TARGETS Bank
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Bank
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
