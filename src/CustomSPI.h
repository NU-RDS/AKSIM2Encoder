#pragma once

#include <SPI.h>

class CustomSPIClass : public SPIClass
{
public:
    CustomSPIClass(uintptr_t myport, uintptr_t myhardware) : SPIClass{myport, myhardware}, port_addr(myport), hardware_addr(myhardware)
    {
    }

    // transfer 8 to 32 bits
    uint32_t transfern(uint16_t data, uint8_t n)
    {
        uint32_t tcr = port().TCR;
        port().TCR = (tcr & 0xfffff000) | LPSPI_TCR_FRAMESZ(n - 1); // turn on n bit mode
        port().TDR = data;                                          // output n bit data.
        while ((port().RSR & LPSPI_RSR_RXEMPTY))
            ;             // wait while the RSR fifo is empty...
        port().TCR = tcr; // restore back
        return port().RDR;
    }

private:
    IMXRT_LPSPI_t &port() { return *(IMXRT_LPSPI_t *)port_addr; }
    const SPI_Hardware_t &hardware() { return *(const SPI_Hardware_t *)hardware_addr; }
    uintptr_t port_addr;
    uintptr_t hardware_addr;
};

extern CustomSPIClass CustomSPI;
extern CustomSPIClass CustomSPI1;
extern CustomSPIClass CustomSPI2;