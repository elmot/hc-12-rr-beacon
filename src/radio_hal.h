//
// Created by Ilia.Motornyi on 15/03/2022.
//

#ifndef L4_SI4463_TEST3_RADIO_HAL_H
#define L4_SI4463_TEST3_RADIO_HAL_H
#define U8 uint8_t
#define U16 uint16_t

bool radio_hal_NirqLevel(void);

void radio_hal_ClearNsel(void);
void radio_hal_SetNsel(void);

void radio_hal_AssertShutdown(void);
void radio_hal_DeassertShutdown(void);

void radio_hal_SpiWriteByte(uint8_t);
uint8_t radio_hal_SpiReadByte(void);

void radio_hal_SpiWriteData(uint8_t byteCount,const uint8_t *pData);
void radio_hal_SpiReadData(uint8_t byteCount, uint8_t *pData);

void delay(uint16_t msec);

#endif //L4_SI4463_TEST3_RADIO_HAL_H
