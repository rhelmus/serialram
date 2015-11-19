# serialram
Arduino library for Microchips' 23LCXXXX SPI ram chips

## Features:
- Simple and fast interface
- Support for chips with small addressing (i.e. < 1 Mbit) and large addressing (i.e. >= 1 Mbit)
- (Optional) use of SPIFIFO on Teensy 3.X boards to greatly speed up data transfers
- Uses SPI transactions (see https://www.arduino.cc/en/Tutorial/SPITransaction)
- Configurable SPI speed and CS pin

## Usage
For usage of the library it is best to see the examples. The sketch `simple`
gives a brief overview on how to use the library. The `test` example
continuously writes and reads a large block of data and verifies if everything
went OK.

The library can be configured by changing settings in `config.h`.

