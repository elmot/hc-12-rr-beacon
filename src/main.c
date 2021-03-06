#include "stm8s.h"
#include "main.h"

#define U8 uint8_t

void static initIWDG() {
    IWDG->KR = 0xCC;//start
    IWDG->KR = 0x55;//enable access
    IWDG->PR = 6;
    IWDG->RLR = 0xff;
    IWDG->KR = 0xAA;
}

static inline void initHW() {
    CLK_SlowActiveHaltWakeUpCmd(ENABLE);
    GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST); //RADIO_SDN
    initIWDG();
    /* Switch off all unnecessary peripherals in sake of power consumption */
    CLK->PCKENR1 = 2/*SPI*/ | 0x20 /*TIM2*/;
    CLK->PCKENR2 = 0;
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV8);
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);

    GPIO_Init(GPIOA, GPIO_PIN_ALL, GPIO_MODE_IN_PU_NO_IT); //Power save

    GPIO_Init(GPIOB, GPIO_PIN_ALL, GPIO_MODE_IN_PU_NO_IT); //Power save

    GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_IN_FL_NO_IT); //nIRQ
    GPIO_Init(GPIOC, (GPIO_Pin_TypeDef) (GPIO_PIN_6 | GPIO_PIN_5), GPIO_MODE_OUT_PP_LOW_FAST); //SPI MOSI, CLS
    GPIO_Init(GPIOC, (GPIO_Pin_TypeDef) (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_7),
              GPIO_MODE_IN_PU_NO_IT); //Power save

    GPIO_Init(GPIOD, (GPIO_Pin_TypeDef) (GPIO_PIN_3 | GPIO_PIN_5), GPIO_MODE_OUT_PP_HIGH_FAST); //SPI Soft CS, LED
    GPIO_Init(GPIOD, (GPIO_Pin_TypeDef) (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7),
              GPIO_MODE_IN_PU_NO_IT); //Power save
    GPIO_Init(GPIOD, GPIO_PIN_6, GPIO_MODE_OUT_PP_LOW_SLOW); //Power save

    SPI_DeInit();
    SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_2, SPI_MODE_MASTER, SPI_CLOCKPOLARITY_LOW,
             SPI_CLOCKPHASE_1EDGE, SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, (uint8_t) 0x07);

    SPI_Cmd(ENABLE);

    TIM2_DeInit();
    TIM2_ARRPreloadConfig(DISABLE);
//    MASTERCLK = 8Mhz, TIM@ after prescaler ~= 1KHz
    TIM2_TimeBaseInit(TIM2_PRESCALER_16384, 1500);
    TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
}

#define RF_TX_POWER_LEN 8

static uint8_t power_command[RF_TX_POWER_LEN] = {0x11, 0x22, 0x04, 0x00, 0x08, 0xFF, 0x00, 0x1D};

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
#ifdef STATION_NUM
    .stationNum = STATION_NUM,
#else
    .stationNum = 0x10,
#endif
    .txPwr      = 0x11,
    .txPwrInv   = 0xEE,
    .patternA   = "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA",
    .patternB   ="\xDB\x6C\xDB\x6C\xDB\x6C\xDB\x6C\xDB\x6C\xDB\x6C\xDB\x6C\xDB\x6C",
};

static void inline ledOn() {
    GPIO_WriteLow(GPIOD, GPIO_PIN_5);
}

static void inline ledOff() {
    GPIO_WriteHigh(GPIOD, GPIO_PIN_5);
}

static void inline decreasePower() {
    CLK->PCKENR1 = 0x20 /*TIM2*/;
//    MASTERCLK = 8Mhz, TIM@ after prescaler ~= 1KHz
    TIM2_TimeBaseInit(TIM2_PRESCALER_2048, 1500);
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV8);
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV128);
}

inline static void sendSinglePacket(int8_t pwrDb, uint8_t si44xx_pwr_lvl) {
    packet.txPwrInv = ~(packet.txPwr = pwrDb);
    power_command[5] = si44xx_pwr_lvl;
    radio_comm_SendCmd(RF_TX_POWER_LEN, power_command);
    vRadio_StartTx(0, (uint8_t *) &packet);
}

int main(void) {

    while (1) {
        initHW();
        vRadio_Init();
        IWDG->KR = 0xAA;
        sendSinglePacket(20, 127);
        delay(50);
        sendSinglePacket(10, 22);
        delay(50);
        sendSinglePacket(0, 7);
        delay(50);
        sendSinglePacket(-10, 3);
        delay(50);
        ledOn();
        sendSinglePacket(-20, 1);
        delay(60);
        sleepRadio();
        ledOff();
        decreasePower();
        delay(500);
        delay(500);
        delay(500);
        delay(500);
        delay(500);
        delay(500);
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
    (void) line;
    /* Infinite loop */
    while (1) {
    }
}

#endif
