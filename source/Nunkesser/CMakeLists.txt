cmake_minimum_required(VERSION 3.14)
project(nunkesser C)

set(CMAKE_C_STANDARD 99)

add_executable(nunkesser main.c qn.c bostree.c qnonline.c)
