#ifndef __MPU6050_H
#define __MPU6050_H
#include "stm32f1xx_hal.h"
#include "i2c.h"
#include "cmsis_os.h"
#include "stdio.h"

#define SMPLRT_DIV 0x19   // 陀螺仪采样率，典型值：0x07(125Hz)
#define CONFIGL 0x1A      // 低通滤波频率，典型值：0x06(5Hz)
#define GYRO_CONFIG 0x1B  // 陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define ACCEL_CONFIG 0x1C // 加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
#define ACCEL_ADDRESS 0x3B
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H 0x41
#define TEMP_OUT_L 0x42
#define GYRO_XOUT_H 0x43
#define GYRO_ADDRESS 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48
#define PWR_MGMT_1 0x6B // 电源管理，典型值：0x00(正常启用)
#define WHO_AM_I 0x75   // IIC地址寄存器(默认数值0x68，只读)
#define MPU6050_PRODUCT_ID 0x68
#define MPU6052C_PRODUCT_ID 0x72
typedef struct{
	// 角速度
	float Accel_X;
	float Accel_Y;
	float Accel_Z;
	// 角度
	float Gyro_X;
	float Gyro_Y;
	float Gyro_Z;
	// 温度
	float Temp;
}MPU6050DATATYPE;

extern MPU6050DATATYPE Mpu6050_Data;
int16_t Sensor_I2C2_Serch(void);
int8_t MPU6050_Init(int16_t Addr);

int8_t Sensor_I2C2_ReadOneByte(uint16_t DevAddr, uint16_t MemAddr, uint8_t *oData);
int8_t Sensor_I2C2_WriteOneByte(uint16_t DevAddr, uint16_t MemAddr, uint8_t *iData);

void MPU6050_Read_Accel(void);
void MPU6050_Read_Gyro(void);
void MPU6050_Read_Temp(void);

int8_t Sensor_I2C2_Read(uint16_t DevAddr, uint16_t MemAddr, uint8_t *oData, uint8_t DataLen);
int8_t Sensor_I2C2_Write(uint16_t DevAddr, uint16_t MemAddr, uint8_t *iData, uint8_t DataLen);
void MPU6050_Read_Accel(void);
void MPU6050_Read_Gyro(void);
void MpuGetData(void *argument);
#endif // !__MPU6050_H