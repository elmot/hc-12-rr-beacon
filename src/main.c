#include "stm8s.h"

#define U8 uint8_t

void vRadio_Init(void);

void vRadio_StartTx(uint8_t channel, const uint8_t *pioFixRadioPacket);
//todo packet sequence
//todo packet format
//todo IWDG setup
//todo led blink
//todo sequence
//todo power save
//todo hw cts



void initHW() {
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV8);
    CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);

    GPIO_DeInit(GPIOD);
    GPIO_DeInit(GPIOC);
    GPIO_Init(GPIOD, GPIO_PIN_2, GPIO_MODE_OUT_PP_HIGH_FAST); //SPI Soft CS
    GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST); //RADIO_SDN
    GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_IN_PU_NO_IT); //nIRQ

    GPIO_Init(GPIOC, GPIO_PIN_6 | GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST); //SPI MOSI, CLS

    SPI_DeInit();
    SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_2, SPI_MODE_MASTER, SPI_CLOCKPOLARITY_LOW,
             SPI_CLOCKPHASE_1EDGE, SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, (uint8_t) 0x07);

    SPI_Cmd(ENABLE);

    TIM2_DeInit();
    TIM2_ARRPreloadConfig(DISABLE);
    TIM2_TimeBaseInit(TIM2_PRESCALER_16384, 1500);
    TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
}

void delay(uint16_t msec);

int main(void) {
    initHW();
//  TODO begin of timer test
    vRadio_Init();
    const unsigned char *PATTERN = (U8 *)
        "\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c"
        "\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c"
        "\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c"
        "\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c"
        "\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c"
        "\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c"
        "\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c"
        "\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c";
    while (1) {
        vRadio_StartTx(0, (unsigned char *) PATTERN);
        delay(1800);//todo adjust
        vRadio_StartTx(0, (unsigned char *) PATTERN);
        delay(1800);//todo adjust
    }
    return 0;
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
