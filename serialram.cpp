#include <Arduino.h>
#include "serialram.h"

#ifdef SERIALRAM_USESPIFIFO
#include <SPIFIFO.h>

#ifdef SERIALRAM_USENODATASWAP
namespace {

inline uint16_t getWord(uint8_t h, uint8_t l) { return h << 8 | l; }
inline uint16_t swapInt16(uint16_t x) { return (x >> 8) | ((x & 0xff) << 8); }

}
#endif

#else
#include <SPI.h>
#endif

void CSerialRam::initTransfer(EInstruction instruction)
{
#ifndef SERIALRAM_USESPIFIFO
    digitalWrite(chipSelect, LOW);
#endif

    sendByteMore(instruction);
}

void CSerialRam::endTransfer()
{
#ifndef SERIALRAM_USESPIFIFO
    digitalWrite(chipSelect, HIGH);
#endif
}

uint8_t CSerialRam::sendByteMore(uint8_t byte)
{
#ifdef SERIALRAM_USESPIFIFO
    SPIFIFO.write(byte, SPI_CONTINUE);
    return SPIFIFO.read();
#else
    return SPI.transfer(byte);
#endif
}

uint8_t CSerialRam::sendByteNoMore(uint8_t byte)
{
#ifdef SERIALRAM_USESPIFIFO
    SPIFIFO.write(byte, 0);
    const uint8_t ret = SPIFIFO.read();
#else
    const uint8_t ret = SPI.transfer(byte);
#endif

    endTransfer();
    return ret;
}

void CSerialRam::sendAddress(uint32_t address)
{
    if (largeAddressing)
        sendByteMore((uint8_t)(address >> 16));
    sendByteMore((uint8_t)(address >> 8));
    sendByteMore((uint8_t)address);
}

void CSerialRam::begin(bool la, uint8_t pin, CSerialRam::ESPISpeed speed)
{
    largeAddressing = la; chipSelect = pin;

#ifdef SERIALRAM_USESPIFIFO
    if (speed == SPEED_FULL)
        SPIFIFO.begin(chipSelect, SPI_CLOCK_24MHz);
    else if (speed == SPEED_HALF)
        SPIFIFO.begin(chipSelect, SPI_CLOCK_12MHz);
    else if (speed == SPEED_QUARTER)
        SPIFIFO.begin(chipSelect, SPI_CLOCK_6MHz);
#else
    SPI.begin();
    if (speed == SPEED_FULL)
        SPI.setClockDivider(SPI_CLOCK_DIV2);
    else if (speed == SPEED_HALF)
        SPI.setClockDivider(SPI_CLOCK_DIV4);
    else if (speed == SPEED_QUARTER)
        SPI.setClockDivider(SPI_CLOCK_DIV8);

    pinMode(chipSelect, OUTPUT);
    digitalWrite(chipSelect, HIGH);
#endif
    // default to sequential mode
    initTransfer(INSTR_WRMR);
    sendByteNoMore(SEQUENTIAL_MODE);
}

void CSerialRam::read(char *buffer, uint32_t address, uint32_t size)
{
    initTransfer(INSTR_READ);
    sendAddress(address);

#ifdef SERIALRAM_USESPIFIFO
    if (size & 1)
    {
        if (size == 1)
        {
            *buffer = sendByteNoMore(0xFF);
            return;
        }
        else
            *buffer++ = sendByteMore(0xFF);
        --size;
    }

    const bool usefifo = size > (SPIFIFO_SIZE*2);
    if (usefifo)
    {
        // fill FIFO
        for (uint8_t i=0; i<(SPIFIFO_SIZE-1); ++i)
            SPIFIFO.write16(0xFFFF, SPI_CONTINUE);
        size -= ((SPIFIFO_SIZE-1)*2);
    }

#ifndef SERIALRAM_USENODATASWAP
    uint16_t w;
#endif

    for (; size != 2; size-=2, buffer+=2)
    {
        SPIFIFO.write16(0xFFFF, SPI_CONTINUE);
#ifdef SERIALRAM_USENODATASWAP
        *(uint16_t *)buffer = swapInt16(SPIFIFO.read());
#else
        *(uint16_t *)buffer = SPIFIFO.read();
#endif
    }

    SPIFIFO.write16(0xFFFF, 0);
#ifdef SERIALRAM_USENODATASWAP
    *(uint16_t *)buffer = swapInt16(SPIFIFO.read());
#else
    *(uint16_t *)buffer = SPIFIFO.read();
#endif
    buffer += 2;

    if (usefifo)
    {
        // purge FIFO
#ifdef SERIALRAM_USENODATASWAP
        for (uint8_t i=0; i<(SPIFIFO_SIZE-1); ++i, buffer+=2)
            *(uint16_t *)buffer = swapInt16(SPIFIFO.read());
#else
        for (uint8_t i=0; i<(SPIFIFO_SIZE-1); ++i, buffer+=2)
            *(uint16_t *)buffer = SPIFIFO.read();
#endif
    }

#else
    for (; size != 1; --size, ++buffer)
        *buffer = sendByteMore(0xFF);

    *buffer++ = sendByteNoMore(0xFF);
#endif
}

void CSerialRam::write(const char *buffer, uint32_t address, uint32_t size)
{
    initTransfer(INSTR_WRITE);
    sendAddress(address);

#ifdef SERIALRAM_USESPIFIFO
    if (size & 1)
    {
        if (size == 1)
        {
            sendByteNoMore(*buffer++);
            return;
        }
        else
            sendByteMore(*buffer++);
        --size;
    }

    const bool usefifo = size > (SPIFIFO_SIZE*2);
    if (usefifo)
    {
        // fill FIFO
        for (uint8_t i=0; i<(SPIFIFO_SIZE-1); ++i, buffer+=2)
        {
#ifdef SERIALRAM_USENODATASWAP
            SPIFIFO.write16(getWord(buffer[0], buffer[1]), SPI_CONTINUE);
#else
            SPIFIFO.write16(*(uint16_t *)buffer, SPI_CONTINUE);
#endif
        }
        size -= ((SPIFIFO_SIZE-1)*2);
    }

    for (; size != 2; size-=2, buffer+=2)
    {
#ifdef SERIALRAM_USENODATASWAP
            SPIFIFO.write16(getWord(buffer[0], buffer[1]), SPI_CONTINUE);
#else
            SPIFIFO.write16(*(uint16_t *)buffer, SPI_CONTINUE);
#endif
        SPIFIFO.read();
    }

#ifdef SERIALRAM_USENODATASWAP
    SPIFIFO.write16(getWord(buffer[0], buffer[1]), 0);
#else
    SPIFIFO.write16(*(uint16_t *)buffer, 0);
#endif
    SPIFIFO.read();

    if (usefifo)
    {
        // purge FIFO
        for (uint8_t i=0; i<(SPIFIFO_SIZE-1); ++i)
            SPIFIFO.read();
    }

#else
    for (; size != 1; --size, ++buffer)
        sendByteMore(*buffer);

    sendByteNoMore(*buffer);
#endif
}
