#include "stdbool.h"
#include "stdint.h"
#include "radio_hal.h"
#include "si446x_cmd.h"

#ifdef Q30_003MHZ
#  include "radio_config_Q30.003MHz.h"
#else
#  include "radio_config_Q30.000MHz.h"
#endif

#define RADIO_CTS_TIMEOUT 255;
enum
{
    SI446X_SUCCESS,
    SI446X_NO_PATCH,
    SI446X_CTS_TIMEOUT,
    SI446X_PATCH_FAIL,
    SI446X_COMMAND_ERROR
};


uint8_t Pro2Cmd[16];
bool ctsWentHigh = false;
union si446x_cmd_reply_union Si446xCmd;
const uint8_t Radio_Configuration_Data_Array[] =
              RADIO_CONFIGURATION_DATA_ARRAY;


void si446x_get_int_status(uint8_t PH_CLR_PEND, uint8_t MODEM_CLR_PEND, uint8_t CHIP_CLR_PEND);

/*!
 * Gets a command response from the radio chip
 *
 * @param byteCount     Number of bytes to get from the radio chip
 * @param pData         Pointer to where to put the data
 *
 * @return CTS value
 */
uint8_t radio_comm_GetResp(uint8_t byteCount, uint8_t * pData)
{
    uint8_t ctsVal = 0u;
    uint16_t errCnt = RADIO_CTS_TIMEOUT;

    while (errCnt != 0)      //wait until radio IC is ready with the data
    {
        radio_hal_ClearNsel();
        radio_hal_SpiWriteByte(0x44);    //read CMD buffer
        ctsVal = radio_hal_SpiReadByte();
        if (ctsVal == 0xFF)
        {
            if (byteCount)
            {
                radio_hal_SpiReadData(byteCount, pData);
            }
            radio_hal_SetNsel();
            break;
        }
        radio_hal_SetNsel();
        errCnt--;
    }

    if (errCnt == 0)
    {
        while(1)
        {
            /* ERROR!!!!  CTS should never take this long. */
#ifdef RADIO_COMM_ERROR_CALLBACK
            RADIO_COMM_ERROR_CALLBACK();
#endif
        }
    }

    if (ctsVal == 0xFF)
    {
        ctsWentHigh = true;
    }

    return ctsVal;
}


/*!
 * Waits for CTS to be high
 *
 * @return CTS value
 */
uint8_t radio_comm_PollCTS(void)
{
#ifdef RADIO_USER_CFG_USE_GPIO1_FOR_CTS
    while(!radio_hal_Gpio1Level())
    {
        /* Wait...*/
    }
    ctsWentHigh = 1;
    return 0xFF;
#else
    return radio_comm_GetResp(0, 0);
#endif
}

/*!
 * Sends a command to the radio chip
 *
 * @param byteCount     Number of bytes in the command to send to the radio device
 * @param pData         Pointer to the command to send.
 */
void radio_comm_SendCmd(uint8_t byteCount,const uint8_t* pData)
{
    while (!ctsWentHigh)
    {
        radio_comm_PollCTS();
    }
    radio_hal_ClearNsel();
    radio_hal_SpiWriteData(byteCount, pData);
    radio_hal_SetNsel();
    ctsWentHigh = false;
}

uint8_t radio_comm_SendCmdGetResp(uint8_t cmdByteCount, uint8_t * pCmdData, uint8_t  respByteCount, uint8_t * pRespData)
{
    radio_comm_SendCmd(cmdByteCount, pCmdData);
    return radio_comm_GetResp(respByteCount, pRespData);
}
/*!
 * This function is used to load all properties and commands with a list of NULL terminated commands.
 * Before this function @si446x_reset should be called.
 */
