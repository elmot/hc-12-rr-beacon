#include "stm8s.h"

#define LED_GPIO_PORT  (GPIOD)
#define LED_GPIO_PINS  (GPIO_PIN_0)


void Delay(uint16_t nCount) {
    /* Decrement nCount value */
    while (nCount != 0) {
        nCount--;
    }
}

void vPlf_McuInit(void) {

    /* SPI configuration */
    SPI_DeInit();
    GPIO_DeInit(GPIOD);
    GPIO_DeInit(GPIOC);
    GPIO_Init(GPIOD, GPIO_PIN_2 | GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST); //SPI Soft CS & RADIO_SDN
    GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_IN_PU_NO_IT); //SPI Soft CS & RADIO_SDN
    SPI_Init(SPI_FIRSTBIT_LSB, SPI_BAUDRATEPRESCALER_128, SPI_MODE_MASTER, SPI_CLOCKPOLARITY_LOW,
             SPI_CLOCKPHASE_1EDGE, SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, (uint8_t) 0x07);

    SPI_Cmd(ENABLE);

}

//*******************************************
//
void radio_hal_AssertShutdown(void) {
    GPIO_WriteHigh(GPIOC, GPIO_PIN_4);
}

void radio_hal_DeassertShutdown(void) {
    GPIO_WriteLow(GPIOC, GPIO_PIN_4);
}

void radio_hal_ClearNsel(void) {
    GPIO_WriteLow(GPIOD, GPIO_PIN_2);
//todo restore?    while (GPIOC->IDR & 0x80); //wait until MISO(PC7) down
    SPI->CR1 |= 0x40; /* Enable the SPI peripheral*/
}

void radio_hal_SetNsel(void) {
    while ((SPI->SR & 0x82) != 0x2);//ccSpiWait for TX high and BSY low
    SPI->CR1 &= ~0x40; /* Disable the SPI peripheral*/
    GPIO_WriteHigh(GPIOD, GPIO_PIN_2);
}

BitStatus radio_hal_NirqLevel(void) {
    return GPIO_ReadInputPin(GPIOC, GPIO_PIN_4);
}

static inline void spiWaitRxne(void) {
    while (!(SPI->SR & 0x01));//ccSpiWait for RXNE
}


static inline U8 spiWriteWrite(U8 byteToWrite) {
    SPI->DR = byteToWrite;
    spiWaitRxne();
    return SPI->DR;
}

void radio_hal_SpiWriteByte(U8 byteToWrite) {
    spiWriteWrite(byteToWrite);
}


U8 radio_hal_SpiReadByte(void)
{
    return spiWriteWrite(0xFF);
}

void radio_hal_SpiWriteData(U8 byteCount, U8 *pData) {
    for (U8 i = 0; i < byteCount; i++)
        radio_hal_SpiWriteByte(pData[i]);
}

void radio_hal_SpiReadData(U8 byteCount, U8* pData)
{
    for (U8 i = 0; i < byteCount; i++) {
        pData[i] = spiWriteWrite(0xFF);
    }
}

//BIT radio_hal_Gpio0Level(void)
//{
//  retVal = RF_GPIO0;
//  return retVal;
//}

//BIT radio_hal_Gpio1Level(void)
//{
//  BIT retVal = FALSE;
//
//  retVal = RF_GPIO1;
//
//  return retVal;
//}
//

//*******************************************
#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line) {
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1) {
    }
}

#endif
