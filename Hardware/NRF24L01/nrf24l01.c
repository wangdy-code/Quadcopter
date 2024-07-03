#include "nrf24l01.h"
#define Set_NRF24L01_CSN (NRF_CSN_GP->BSRR = NRF24L01_CSN)   //
#define Clr_NRF24L01_CSN (NRF_CSN_GP->BRR = NRF24L01_CSN)    //
#define Set_NRF24L01_CE (NRF_CE_GP->BSRR = NRF24L01_CE)      //
#define Clr_NRF24L01_CE (NRF_CE_GP->BRR = NRF24L01_CE)       //
#define READ_NRF24L01_IRQ (NRF_IRQ_GP->IDR & NRF24L01_IRQ)   // IRQ主机数据输入
const uint8_t TX_ADDRESS[] = {0xAA, 0xBB, 0xCC, 0x00, 0x01}; // 本地地址
const uint8_t RX_ADDRESS[] = {0xAA, 0xBB, 0xCC, 0x00, 0x01}; // 接收地址RX_ADDR_P0 == RX_ADDR
uint8_t NRF24L01_2_RXDATA[RX_PLOAD_WIDTH];                   // nrf24l01接收到的数据
uint8_t NRF24L01_2_TXDATA[RX_PLOAD_WIDTH];                   // nrf24l01需要发送的数据
void NRF24L01Init()
{
    Clr_NRF24L01_CE;  // 使能24L01
    Set_NRF24L01_CSN; // SPI片选取消
}
uint8_t NRF24SPISendByte(uint8_t txData)
{
    uint8_t rxData;
    if (HAL_SPI_TransmitReceive(&hspi2, &txData, &rxData, 1, 0xff) != HAL_OK)
    {
        return 0xff;
    }

    return rxData;
}
uint8_t NRF24WriteReg(uint8_t regAddr, uint8_t data)
{
    uint8_t status;
    Clr_NRF24L01_CSN;
    status = NRF24SPISendByte(regAddr);
    NRF24SPISendByte(data);
    Set_NRF24L01_CSN;
    return status;
}
uint8_t NRF24ReadReg(uint8_t regAddr)
{
    uint8_t regVal;
    Clr_NRF24L01_CSN;
    NRF24SPISendByte(regAddr);
    regVal = NRF24SPISendByte(NOP);
    Set_NRF24L01_CSN;
    return regVal;
}

uint8_t NRF24ReadBuf(uint8_t regAddr, uint8_t *pBuf, uint8_t dataLen)
{
    uint8_t status;
    Clr_NRF24L01_CSN;
    status = NRF24SPISendByte(regAddr);
    HAL_SPI_Receive(&hspi2, pBuf, dataLen, 0xff);
    Set_NRF24L01_CSN;
    return status;
}
uint8_t NRF24WriteBuf(uint8_t regAddr, uint8_t *pBuf, uint8_t dataLen)
{
    uint8_t status;
    Clr_NRF24L01_CSN;
    status = NRF24SPISendByte(regAddr);
    HAL_SPI_Transmit(&hspi2, pBuf, dataLen, 0xff);
    Set_NRF24L01_CSN;
    return status;
}
// 上电检测NRF24L01是否在位
// 写5个数据然后再读回来进行比较，
// 相同时返回值:0，表示在位;否则返回1，表示不在位
uint8_t NRF24L01Check(void)
{
    uint8_t buf[5] = {0XA5, 0XA5, 0XA5, 0XA5, 0XA5};
    uint8_t buf1[5];
    uint8_t i;
    NRF24WriteBuf(SPI_WRITE_REG + TX_ADDR, buf, 5); // 写入5个字节的地址.
    NRF24ReadBuf(TX_ADDR, buf1, 5);

    for (i = 0; i < 5; i++)
    {
        printf("%d ", buf1[1]); // 读出写入的地址
        if (buf1[i] != 0xA5)
        {

            break;
        }
    }
    if (i != 5)
    {
        return 1;
    }

    return 0; // NRF24L01在位
}

