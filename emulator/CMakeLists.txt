add_definitions("-DEMULATOR")
add_executable(
  emulator
  ../main.c
  emulator.c
  uart.c
)
