# Building

    mkdir build
    cd build
    cmake ..
    make

# Flashing

    make set_fuses
    make upload_firmware

you might have to edit the `CMakeLists.txt` file to accomodate for your programmer/port/etc.

# Protocol

* serial (via ftdi, so `/dev/ttyUSB*` or the likes) 9600baud 8n1
* `\r\n` ending, human readable commands
* commands:
  * `C [wrcRC]` sets "command pins" depending on the argument characters present: (`~` means inverted signal; this is known to the firmware, so set to pull the pin low, therefore enabling the function)
    * `w`: ~WE
    * `r`: ~RE
    * `c`: ~CS
    * `R`: ~RST
    * `C`: CLK
  * `A address` sets the `address` (as 16bit hex)
  * `R` reads a byte and outputs it as hex
  * `W byte` sets the `byte` (as hex)
* responses:
  * to `R` command: the hex value
  * to malformed commands: `ERR`
  * everything else works silently but blocks the input while working