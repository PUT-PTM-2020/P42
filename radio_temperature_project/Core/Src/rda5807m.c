#include "rda5807m.h"


uint8_t buf1[64];
uint8_t buf2[64];

void RDA5807M_revertBytes(uint8_t *buf, uint8_t cnt){
	uint8_t tmp;
	while(cnt > 1){
		tmp = *(buf + 1);
		*(buf + 1) = *buf;
		*buf = tmp;
		buf += 2;
		cnt -= 2;
	}
}

void RDA5807M_write(I2C_HandleTypeDef *I2Cx, uint8_t RegAddress, uint16_t *buf, uint8_t RegNum){
	HAL_StatusTypeDef ret;

	RDA5807M_revertBytes((uint8_t *) buf, RegNum << 1);

	ret = HAL_I2C_Mem_Write(I2Cx, RDA5807M_Rand_Address << 1u, RegAddress, I2C_MEMADD_SIZE_8BIT, (uint8_t *) buf, RegNum << 1, 1000);
	if(ret != HAL_OK){
			//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 1);
		}

	RDA5807M_revertBytes((uint8_t *) buf, RegNum << 1);
}

void RDA5807M_writeRegister(I2C_HandleTypeDef *I2Cx, uint16_t *buf, uint8_t RegNum){
	HAL_StatusTypeDef ret;

	RDA5807M_revertBytes((uint8_t *) buf, RegNum << 1);
	ret = HAL_I2C_Master_Transmit(I2Cx, (uint16_t) RDA5807M_Seq_Address << 1, (uint8_t *) buf, RegNum << 1, 1000);
	if(ret != HAL_OK){
		//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 1);
	}
	RDA5807M_revertBytes((uint8_t *) buf, RegNum << 1);
}

void RDA5807M_read(I2C_HandleTypeDef *I2Cx, uint8_t RegAddress, uint16_t *buf, uint8_t RegNum){
	HAL_StatusTypeDef ret;

	ret = HAL_I2C_Mem_Read(I2Cx, RDA5807M_Rand_Address << 1, RegAddress, I2C_MEMADD_SIZE_8BIT, (uint8_t *) buf, RegNum << 1, 1000);

	if(ret != HAL_OK){
		//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 1);
	}

	RDA5807M_revertBytes((uint8_t *) buf, RegNum << 1);
}

void RDA5807M_init(I2C_HandleTypeDef *I2Cx){
	uint8_t buf[2] = {0, 0};
	HAL_StatusTypeDef ret;

	ret = HAL_I2C_Mem_Read(I2Cx, RDA5807M_Rand_Address << 1, 0, I2C_MEMADD_SIZE_8BIT, buf, 2, 1000);
	if(ret != HAL_OK || buf[0] != 0x58){
		//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 1);
	}


	ret = HAL_I2C_Mem_Read(I2Cx, RDA5807M_Rand_Address << 1, 0, I2C_MEMADD_SIZE_8BIT, buf, 1, 1000);
	if(ret != HAL_OK || buf[0] != 0x58){
		//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 1);


		NVIC_SystemReset();

	}

	for(int i = 0; i < 64; i++){
		buf1[i] = 0;
		buf2[i] = 0;
	}

	ret = HAL_I2C_Mem_Read(I2Cx, RDA5807M_Rand_Address << 1, 0, I2C_MEMADD_SIZE_8BIT, buf2, 64, 1000);
	if(ret != HAL_OK || buf[0] != 0x58){
			//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 1);
	}
	ret = HAL_I2C_Master_Receive(I2Cx, RDA5807M_Seq_Address << 1, buf1, 64, 1000);
	if(ret != HAL_OK || buf[0] != 0x58){
			//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, 1);
	}


	RDA5807M_softReset(I2Cx);
	RDA5807M_resetSettings(I2Cx);

	RDA5807M_seek(I2Cx, 1);
	while(RDA5807M_get_SeekReadyFlag(I2Cx));
}

void RDA5807M_softReset(I2C_HandleTypeDef *I2Cx){
	Reg02h r02;
	r02.Enable = 1;
	r02.Soft_Reset = 1;
	RDA5807M_write(I2Cx, 0x02, (uint16_t*) &r02, 1);
	r02.Enable = 1;
	r02.Soft_Reset = 0;
	RDA5807M_write(I2Cx, 0x02, (uint16_t*) &r02, 1);

}

