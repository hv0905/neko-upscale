cmake_minimum_required(VERSION 3.19)
project(neko-upscale LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

qt_standard_project_setup()

qt_add_executable(neko-upscale
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

target_link_libraries(neko-upscale PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui)

# Install the executable
install(TARGETS neko-upscale DESTINATION bin)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")

    install(FILES ${CMAKE_SOURCE_DIR}/platforms/linux/neko-upscale.desktop DESTINATION share/kio/servicemenus)

endif ()


# include(GNUInstallDirs)

# install(TARGETS neko-upscale
#     BUNDLE  DESTINATION .
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
# )

# qt_generate_deploy_app_script(
#     TARGET neko-upscale
#     OUTPUT_SCRIPT deploy_script
#     NO_UNSUPPORTED_PLATFORM_ERROR
# )
# install(SCRIPT ${deploy_script})
