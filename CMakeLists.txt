cmake_minimum_required(VERSION 3.21)
project(CWebServer)
set(CMAKE_C_STANDARD 11)
add_definitions(-DPROJECT_ROOT="${PROJECT_SOURCE_DIR}")

add_executable(cwebserver webserver.c tlse_comp.c sqlite3.c)
add_executable(cwebserverSimple webserver_simple.c tlse_comp.c sqlite3.c)
add_executable(cwebclient webclient.c tlse_comp.c sqlite3.c)