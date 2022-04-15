#include "stm8s.h"

#define U8 uint8_t

void vRadio_Init(void);

void vRadio_StartTx(uint8_t channel, const uint8_t *pioFixRadioPacket);
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

    GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_6 | GPIO_PIN_5), GPIO_MODE_OUT_PP_LOW_FAST); //SPI MOSI, CLS

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

void radio_comm_SendCmd(uint8_t byteCount, uint8_t const *pData);

#define RF_TX_POWER_LEN 8
#define RF_TX_POWER(x) 0x11, 0x22, 0x04, 0x00, 0x08, x, 0x00, 0x5D

const uint8_t POWER_STEP_0[] = {RF_TX_POWER(127)};
const uint8_t POWER_STEP_1[] = {RF_TX_POWER(50)};
const uint8_t POWER_STEP_2[] = {RF_TX_POWER(20)};
const uint8_t POWER_STEP_3[] = {RF_TX_POWER(8)};
const uint8_t POWER_STEP_4[] = {RF_TX_POWER(0x0)};

const unsigned char *PATTERN = (U8 *)
    "\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c"
    "\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c"
    "\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c"
    "\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c"
    "\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c"
    "\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c"
    "\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c"
    "\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c\xDB\x6c";

int main(void) {
    initHW();
    vRadio_Init();
    while (1) {
        radio_comm_SendCmd(RF_TX_POWER_LEN, POWER_STEP_0);//todo remove when IWDG works
        vRadio_StartTx(0, (unsigned char *) PATTERN);
        delay(50);
        radio_comm_SendCmd(RF_TX_POWER_LEN, POWER_STEP_1);
        vRadio_StartTx(0, (unsigned char *) PATTERN);
        delay(50);
        radio_comm_SendCmd(RF_TX_POWER_LEN, POWER_STEP_2);
        vRadio_StartTx(0, (unsigned char *) PATTERN);
        delay(50);
        radio_comm_SendCmd(RF_TX_POWER_LEN, POWER_STEP_3);
        vRadio_StartTx(0, (unsigned char *) PATTERN);
        delay(50);
        radio_comm_SendCmd(RF_TX_POWER_LEN, POWER_STEP_4);
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
