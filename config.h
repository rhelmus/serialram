#ifndef CONFIG_H
#define CONFIG_H

// Use SPIFIFO on Teensy 3.X boards
//#define SERIALRAM_USESPIFIFO

// If using SPIFIFO, do not reverse byte order to speed up reads.
// Enabling this might be useful if you plan to use the RAM with other
// code
//#define SERIALRAM_USENODATASWAP

#endif // CONFIG_H
