#include "stm8s.h"
#include "main.h"

#define U8 uint8_t

//todo IWDG setup
//todo led blink
//todo power save

static inline void initHW() {

    CLK->PCKENR1 = 2/*SPI*/ | 0x20 /*TIM2*/;
    CLK->PCKENR2 = 0;
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV8);
    CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);

    GPIO_DeInit(GPIOD);
    GPIO_DeInit(GPIOC);
    GPIO_Init(GPIOD, (GPIO_Pin_TypeDef) (GPIO_PIN_2 | GPIO_PIN_5), GPIO_MODE_OUT_PP_HIGH_FAST); //SPI Soft CS, LED
    GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST); //RADIO_SDN
    GPIO_Init(GPIOD, (GPIO_Pin_TypeDef) (GPIO_PIN_0 | GPIO_PIN_1| GPIO_PIN_3| GPIO_PIN_6| GPIO_PIN_7), GPIO_MODE_IN_PU_NO_IT); //Power save

    GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_IN_FL_NO_IT); //nIRQ
    GPIO_Init(GPIOC, (GPIO_Pin_TypeDef) (GPIO_PIN_6 | GPIO_PIN_5), GPIO_MODE_OUT_PP_LOW_FAST); //SPI MOSI, CLS
    GPIO_Init(GPIOC, (GPIO_Pin_TypeDef) (GPIO_PIN_0 | GPIO_PIN_1| GPIO_PIN_2| GPIO_PIN_3| GPIO_PIN_7), GPIO_MODE_IN_PU_NO_IT); //Power save
    GPIO_Init(GPIOA, GPIO_PIN_ALL, GPIO_MODE_IN_PU_NO_IT); //Power save
    GPIO_Init(GPIOB, GPIO_PIN_ALL, GPIO_MODE_IN_PU_NO_IT); //Power save

    SPI_DeInit();
    SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_2, SPI_MODE_MASTER, SPI_CLOCKPOLARITY_LOW,
             SPI_CLOCKPHASE_1EDGE, SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, (uint8_t) 0x07);

    SPI_Cmd(ENABLE);

    /* Power save */

    TIM2_DeInit();
    TIM2_ARRPreloadConfig(DISABLE);
    TIM2_TimeBaseInit(TIM2_PRESCALER_16384, 1500);
    TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
}

#define RF_TX_POWER_LEN 8

static uint8_t power_command[RF_TX_POWER_LEN]  = {0x11, 0x22, 0x04, 0x00, 0x08, 0xFF, 0x00, 0x5D};

//const DEF_TX_POWER POWER_STEP_0 = RF_TX_POWER(20, 127);
//const DEF_TX_POWER POWER_STEP_1 = RF_TX_POWER(50);
//const DEF_TX_POWER POWER_STEP_2 = RF_TX_POWER(20);
//const DEF_TX_POWER POWER_STEP_3 = RF_TX_POWER(8);
//const DEF_TX_POWER POWER_STEP_4 = RF_TX_POWER(0x0);

typedef struct {
    uint8_t _split[4];
    uint8_t stationNum;
    uint8_t txPwr;
    uint8_t txPwrInv;
    uint8_t patternA[13];
    uint8_t patternB[16];
} PACKET;

static PACKET packet = {
    ._split     = "\xD3\x91\xD3\x91",
    .stationNum = 0x10,
    .txPwr      = 0x11,
    .txPwrInv   = 0xEE,
    .patternA   = "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA",
    .patternB   ="\xDB\x6C\xDB\x6C\xDB\x6C\xDB\x6C\xDB\x6C\xDB\x6C\xDB\x6C\xDB\x6C",
};

inline static void sendSinglePacket(int8_t pwrDb, uint8_t si4463_pwr_lvl) {
    packet.txPwrInv = ~(packet.txPwr = pwrDb);
    power_command[5] = si4463_pwr_lvl;
    radio_comm_SendCmd(RF_TX_POWER_LEN, power_command);
    vRadio_StartTx(0, (uint8_t *) &packet);
}

int main(void) {
    initHW();
    vRadio_Init();
    while (1) {
        sendSinglePacket(20,127);
        delay(50);
        sendSinglePacket(10,22);
        delay(50);
        sendSinglePacket(0,7);
        delay(50);
        sendSinglePacket(-10,3);
        delay(50);
        sendSinglePacket(-20,1);
        delay(1800);
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
    (void)line;
    (void)file;
    /* Infinite loop */
    while (1) {
    }
}

#endif
