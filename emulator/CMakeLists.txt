cmake_minimum_required(VERSION 3.11)
add_definitions("-DEMULATOR")
add_executable(
  emulator
  ../main.c
  emulator.c
)