uint8_t si446x_configuration_init(const uint8_t* pSetPropCmd)
{
    uint8_t col;
    uint8_t numOfBytes;

    /* While cycle as far as the pointer points to a command */
    while (*pSetPropCmd != 0x00)
    {
        /* Commands structure in the array:
         * --------------------------------
         * LEN | <LEN length of data>
         */

        numOfBytes = *pSetPropCmd++;

        if (numOfBytes > 16u)
        {
            /* Number of command bytes exceeds maximal allowable length */
            return SI446X_COMMAND_ERROR;
        }

        for (col = 0u; col < numOfBytes; col++)
        {
            Pro2Cmd[col] = *pSetPropCmd;
            pSetPropCmd++;
        }

        if (radio_comm_SendCmdGetResp(numOfBytes, Pro2Cmd, 0, 0) != 0xFF)
        {
            /* Timeout occured */
            return SI446X_CTS_TIMEOUT;
        }

        if (radio_hal_NirqLevel() == 0)
        {
            /* Get and clear all interrupts.  An error has occured... */
            si446x_get_int_status(0, 0, 0);
            if (Si446xCmd.GET_INT_STATUS.CHIP_PEND & SI446X_CMD_GET_CHIP_STATUS_REP_CHIP_PEND_CMD_ERROR_PEND_MASK)
            {
                return SI446X_COMMAND_ERROR;
            }
        }
    }

    return SI446X_SUCCESS;
}

/*!
 * Send the FIFO_INFO command to the radio. Optionally resets the TX/RX FIFO. Reads the radio response back
 * into @Si446xCmd.
 *
 * @param FIFO  RX/TX FIFO reset flags.
 */
void si446x_fifo_info(uint8_t FIFO)
{
    Pro2Cmd[0] = SI446X_CMD_ID_FIFO_INFO;
    Pro2Cmd[1] = FIFO;

    radio_comm_SendCmdGetResp( SI446X_CMD_ARG_COUNT_FIFO_INFO,
                               Pro2Cmd,
                               SI446X_CMD_REPLY_COUNT_FIFO_INFO,
                               Pro2Cmd );

    Si446xCmd.FIFO_INFO.RX_FIFO_COUNT   = Pro2Cmd[0];
    Si446xCmd.FIFO_INFO.TX_FIFO_SPACE   = Pro2Cmd[1];
}
/*! Sends START_TX command to the radio.
 *
 * @param CHANNEL   Channel number.
 * @param CONDITION Start TX condition.
 * @param TX_LEN    Payload length (exclude the PH generated CRC).
 */
void si446x_start_tx(uint8_t CHANNEL, uint8_t CONDITION, uint16_t TX_LEN)
{
    Pro2Cmd[0] = SI446X_CMD_ID_START_TX;
    Pro2Cmd[1] = CHANNEL;
    Pro2Cmd[2] = CONDITION;
    Pro2Cmd[3] = (uint8_t)(TX_LEN >> 8);
    Pro2Cmd[4] = (uint8_t)(TX_LEN);
    Pro2Cmd[5] = 0x00;

    // Don't repeat the packet,
    // ie. transmit the packet only once
    Pro2Cmd[6] = 0x00;

    radio_comm_SendCmd( SI446X_CMD_ARG_COUNT_START_TX, Pro2Cmd );
}
/*!
 * Gets a command response from the radio chip
 *
 * @param cmd           Command ID
 * @param pollCts       Set to poll CTS
 * @param byteCount     Number of bytes to get from the radio chip
 * @param pData         Pointer to where to put the data
 */
void radio_comm_WriteData(uint8_t cmd, bool pollCts, uint8_t byteCount,const uint8_t* pData)
{
    if(pollCts)
    {
        while(!ctsWentHigh)
        {
            radio_comm_PollCTS();
        }
    }
    radio_hal_ClearNsel();
    radio_hal_SpiWriteByte(cmd);
    radio_hal_SpiWriteData(byteCount, pData);
    radio_hal_SetNsel();
    ctsWentHigh = 0;
}

/*!
 * The function can be used to load data into TX FIFO.
 *
 * @param numBytes  Data length to be load.
 * @param pTxData   Pointer to the data (U8*).
 */
