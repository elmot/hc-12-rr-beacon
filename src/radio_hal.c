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

    while(SPI_GetFlagStatus(SPI_FLAG_BSY) == SET) {}
    GPIO_WriteHigh(GPIOD, GPIO_PIN_2);
    shortDelay();
    shortDelay();
    shortDelay();
    shortDelay();
    shortDelay();
    shortDelay();
    shortDelay();
    shortDelay();
    shortDelay();
    shortDelay();
    shortDelay();
    shortDelay();
    shortDelay();
    shortDelay();
    shortDelay();
    shortDelay();
    shortDelay();
    shortDelay();
    shortDelay();
    shortDelay();
}

static inline uint8_t SpiSendReceive(uint8_t data) {
    while ((SPI->SR & (uint8_t)SPI_FLAG_TXE) == 0) {}
    SPI->DR = data;

    while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET) {}
    SPI_ReceiveData();
}

void radio_hal_SpiWriteData(uint8_t byteCount, const uint8_t *pData) {
    for (int i = 0; i < byteCount; i++) {
        SpiSendReceive(pData[i]);
    }
}

void radio_hal_SpiReadData(uint8_t byteCount, uint8_t *pData) {
    for (int i = 0; i < byteCount; i++) {
        pData[i] = SpiSendReceive(0xFF);
    }
}

void radio_hal_SpiWriteByte(uint8_t byteToWrite) {
    SpiSendReceive(byteToWrite);
}

uint8_t radio_hal_SpiReadByte(void) {
    return SpiSendReceive(0xFF);
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
