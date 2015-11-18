# serialram
Arduino library for Microchips' 23LCXXXX SPI ram chips

Features:
- Simple and fast interface
- Support for chips with small addressing (i.e. < 1 Mbit) and large addressing (i.e. >= 1 Mbit)
- (Optional) use of SPIFIFO on Teensy 3.X boards to greatly speed up data transfers
- Uses SPI transactions (see https://www.arduino.cc/en/Tutorial/SPITransaction)
- Configurable SPI speed and CS pin