uint8_t NRF24L01TxPacket(uint8_t *txBuf)
{
    uint8_t state;
    Clr_NRF24L01_CE;
    NRF24WriteBuf(WR_TX_PLOAD, txBuf, WR_TX_PLOAD);
    Set_NRF24L01_CE;
    while (READ_NRF24L01_IRQ != 0)
        ;
    state = NRF24ReadReg(STATUS);
    NRF24WriteReg(SPI_WRITE_REG + STATUS, state);
    if (state & MAX_TX)
    {
        NRF24WriteReg(FLUSH_TX, 0xff);
        printf("NRF TX TXMAX error");
        return 1;
    }
    if (state & TX_OK)
    {
        printf("NRF TX OK");
        return 0;
    }
    printf("NRF OTHER TX ERROR");
    return 1;
}
uint8_t NRF24L01RXPacket(uint8_t *rxBuf)
{
    uint8_t state;
    state = NRF24ReadReg(STATUS);
    NRF24WriteReg(SPI_WRITE_REG + STATUS, state);
    if (state & RX_OK)
    {
        NRF24ReadBuf(RD_RX_PLOAD, rxBuf, RX_PLOAD_WIDTH);
        NRF24WriteReg(FLUSH_RX, 0xFF);
        return 0;
    }

    return 1;
}
void TXMode()
{
    Clr_NRF24L01_CE;
    // 写节点地址
    NRF24WriteBuf(SPI_WRITE_REG + TX_ADDR, (uint8_t *)TX_ADDRESS, TX_ADR_WIDTH);
    // 设置TX节点地址
    NRF24WriteBuf(SPI_WRITE_REG + RX_ADDR_P0, (uint8_t *)RX_ADDRESS, RX_ADR_WIDTH);
    // 使能通道0的自动应答
    NRF24WriteReg(SPI_WRITE_REG + EN_AA, 0x01);
    // 使能通道0的接收地址
    NRF24WriteReg(SPI_WRITE_REG + EN_RXADDR, 0x01);
    // 设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
    NRF24WriteReg(SPI_WRITE_REG + SETUP_RETR, 0x1a);
    // 设置RF通道为40
    NRF24WriteReg(SPI_WRITE_REG + RF_CH, 45);      //(要改单机控制，就把45改成跟遥控器单独一样的。就可以单机控制了)
                                                   // 设置TX发射参数,0db增益,2Mbps,低噪声增益开启
    NRF24WriteReg(SPI_WRITE_REG + RF_SETUP, 0x0f); // 0x27  250K   0x07 1M
    // 配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,PRIM_RX发送模式,开启所有中断
    NRF24WriteReg(SPI_WRITE_REG + NCONFIG, 0x0e);
    // CE为高,10us后启动发送
    Set_NRF24L01_CE;
}
// 该函数初始化NRF24L01到RX模式
// 设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
// 当CE变高后,即进入RX模式,并可以接收数据了
void RX_Mode(void)
{
    Clr_NRF24L01_CE;
    // 写RX节点地址
    NRF24WriteBuf(SPI_WRITE_REG + RX_ADDR_P0, (uint8_t *)RX_ADDRESS, RX_ADR_WIDTH);

    // 使能通道0的自动应答
    NRF24WriteReg(SPI_WRITE_REG + EN_AA, 0x01);
    // 使能通道0的接收地址
    NRF24WriteReg(SPI_WRITE_REG + EN_RXADDR, 0x01);
    // 设置RF通信频率
    NRF24WriteReg(SPI_WRITE_REG + RF_CH, 45); //(要改单机控制，就把45改成跟遥控器单独一样的。就可以单机控制了)
                                              // 选择通道0的有效数据宽度
    NRF24WriteReg(SPI_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH);
    // 设置TX发射参数,0db增益,2Mbps,低噪声增益开启
    NRF24WriteReg(SPI_WRITE_REG + RF_SETUP, 0x0f);
    // 配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,PRIM_RX接收模式
    NRF24WriteReg(SPI_WRITE_REG + NCONFIG, 0x0f);
    // CE为高,进入接收模式
    Set_NRF24L01_CE;
}
void ANO_NRF_Init(uint8_t model, uint8_t ch)
{
    Clr_NRF24L01_CE;

    NRF24WriteBuf(SPI_WRITE_REG + RX_ADDR_P0, (uint8_t *)RX_ADDRESS, RX_ADR_WIDTH); // 写RX节点地址
    NRF24WriteBuf(SPI_WRITE_REG + TX_ADDR, (uint8_t *)TX_ADDRESS, TX_ADR_WIDTH);    // 写TX节点地址
    NRF24WriteReg(SPI_WRITE_REG + EN_AA, 0x01);                                     // 使能通道0的自动应答
    NRF24WriteReg(SPI_WRITE_REG + EN_RXADDR, 0x01);                                 // 使能通道0的接收地址
    NRF24WriteReg(SPI_WRITE_REG + SETUP_RETR, 0x1a);                                // 设置自动重发间隔时间:500us;最大自动重发次数:10次 2M波特率下
    NRF24WriteReg(SPI_WRITE_REG + RF_CH, ch);                                       // 设置RF通道为CHANAL
    NRF24WriteReg(SPI_WRITE_REG + RF_SETUP, 0x0f);                                  // 设置TX发射参数,0db增益,2Mbps,低噪声增益开启
    // Write_Reg(NRF_WRITE_REG+RF_SETUP,0x07); 												    			//设置TX发射参数,0db增益,1Mbps,低噪声增益开启
    // Write_Reg(NRF_WRITE_REG+RF_SETUP,0x27); 												   				//设置TX发射参数,0db增益,250Kbps,低噪声增益开启
    /////////////////////////////////////////////////////////
    if (model == 1) // RX
    {
        NRF24WriteReg(SPI_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); // 选择通道0的有效数据宽度
        NRF24WriteReg(SPI_WRITE_REG + NCONFIG, 0x0f);            // IRQ收发完成中断开启,16位CRC,主接收
    }
    else if (model == 2) // TX
    {
        NRF24WriteReg(SPI_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); // 选择通道0的有效数据宽度
        NRF24WriteReg(SPI_WRITE_REG + NCONFIG, 0x0e);            // IRQ收发完成中断开启,16位CRC,主发送
    }
    else if (model == 3) // RX2
    {
        NRF24WriteReg(FLUSH_TX, 0xff);
        NRF24WriteReg(FLUSH_RX, 0xff);
        NRF24WriteReg(SPI_WRITE_REG + NCONFIG, 0x0f); // IRQ收发完成中断开启,16位CRC,主接收
        NRF24SPISendByte(0x50);
        NRF24SPISendByte(0x73);
        NRF24WriteReg(SPI_WRITE_REG + 0x1c, 0x01);
        NRF24WriteReg(SPI_WRITE_REG + 0x1d, 0x06);
    }
    else // TX2
    {
        NRF24WriteReg(SPI_WRITE_REG + NCONFIG, 0x0e); // IRQ收发完成中断开启,16位CRC,主发送
        NRF24WriteReg(FLUSH_TX, 0xff);
        NRF24WriteReg(FLUSH_RX, 0xff);

        NRF24SPISendByte(0x50);
        NRF24SPISendByte(0x73);
        NRF24WriteReg(SPI_WRITE_REG + 0x1c, 0x01);
        NRF24WriteReg(SPI_WRITE_REG + 0x1d, 0x06);
    }
    Set_NRF24L01_CE;
}
void NRF24L01_init(void)
{

    do
    {
        ANO_NRF_Init(MODEL_RX2, 0);
        printf("NRF24l01 INit\n");
    } while (NRF24L01Check() != 0);
}

