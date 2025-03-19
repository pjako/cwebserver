cmake_minimum_required(VERSION 3.21)
project(CWebServer)

add_executable(cwebserver webserver.c tlse_comp.c sqlite3.c)
add_executable(cwebclient webclient.c tlse_comp.c sqlite3.c)