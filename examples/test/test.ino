#include <Arduino.h>
#include <SPI.h>
#include <serialram.h>

// config
const uint16_t bufSize = 4096;
const uint8_t IORepeats = 128;
const bool largeAddressing = true; // true for chips > 512 kbit
const uint8_t chipSelect = 9;
const CSerialRam::ESPISpeed SPISpeed = CSerialRam::SPEED_FULL;

CSerialRam serialRam;

void setup()
{
    Serial.begin(9600);
    while (!Serial)
        ;

    delay(3000);
    serialRam.begin(largeAddressing, chipSelect, SPISpeed);
    Serial.println("Initialized");
}

void loop()
{
    char data[bufSize];

    for (int i=0; i<bufSize; ++i)
        data[i] = i;

    Serial.print("Writing "); Serial.print(bufSize * IORepeats); Serial.println(" bytes...");
    uint32_t curtime = millis();
    for (int i=0; i<IORepeats; ++i)
        serialRam.write(data, 0, bufSize);
    uint32_t spendtime = millis() - curtime;
    Serial.print("time: "); Serial.print(spendtime); Serial.print(" ms, speed: "); Serial.print(bufSize * IORepeats / spendtime); Serial.println(" kB/s");

    Serial.print("Reading "); Serial.print(bufSize * IORepeats); Serial.println(" bytes...");
    memset(data, 0, bufSize);
    curtime = millis();
    for (int i=0; i<IORepeats; ++i)
        serialRam.read(data, 0, bufSize);
    spendtime = millis() - curtime;
    Serial.print("time: "); Serial.print(spendtime); Serial.print(" ms, speed: "); Serial.print(bufSize * IORepeats / spendtime); Serial.println(" kB/s");

    Serial.println("Verifiying read data...");
    bool ok = true;
    for (int i=0; i<bufSize; ++i)
    {
        if (data[i] != (uint8_t)i)
        {
            ok = false;
            Serial.print("Mismatch: @ "); Serial.print(i); Serial.print(" : "); Serial.println((int)data[i]);
        }
    }

    if (ok)
        Serial.println("All OK!");

    delay(10000);
}