void RDA5807M_resetSettings(I2C_HandleTypeDef *I2Cx){
	struct {
	        Reg02h r02;
	        Reg03h r03;
	        Reg04h r04;
	        Reg05h r05;
	        Reg06h r06;
	        Reg07h r07;
	    } buf;

	buf.r02.Enable 					= 1;
	buf.r02.Soft_Reset 				= 0;
	buf.r02.New_Demodulate 			= 1;
	buf.r02.RDS_Enable 				= 1;
	buf.r02.CLK_Mode 				= 0;
	buf.r02.Seek_Mode				= 0;
	buf.r02.Seek					= 0;
	buf.r02.Seek_Up					= 1;
	buf.r02.RCLK_Dir_In_Mode		= 0;
	buf.r02.RCLK_Non_Calib			= 0;
	buf.r02.Bass					= 0;
	buf.r02.Mono					= 0;
	buf.r02.Demute					= 1;
	buf.r02.DHIZ					= 1;

	buf.r03.Space					= 3;
	buf.r03.Band					= 0;
	buf.r03.Tune					= 1;
	buf.r03.Direct_Mode				= 0;
	buf.r03.Channel_Select			= 0;

	buf.r04.Reserved				= 0;
	buf.r04.AFCD					= 0;
	buf.r04.Soft_Mute				= 1;
	buf.r04.Reserved2				= 0;
	buf.r04.De_emphasis				= 0;
	buf.r04.Reserved3				= 0;

	buf.r05.Volume					= 0;
	buf.r05.Reserved				= 3;
	buf.r05.Reserved2				= 3;
	buf.r05.Seek_thresh				= 8;
	buf.r05.Reserved3				= 0;
	buf.r05.INT_Mode				= 1;

	buf.r06.Reserved				= 0;
	buf.r06.Open_mode				= 0;
	buf.r06.Reserved2				= 0;

	buf.r07.Freq_Mode				= 0;
	buf.r07.Soft_Blend_enable		= 1;
	buf.r07.Seek_thresh				= 0;
	buf.r07.Reserved				= 0;
	buf.r07.Mode_65M_50M			= 1;
	buf.r07.Soft_Blend_thresh		= 16;
	buf.r07.Reserved2				= 0;

	RDA5807M_writeRegister(I2Cx, (uint16_t *) &buf.r02, 6);
}

void RDA5807M_seek(I2C_HandleTypeDef *I2Cx, uint8_t direction){
	Reg02h r02;
	RDA5807M_read(I2Cx, 0x02, (uint16_t *) &r02, 1);
	RDA5807M_resetRDS_settings(I2Cx);
	r02.Seek_Mode	= 1;
	r02.Seek		= 1;
	if(direction){
		r02.Seek_Up	= 1;
	}else{
		r02.Seek_Up	= 0;
	}

	RDA5807M_write(I2Cx, 0x02, (uint16_t *) &r02, 1);
	//test
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13,0);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12,0);
}

uint8_t RDA5807M_get_SeekReadyFlag(I2C_HandleTypeDef *I2Cx){
	Reg0Ah r0A;

	RDA5807M_read(I2Cx, 0x0A, (uint16_t *) &r0A, 1);

	return r0A.Seek_Tune_Complete;
}

void RDA5807M_setVolume(I2C_HandleTypeDef *I2Cx, uint8_t value){
	Reg02h r02;
	Reg05h r05;
	uint8_t mute;

	if(value) mute = 0;
	else mute = 1;

	if(value > 16) value = 16;

	value--;

	if(!mute){
		RDA5807M_read(I2Cx, 0x05, (uint16_t *) &r05, 1);
		r05.Volume = value;
		RDA5807M_write(I2Cx, 0x05, (uint16_t *) &r05, 1);
	}

	RDA5807M_read(I2Cx, 0x02, (uint16_t *) &r02, 1);
	if(mute) r02.Demute = 0;
	else r02.Demute = 1;
	RDA5807M_write(I2Cx, 0x02, (uint16_t *) &r02, 1);
}

void RDA5807M_setFreq(I2C_HandleTypeDef *I2Cx, uint16_t freq){
	Reg03h r03;
	if(freq < 870) freq = 870;
	else if(freq > 1080) freq = 1080;

	freq -= 870;

	RDA5807M_read(I2Cx, 0x03, (uint16_t *) &r03, 1);
	r03.Channel_Select = freq;
	r03.Tune = 1;
	RDA5807M_write(I2Cx, 0x03, (uint16_t *) &r03, 1);
}

