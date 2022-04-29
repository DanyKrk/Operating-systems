cmake_minimum_required(VERSION 3.22)
project(cw06 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(zad1)
include_directories(zad2)

add_executable(cw06
        zad1/client.c
        zad1/server.c zad1/constants.h)
