cmake_minimum_required(VERSION 3.14)
project(calculator C)

set(CMAKE_C_STANDARD 11)

include_directories(include)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0)
if (GTK3_FOUND)
    add_executable(client
        src/client/callbacks.c
        src/client/interface.c
        src/client/main.c
        src/client/main.h
        src/client/support.c
        src/common/common.c
        src/common/common.h)
    target_link_libraries(client PRIVATE PkgConfig::GTK3 ncurses)

    add_executable(server
        src/server/main.c
        src/server/main.h
        src/server/support.c
        src/server/user_handle.c
        src/server/gif_handle.c
        src/common/common.c
        src/common/common.h)
    target_link_libraries(server PRIVATE PkgConfig::GTK3 ncurses)
endif ()