uint16_t RDA5807M_getFreq(I2C_HandleTypeDef *I2Cx){
	Reg0Ah r0A;
	uint16_t freq;

	RDA5807M_read(I2Cx, 0x0A, (uint16_t *) &r0A, 1);
	freq = r0A.Read_channel;
	freq = freq/4;
	if(freq == 319) return 0;

	freq += 870;

	return freq;
}



bool RDA5807M_readRDS(I2C_HandleTypeDef *I2Cx){
	Reg0Ah r0A;
	Reg0Bh r0B;

	RDA5807M_read(I2Cx, 0x0A, (uint16_t *) &r0A, 1);
	RDA5807M_read(I2Cx, 0x0B, (uint16_t *) &r0B, 1);


	RDA5807M_read(I2Cx, 0x0C, (uint16_t *) &RDS_BlockA, 1);
	RDA5807M_read(I2Cx, 0x0D, (uint16_t *) &RDS_BlockB, 1);
	RDA5807M_read(I2Cx, 0x0E, (uint16_t *) &RDS_BlockC, 1);
	RDA5807M_read(I2Cx, 0x0F, (uint16_t *) &RDS_BlockD, 1);

	if(r0B.BLERB >0 || r0B.BLERA >0){
						return false;
	}

	if(!r0A.RDS_Ready){
		return false;
	}

	if(r0A.RDS_Ready){
		if(!r0B.ABCD_E){

			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13,1);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12,1);
			RDA5807M_processRDS();
		}
	}
	return true;
}

char* RDA5807M_getStationName(){
	return ActualStationName;
}


void RDA5807M_resetRDS_settings(I2C_HandleTypeDef *I2Cx){//reset RDSa
	Reg02h r02;
	r02.Enable 					= 1;
	r02.Soft_Reset 				= 0;
	r02.New_Demodulate 			= 1;
	r02.RDS_Enable 				= 0;
	r02.CLK_Mode 				= 0;
	r02.Seek_Mode				= 0;
	r02.Seek					= 0;
	r02.Seek_Up					= 1;
	r02.RCLK_Dir_In_Mode		= 0;
	r02.RCLK_Non_Calib			= 0;
	r02.Bass					= 0;
	r02.Mono					= 0;
	r02.Demute					= 1;
	r02.DHIZ					= 1;
	RDA5807M_writeRegister(I2Cx,(uint16_t *) &r02,1);
	r02.RDS_Enable              = 1;
	RDA5807M_writeRegister(I2Cx,(uint16_t *) &r02,1);

	strcpy(StationName, "        ");
	RadReady = 0;
	strcpy(ActualStationName , "RadPogod");
}



void RDA5807M_processRDS(){
	unsigned char offset, char1, char2;
	int RDS_Group;
	RDS_Group = RDS_BlockB & 0xF800;
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);

	RDS_Group= RDS_BlockB >>10;
	if(RDS_Group == 0x0001){

		offset = (RDS_BlockB & 0x03);
		char1 = (char)(RDS_BlockD >> 8);
		char2 = (char)(RDS_BlockD & 0x00FF);
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
		switch (offset){
		case 0:
			if(StationName[0]!=char1 || StationName[1]!=char2){
				if(RadReady== 0xFF) RadReady = 0;
				StationName[0]=char1;
				StationName[1]=char2;
				RadReady = RadReady | 0xC0 ;
			}
			break;
		case 1:
			if(StationName[2]!=char1 || StationName[3]!=char2){
				if(RadReady== 0xFF) RadReady = 0;
				StationName[2]=char1;
				StationName[3]=char2;
				RadReady = RadReady | 0x30 ;
			}
			break;
		case 2:
			if(StationName[4]!=char1 || StationName[5]!=char2){
				if(RadReady== 0xFF) RadReady = 0;
				StationName[4]=char1;
				StationName[5]=char2;
				RadReady = RadReady | 0x0C ;
			}
			break;
		case 3:
			if(StationName[6]!=char1 || StationName[7]!=char2){
				if(RadReady== 0xFF) RadReady = 0;
				StationName[6]=char1;
				StationName[7]=char2;
				RadReady = RadReady | 0x03 ;
			}
			break;
		default:
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
			break;
		}
		if(RadReady == 0xFF){
			strcpy(ActualStationName , StationName);
		}
	}

}