void si446x_write_tx_fifo(uint8_t numBytes,const uint8_t* pTxData)
{
    radio_comm_WriteData( SI446X_CMD_ID_WRITE_TX_FIFO, 0, numBytes, pTxData );
}
/*!
 * Get the Interrupt status/pending flags form the radio and clear flags if requested.
 *
 * @param PH_CLR_PEND     Packet Handler pending flags clear.
 * @param MODEM_CLR_PEND  Modem Status pending flags clear.
 * @param CHIP_CLR_PEND   Chip State pending flags clear.
 */
void si446x_get_int_status(uint8_t PH_CLR_PEND, uint8_t MODEM_CLR_PEND, uint8_t CHIP_CLR_PEND)
{
    Pro2Cmd[0] = SI446X_CMD_ID_GET_INT_STATUS;
    Pro2Cmd[1] = PH_CLR_PEND;
    Pro2Cmd[2] = MODEM_CLR_PEND;
    Pro2Cmd[3] = CHIP_CLR_PEND;

    radio_comm_SendCmdGetResp( SI446X_CMD_ARG_COUNT_GET_INT_STATUS,
                               Pro2Cmd,
                               SI446X_CMD_REPLY_COUNT_GET_INT_STATUS,
                               Pro2Cmd );

    Si446xCmd.GET_INT_STATUS.INT_PEND       = Pro2Cmd[0];
    Si446xCmd.GET_INT_STATUS.INT_STATUS     = Pro2Cmd[1];
    Si446xCmd.GET_INT_STATUS.PH_PEND        = Pro2Cmd[2];
    Si446xCmd.GET_INT_STATUS.PH_STATUS      = Pro2Cmd[3];
    Si446xCmd.GET_INT_STATUS.MODEM_PEND     = Pro2Cmd[4];
    Si446xCmd.GET_INT_STATUS.MODEM_STATUS   = Pro2Cmd[5];
    Si446xCmd.GET_INT_STATUS.CHIP_PEND      = Pro2Cmd[6];
    Si446xCmd.GET_INT_STATUS.CHIP_STATUS    = Pro2Cmd[7];
}

void vRadio_StartTx(uint8_t channel, const uint8_t *pioFixRadioPacket) {
    /* Reset TX FIFO */
    si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_FIFO_TX_BIT);

    // Read ITs, clear pending ones
    si446x_get_int_status(0u, 0u, 0u);

    /* Fill the TX fifo with datas */
    si446x_write_tx_fifo(RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH, pioFixRadioPacket);

    /* Start sending packet, channel 0, START immediately, Packet length according to PH, go READY when done */
    si446x_start_tx(channel, 0x30,  0x00);
}

/*!
 *  Power up the Radio.
 *
 *  @note
 *
 */
void vRadio_PowerUp(void)
{

    /* Put radio in shutdown, wait then release */
    radio_hal_AssertShutdown();
    delay(150);//todo optimize
    radio_hal_DeassertShutdown();
    ctsWentHigh = 0;
    /* Wait until reset timeout or Reset IT signal */
    delay(150);//todo optimize
}

void vRadio_Init(){
    uint16_t wDelay;

    /* Power Up the radio chip */
    vRadio_PowerUp();

    /* Load radio configuration */
    while (SI446X_SUCCESS != si446x_configuration_init(Radio_Configuration_Data_Array))
    {
        /* Error hook */
#if !(defined SILABS_PLATFORM_WMB912)
//    LED4 = !LED4;
#else
        vCio_ToggleLed(eHmi_Led4_c);
#endif
        for (wDelay = 0x7FFF; wDelay--; ) ;
        /* Power Up the radio chip */
        vRadio_PowerUp();
    }

    // Read ITs, clear pending ones
    si446x_get_int_status(0u, 0u, 0u);
}


/*!
 *  Check if Packet sent IT flag is pending.
 *
 *  @return   TRUE / FALSE
 *
 *  @note
 *
 */
bool gRadio_CheckTransmitted(void)
{
    if (radio_hal_NirqLevel() == false)
    {
        /* Read ITs, clear pending ones */
        si446x_get_int_status(0u, 0u, 0u);

        /* check the reason for the IT */
        if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_SENT_PEND_BIT)
        {
            return true;
        }
    }

    return false;
}




