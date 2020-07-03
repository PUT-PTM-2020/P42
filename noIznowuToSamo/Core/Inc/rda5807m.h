/*
 * rda5807m.h
 *
 *  Created on: May 16, 2020
 *      Author: mikolaj
 */

#ifndef INC_RDA5807M_H_
#define INC_RDA5807M_H_

#include "stm32f4xx_hal.h"
#include "stdbool.h"
#include "string.h"

#define RDA5807M_Seq_Address  0x10
#define RDA5807M_Rand_Address 0x11

#define I2C_wait_time 1000

/********REGISTERS****************/

typedef struct {
	uint16_t Enable				:1;
	uint16_t Soft_Reset			:1;
	uint16_t New_Demodulate		:1;
	uint16_t RDS_Enable			:1;
	uint16_t CLK_Mode			:3;
	uint16_t Seek_Mode			:1;
	uint16_t Seek				:1;
	uint16_t Seek_Up			:1;
	uint16_t RCLK_Dir_In_Mode	:1;
	uint16_t RCLK_Non_Calib		:1;
	uint16_t Bass	            :1;
	uint16_t Mono	            :1;
	uint16_t Demute	            :1;
	uint16_t DHIZ	            :1;
} Reg02h;

typedef struct {
    uint16_t Space              :2;
    uint16_t Band               :2;
    uint16_t Tune               :1;
    uint16_t Direct_Mode        :1;
    uint16_t Channel_Select     :10;
} Reg03h;

typedef struct {
    uint16_t Reserved           :8;
    uint16_t AFCD               :1;
    uint16_t Soft_Mute          :1;
    uint16_t Reserved2          :1;
    uint16_t De_emphasis        :1;
    uint16_t Reserved3          :4;
} Reg04h;

typedef struct {
    uint16_t Volume             :4;
    uint16_t Reserved           :2;
    uint16_t Reserved2          :2;
    uint16_t Seek_thresh        :4;
    uint16_t Reserved3          :3;
    uint16_t INT_Mode           :1;
} Reg05h;

typedef struct {
    uint16_t Reserved           :13;
    uint16_t Open_mode          :2;
    uint16_t Reserved2          :1;
} Reg06h;

typedef struct {
    uint16_t Freq_Mode			:1;
    uint16_t Soft_Blend_thresh  :1;
    uint16_t Seek_thresh        :6;
    uint16_t Reserved           :1;
    uint16_t Mode_65M_50M       :1;
    uint16_t Soft_Blend_enable  :5;
    uint16_t Reserved2          :1;
} Reg07h;

typedef struct {
    uint16_t Read_channel       :10;
    uint16_t Stereo_ind         :1;
    uint16_t BLK_E		        :1;
    uint16_t RDS_Sync	        :1;
    uint16_t Seek_Fail          :1;
    uint16_t Seek_Tune_Complete :1;
    uint16_t RDS_Ready	        :1;
} Reg0Ah;

typedef struct {
    uint16_t BLERB			     :2;
    uint16_t BLERA		   	     :2;
    uint16_t ABCD_E 		     :1;
    uint16_t Reserved		     :2;
    uint16_t FM_Ready		     :1;
    uint16_t FM_True		     :1;
    uint16_t RSSI		         :7;
} Reg0Bh;

uint16_t RDS_BlockA;
uint16_t RDS_BlockB;
uint16_t RDS_BlockC;
uint16_t RDS_BlockD;

char StationName[16];
char ActualStationName[9];
char StName1[10];
char StName2[10];
char RDSmessage[64];
char RDStime[12];
int messageAB;
int messageABlast;
int lastMessage;
int	rdsGroup,rdstp,rdspty;
bool hasStationName;
/**********FUNCTIONS***************/
void RDA5807M_revertBytes(uint8_t *buf, uint8_t cnt);
void RDA5807M_write(I2C_HandleTypeDef *I2Cx, uint8_t RegAddress, uint16_t *buf, uint8_t RegNum);
void RDA5807M_writeRegister(I2C_HandleTypeDef *I2Cx, uint16_t *buf, uint8_t RegNum);
void RDA5807M_read(I2C_HandleTypeDef *I2Cx, uint8_t RegAddress, uint16_t *buf, uint8_t RegNum);
void RDA5807M_init(I2C_HandleTypeDef *I2Cx);
void RDA5807M_softReset(I2C_HandleTypeDef *I2Cx);
void RDA5807M_resetSettings(I2C_HandleTypeDef *I2Cx);
void RDA5807M_seek(I2C_HandleTypeDef *I2Cx, uint8_t direction); //up = 1
uint8_t RDA5807M_get_SeekReadyFlag(I2C_HandleTypeDef *I2Cx);
void RDA5807M_setVolume(I2C_HandleTypeDef *I2Cx, uint8_t value);
void RDA5807M_setFreq(I2C_HandleTypeDef *I2Cx, uint16_t freq);
uint16_t RDA5807M_getFreq(I2C_HandleTypeDef *I2Cx);
bool RDA5807M_readRDS(I2C_HandleTypeDef *I2Cx);
#endif /* INC_RDA5807M_H_ */
