#include "MPU6050.h"
static int16_t MPU6050_ADDRESS = 0xD0;
int8_t Mpu6050Rest()
{
    uint8_t Data;
    Data = 0x80;
    if (HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDRESS, PWR_MGMT_1, I2C_MEMADD_SIZE_8BIT, &Data, 1, I2C_SR1_TIMEOUT) == HAL_ERROR)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}

int8_t MPU6050Init()
{
    uint8_t Data;
    int date = SUCCESS;

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1 | GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9 | GPIO_PIN_10, GPIO_PIN_RESET);

    do
    {
        Data = 0x80;
        date = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDRESS, PWR_MGMT_1, I2C_MEMADD_SIZE_8BIT, &Data, 1, I2C_SR1_TIMEOUT); // 复位
        HAL_Delay(30);
        Data = 0x02;
        date += HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDRESS, SMPLRT_DIV, I2C_MEMADD_SIZE_8BIT, &Data, 1, I2C_SR1_TIMEOUT); // 陀螺仪采样率，0x00(500Hz)
        Data = 0x03;
        date += HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDRESS, PWR_MGMT_1, I2C_MEMADD_SIZE_8BIT, &Data, 1, I2C_SR1_TIMEOUT); // 设置设备时钟源，陀螺仪Z轴
        Data = 0x03;
        date += HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDRESS, CONFIGL, I2C_MEMADD_SIZE_8BIT, &Data, 1, I2C_SR1_TIMEOUT); // 低通滤波频率，0x03(42Hz)
        Data = 0x18;
        date += HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDRESS, GYRO_CONFIG, I2C_MEMADD_SIZE_8BIT, &Data, 1, I2C_SR1_TIMEOUT); //+-2000deg/s
        Data = 0x09;
        date += HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDRESS, ACCEL_CONFIG, I2C_MEMADD_SIZE_8BIT, &Data, 1, I2C_SR1_TIMEOUT); //+-4G

    } while (date != HAL_OK);
    Data = 0x75;
    date = HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, &Data, 1, I2C_SR1_TIMEOUT);
    if (date != MPU6050_PRODUCT_ID)
    {
        return HAL_ERROR;
    }
    else
    {
        MpuGetOffset();
        return HAL_OK;
    }
}