#ifndef SERRAM_CONFIG_H
#define SERRAM_CONFIG_H

// Use SPIFIFO on Teensy 3.X boards
#define SERIALRAM_USESPIFIFO

// If using SPIFIFO, do not reverse byte order to speed up reads.
// Enabling this might be useful if you plan to use the RAM with other
// code
// UNDONE: doesn't work with swapping
#define SERIALRAM_USENODATASWAP

#endif // SERRAM_CONFIG_H
