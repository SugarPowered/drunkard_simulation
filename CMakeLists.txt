cmake_minimum_required(VERSION 3.22)
project(
POS-Semestralka-Random-Walk
VERSION 0.0.1
DESCRIPTION "POS Semestral project focused on simulation of random walk, with sockets"
LANGUAGES C 
)

add_subdirectory(client)
add_subdirectory(server)
add_subdirectory(sockets-lib)
