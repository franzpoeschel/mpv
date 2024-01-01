cmake_minimum_required(VERSION 3.15.0)

project(gnome-session-inhibitor)

add_executable(gnome-session-inhibitor main.c)
install(TARGETS gnome-session-inhibitor)