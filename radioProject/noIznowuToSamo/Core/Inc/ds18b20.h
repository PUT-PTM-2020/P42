/*
 * ds18b20.h
 *
 *  Created on: May 27, 2020
 *      Author: mikolaj
 */

#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include <stdlib.h>
#include <stdio.h>

#ifndef INC_DS18B20_H_
#define INC_DS18B20_H_

#define DS18B20_PORT GPIOA
#define DS18B20_PIN GPIO_PIN_1

void Set_Pin_Output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void Set_Pin_Input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

uint8_t DS18B20_Start(void);

void DS18B20_Write(uint8_t data);

uint8_t DS18B20_Read(void);


#endif /* INC_DS18B20_H_ */
