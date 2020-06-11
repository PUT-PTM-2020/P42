/*
 * ds18b20.c
 *
 *  Created on: May 27, 2020
 *      Author: mikolaj
 */

#include "ds18b20.h"

void Set_Pin_Output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}
void Set_Pin_Input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

uint8_t DS18B20_Start(void){
	uint8_t Response = 0;
	Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);
	HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, 0);
	delay(480);

	Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);
	delay(80);

	if(!(HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN))) Response = 1;
	else Response = -1;

	delay(400);

	return Response;
}

void DS18B20_Write(uint8_t data){
	Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);

	for(int i=0; i<8; i++){
		if((data & (1<<i))!=0){
			Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);
			HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, 0);
			delay(1);

			Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);
			delay(60);
		}
		else{
			Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);
			HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, 0);
			delay(60);

			Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);
		}
	}
}

uint8_t DS18B20_Read(void){
	uint8_t value = 0;
	Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);

	for(int i=0; i<8; i++){
		Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);

		HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, 0);
		delay(2);

		Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);
		if(HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN)){
			value |= 1 << i;
		}
		delay(60);
	}
	return value;
}
