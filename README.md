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

# Emulator
there's an emulator feature that replaces the usb serial connection with `stdio` and the cartridge by an emulated bus (connected to a `0xffff` bytes ram buffer) while linking to the actual protocol part of the firmware.

you can find it in the `emulator` directory.

## Building

    mkdir emulator/build
    cd emulator/build
    cmake ..
    make

## Usage

* `./emulator [-r] [-l path]`
  * `-r` randomizes the bus state on startup (to simulate a random electronic state in the chips on power-on)
  * `-l path` loads up to `0xffff` bytes from `path` into the internal buffer.
