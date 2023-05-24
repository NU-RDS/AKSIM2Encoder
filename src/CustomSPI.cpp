#include "CustomSPI.h"
// Teensy SPI info
// port, hardware
CustomSPIClass CustomSPI((uintptr_t)&IMXRT_LPSPI4_S, (uintptr_t)&SPIClass::spiclass_lpspi4_hardware);
CustomSPIClass CustomSPI1((uintptr_t)&IMXRT_LPSPI3_S, (uintptr_t)&SPIClass::spiclass_lpspi3_hardware);
CustomSPIClass CustomSPI2((uintptr_t)&IMXRT_LPSPI1_S, (uintptr_t)&SPIClass::spiclass_lpspi1_hardware);