void ANONRFCheckEvent(void *argument)
{
    while (1)
    {

        NRF24L01RXPacket(NRF24L01_2_RXDATA);
        // uint8_t state = NRF24ReadReg(SPI_READ_REG + STATUS);
        // printf("state:%d  \r\n", state);

        // if (state & (1 << 6))
        // {
        //     uint8_t rxLen = NRF24ReadReg(R_RX_PL_WID);
        //     printf("rxLEn %d \r\n", rxLen);
        //     if (rxLen < 33)
        //     {
        //         NRF24ReadBuf(RD_RX_PLOAD, NRF24L01_2_RXDATA, rxLen);
        //     }
        //     else
        //     {
        //         NRF24WriteReg(FLUSH_RX, 0xFF);
        //     }
        // }
        // if (state & (1 << 5))
        // {
        //     /* code */
        // }
        // if (state & (1 << 4))
        // {
        //     if (state & 0x01)
        //     {
        //         NRF24WriteReg(FLUSH_TX, 0xFF);
        //     }
        // }

        // NRF24WriteReg(SPI_WRITE_REG + STATUS, state);
        osDelay(100);
    }
}

void NRFConnect(void *argument)
{
    while (1)
    {
        printf("NRF24L01_2_RXDATA:[");
        for (uint8_t i = 0; i < NRF24L01_2_RXDATA[3] + 5; i++)
        {
            printf("%d, ", NRF24L01_2_RXDATA[i]);
        }

        printf("]   \r\n");
        osDelay(100);
    }
}