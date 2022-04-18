//
// Created by Ilia.Motornyi on 17/04/2022.
//

#ifndef HC_12_RR_BEACON_MAIN_H
#define HC_12_RR_BEACON_MAIN_H
void delay(uint16_t msec);

void radio_comm_SendCmd(uint8_t byteCount, uint8_t const *pData);

void vRadio_Init(void);

void vRadio_StartTx(uint8_t channel, const uint8_t *pioFixRadioPacket);

void sleepRadio(void);

#endif //HC_12_RR_BEACON_MAIN_H
