#include "stm8s.h"
#include "radio_hal.h"

bool radio_hal_NirqLevel(void) {
    return (bool)GPIO_ReadInputPin(GPIOC, GPIO_PIN_4);
}

void radio_hal_ClearNsel(void) {
//    SPI->CR1 |= 0x40; /* Enable the SPI peripheral*/
    GPIO_WriteLow(GPIOD, GPIO_PIN_2);
}

void radio_hal_SetNsel(void) {
  
    while ((SPI->SR & (SPI_SR_BSY | SPI_SR_TXE | SPI_SR_RXNE)) != SPI_SR_TXE ) nop();//ccSpiWait for TXE high and BSY low
    GPIO_WriteHigh(GPIOD, GPIO_PIN_2);
//    SPI->CR1 &= ~0x40; /* Disable the SPI peripheral*/
}

static inline void spiWaitRxne(void) {
 while (!(SPI->SR & SPI_SR_RXNE));//ccSpiWait for RXNE
}


static inline U8 spiReadWrite(U8 byteToWrite) {
    while ((SPI->SR & SPI_SR_TXE) != SPI_SR_TXE) nop();
    SPI->DR = byteToWrite;
    spiWaitRxne();
    return SPI->DR;
}


void radio_hal_SpiWriteData(U8 byteCount, const U8 *pData) {
    for (U8 i = 0; i < byteCount; i++)
        spiReadWrite(pData[i]);
    
}

void radio_hal_SpiReadData(U8 byteCount, U8* pData)
{
    for (U8 i = 0; i < byteCount; i++) {
        pData[i] = spiReadWrite(0xFF);
    }
}

void delay(uint16_t msec) {
  while(msec--) {
    for(int i = 0; i < 10000; i++)
      nop();
  }
}

void radio_hal_AssertShutdown(void) {
    GPIO_WriteHigh(GPIOD, GPIO_PIN_4);
    delay(100);
}

void radio_hal_DeassertShutdown(void) {
    GPIO_WriteLow(GPIOD, GPIO_PIN_4);
    delay(100);
}
