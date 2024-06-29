#include "led.h"
#include "usart.h"
#include "stdio.h"
sLED LED = {300, AllFlashLight};
void LEDInit()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);

    /*Configure GPIO pins : PB1 PB2 PB8 PB9 */
    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void PilotLED(void *argument)
{
    while (1)
    {
        switch (LED.status)
        {
        case AlwaysOff:
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_SET);
            break;

        case AllFlashLight:
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_8 | GPIO_PIN_9);
            break;
        case AlwaysOn:
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);
            break;
        case AlternateFlash:
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1 | GPIO_PIN_2, GPIO_PIN_SET);

            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1 | GPIO_PIN_2, GPIO_PIN_RESET);
            LED.status = AllFlashLight;
            break;
        case WARNING:
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_8 | GPIO_PIN_9);
            LED.FlashTime = 800;
            break;
        case DANGEROURS:
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_8 | GPIO_PIN_9);
            LED.FlashTime = 70;
            break;
        default:
            LED.status = AlwaysOff;
            break;
        }
        osDelay(LED.FlashTime);
    }
}