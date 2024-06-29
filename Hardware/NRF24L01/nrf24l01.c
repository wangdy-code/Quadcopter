#include "nrf24l01.h"
#define Set_NRF24L01_CSN (NRF_CSN_GP->BSRR = NRF24L01_CSN) //
#define Clr_NRF24L01_CSN (NRF_CSN_GP->BRR = NRF24L01_CSN)  //
#define Set_NRF24L01_CE (NRF_CE_GP->BSRR = NRF24L01_CE)    //
#define Clr_NRF24L01_CE (NRF_CE_GP->BRR = NRF24L01_CE)     //
#define READ_NRF24L01_IRQ (NRF_IRQ_GP->IDR & NRF24L01_IRQ) // IRQ主机数据输入
void NRF24L01Init()
{
    Clr_NRF24L01_CE;  // 使能24L01
    Set_NRF24L01_CSN; // SPI片选取消
}
uint8_t NRF24SPISendByte(uint8_t txData)
{
    uint8_t rxData;
    HAL_SPI_TransmitReceive(&hspi2, &txData, &rxData, 1, 0x10);
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
        // printf("%d ", buf1[1]); // 读出写入的地址
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
} // 在指定位置读出指定长度的数据
//*pBuf:数据指针
// 返回值,此次读到的状态寄存器值
