#include "stm8s.h"

#define LED_GPIO_PORT  (GPIOD)
#define LED_GPIO_PINS  (GPIO_PIN_0)


void Delay(uint16_t nCount) {
    /* Decrement nCount value */
    while (nCount != 0) {
        nCount--;
    }
}

int main(void) {

    /* SPI configuration */
    SPI_DeInit();
    GPIO_DeInit(GPIOD);
    GPIO_Init(GPIOD,)
    SPI_Init(SPI_FIRSTBIT_LSB, SPI_BAUDRATEPRESCALER_128, SPI_MODE_MASTER, SPI_CLOCKPOLARITY_LOW,
             SPI_CLOCKPHASE_1EDGE, SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, (uint8_t) 0x07);

    SPI_Cmd(ENABLE);

    while (1) {
        /* Toggles LEDs */
        Delay(0xFFFF);
    }
}


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
