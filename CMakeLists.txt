cmake_minimum_required(VERSION 3.14)
project(calculator C)

set(CMAKE_C_STANDARD 11)

include_directories(include)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GNOME IMPORTED_TARGET libgnome-2.0 libgnomeui-2.0)
if (GNOME_FOUND)
    add_executable(client
        src/client/callbacks.c
        src/client/interface.c
        src/client/main.c
        src/client/main.h
        src/client/support.c
        src/common/common.c
        src/common/common.h)
    target_link_libraries(client PRIVATE PkgConfig::GNOME ncurses)

    add_executable(server
        src/server/main.c
        src/server/main.h
        src/server/support.c
        src/server/user_handle.c
        src/server/gif_handle.c
        src/common/common.c
        src/common/common.h)
    target_link_libraries(server PRIVATE PkgConfig::GNOME ncurses)
endif ()

