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

# reference client implementations

there's a few reference implementations in `ref_impl`.

## `show_header`

this client reads the cartridge header and displays the contained information in a human-friendly format (including decoding known constants nintendo used).

### Building
    mkdir ref_impl/show_header/build
    cd ref_impl/show_header/build
    cmake ..
    make

### Usage
* `./show_header path`
  * `path` is the path to a character device which will be used to communicate with the hardware/emulator.
    * if you want to use it with the hardware, use `/dev/ttyUSB*` (NOTE: untested for now).
    * if you want to use it with the emulator, create a virtual device wrapping it with a command like:

      `socat -ddd -ddd pty,raw,echo=0,crnl "exec:'./emulator -rl /path/to/rom.gb',pty,raw,echo=0,crnl"`

      it will then show you a `/dev/pty/*` device to connect to.
