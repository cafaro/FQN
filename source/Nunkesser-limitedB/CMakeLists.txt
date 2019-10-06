cmake_minimum_required(VERSION 3.14)
project(nunkesser_limitedB C)

set(CMAKE_C_STANDARD 99)

add_executable(nunkesser_limitedB main.c qn.c bostree.c qnonline.c)
