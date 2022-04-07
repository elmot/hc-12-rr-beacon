#include "stm8s.h"
#include "radio_hal.h"

//#define MOSI_GPIO_Port GPIOC
//#define MOSI_Pin GPIO_PIN_6
//
//#define MISO_GPIO_Port GPIOC
//#define MISO_Pin GPIO_PIN_7
//
//#define SCK_GPIO_Port GPIOC
//#define SCK_Pin GPIO_PIN_5

#define SDN_GPIO_Port GPIOD
#define SDN_Pin GPIO_PIN_4

void static inline shortDelay() {
    asm ("nop");
}

bool radio_hal_NirqLevel(void) {
    return (bool) GPIO_ReadInputPin(GPIOC, GPIO_PIN_4);
}

void radio_hal_ClearNsel(void) {
//    SPI->CR1 |= 0x40; /* Enable the SPI peripheral*/
    GPIO_WriteLow(GPIOD, GPIO_PIN_2);
}

void radio_hal_SetNsel(void) {

//    while ((SPI->SR & (SPI_SR_BSY | SPI_SR_TXE | SPI_SR_RXNE)) != SPI_SR_TXE ) nop();//ccSpiWait for TXE high and BSY low
    GPIO_WriteHigh(GPIOD, GPIO_PIN_2);
//    SPI->CR1 &= ~0x40; /* Disable the SPI peripheral*/
}


void radio_hal_SpiWriteData(uint8_t byteCount, const uint8_t *pData) {
    for (int i = 0; i < byteCount; i++) {
        while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET) {}
        SPI_SendData(SPI->DR = pData[i]);

        while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET) {}
        SPI_ReceiveData();
    }
}

void radio_hal_SpiReadData(uint8_t byteCount, uint8_t *pData) {
    for (int i = 0; i < byteCount; i++) {
        while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET) {}
        SPI_SendData(0xFF);

        while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET) {}
        pData[i] = SPI_ReceiveData();
    }
}

void delay(uint16_t msec) {
    for (long i = 0; i < msec * 10000; i++) {
        shortDelay();
    }
}

void radio_hal_AssertShutdown(void) {
    GPIO_WriteHigh(SDN_GPIO_Port, SDN_Pin);
}

void radio_hal_DeassertShutdown(void) {
    GPIO_WriteLow(SDN_GPIO_Port, SDN_Pin);
}
