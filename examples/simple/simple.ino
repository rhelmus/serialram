#include <Arduino.h>
#include <SPI.h>
#include <serialram.h>

// config
const uint16_t bufSize = 128;
const bool largeAddressing = true; // true for chips > 512 kbit
const uint8_t chipSelect = 9;

// Speed options: CSerialRam::SPEED_FULL, CSerialRam::SPEED_HALVE and CSerialRam::SPEED_QUARTER
const CSerialRam::ESPISpeed SPISpeed = CSerialRam::SPEED_FULL;

CSerialRam serialRam;

void setup()
{
    Serial.begin(9600);
    while (!Serial)
        ;

    delay(3000); // Wait for things to settle

    // Initialize serial ram interface
    serialRam.begin(largeAddressing, chipSelect, SPISpeed);
}

void loop()
{
    char data[bufSize];

    // Fill buffer with some data
    for (int i=0; i<bufSize; ++i)
        data[i] = i;

    // Write complete buffer to RAM with start offset zero
    serialRam.write(data, 0, bufSize);

    // Read data in RAM (starting at offset 0) back to buffer
    serialRam.read(data, 0, bufSize);

    delay(1000);
}
