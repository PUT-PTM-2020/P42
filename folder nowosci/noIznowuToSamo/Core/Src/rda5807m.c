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
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 1);
		}

	RDA5807M_revertBytes((uint8_t *) buf, RegNum << 1);
}

void RDA5807M_writeRegister(I2C_HandleTypeDef *I2Cx, uint16_t *buf, uint8_t RegNum){
	HAL_StatusTypeDef ret;

	RDA5807M_revertBytes((uint8_t *) buf, RegNum << 1);
	ret = HAL_I2C_Master_Transmit(I2Cx, (uint16_t) RDA5807M_Seq_Address << 1, (uint8_t *) buf, RegNum << 1, 1000);
	if(ret != HAL_OK){
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 1);
	}
	RDA5807M_revertBytes((uint8_t *) buf, RegNum << 1);
}

void RDA5807M_read(I2C_HandleTypeDef *I2Cx, uint8_t RegAddress, uint16_t *buf, uint8_t RegNum){
	HAL_StatusTypeDef ret;

	ret = HAL_I2C_Mem_Read(I2Cx, RDA5807M_Rand_Address << 1, RegAddress, I2C_MEMADD_SIZE_8BIT, (uint8_t *) buf, RegNum << 1, 1000);

	if(ret != HAL_OK){
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 1);
	}

	RDA5807M_revertBytes((uint8_t *) buf, RegNum << 1);
}

void RDA5807M_init(I2C_HandleTypeDef *I2Cx){
	uint8_t buf[2] = {0, 0};
	HAL_StatusTypeDef ret;

	//TODO: czy to konieczne
	//nie wiem
	ret = HAL_I2C_Mem_Read(I2Cx, RDA5807M_Rand_Address << 1, 0, I2C_MEMADD_SIZE_8BIT, buf, 2, 1000);
	if(ret != HAL_OK || buf[0] != 0x58){
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 1);
	}

	//TODO: czy to konieczne, byc moze wystarczy tylko raz sprawdzac
	ret = HAL_I2C_Mem_Read(I2Cx, RDA5807M_Rand_Address << 1, 0, I2C_MEMADD_SIZE_8BIT, buf, 1, 1000);
	if(ret != HAL_OK || buf[0] != 0x58){
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 1);


		NVIC_SystemReset(); //jak się nagle rozpimpasi całe te, to przez te linijke

	}

	for(int i = 0; i < 64; i++){
		buf1[i] = 0;
		buf2[i] = 0;
	}

	ret = HAL_I2C_Mem_Read(I2Cx, RDA5807M_Rand_Address << 1, 0, I2C_MEMADD_SIZE_8BIT, buf2, 64, 1000);
	if(ret != HAL_OK || buf[0] != 0x58){
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 1);
	}
	ret = HAL_I2C_Master_Receive(I2Cx, RDA5807M_Seq_Address << 1, buf1, 64, 1000);
	if(ret != HAL_OK || buf[0] != 0x58){
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, 1);
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

	buf.r03.Space					= 0;
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
	buf.r05.Reserved				= 0;
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

	r02.Seek_Mode	= 1;
	r02.Seek		= 1;
	if(direction){
		r02.Seek_Up	= 1;
	}else{
		r02.Seek_Up	= 0;
	}

	RDA5807M_write(I2Cx, 0x02, (uint16_t *) &r02, 1);
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

	if(freq == 319) return 0;

	freq += 870;

	return freq;
}


bool RDA5807M_readRDS(I2C_HandleTypeDef *I2Cx){
	Reg0Ah r0A;
	RDA5807M_read(I2Cx, 0x0A, (uint16_t *) &r0A, 1);

	if(!r0A.RDS_Ready) return false;
	RDA5807M_read(I2Cx, 0x0C, (uint16_t *) &RDS_BlockA, 1);
	RDA5807M_read(I2Cx, 0x0D, (uint16_t *) &RDS_BlockB, 1);
	RDA5807M_read(I2Cx, 0x0E, (uint16_t *) &RDS_BlockC, 1);
	RDA5807M_read(I2Cx, 0x0F, (uint16_t *) &RDS_BlockD, 1);

	return true;
}
void RDA5807M_resetRDS(I2C_HandleTypeDef *I2Cx){
	 strcpy(StName1, "        ");
	 strcpy(StName2, StName1);
	 strcpy(StationName, "        ");
	 memset(RDSmessage, '\0', sizeof(RDSmessage));
	 lastMessage = 0;
}
void RDA5807M_processRDS(I2C_HandleTypeDef *I2Cx){
	 char c1, c2;
	 char StationNametmp[10];
	  int  i;     // index of rdsText
	  int tb;
	  int mins;     // RDS time in minutes
	  int offset;      // RDS time offset and sign
	  RDA5807M_readRDS(I2Cx);
	  if (RDS_BlockA==0) {
	    // reset all the RDS info.
		  RDA5807M_resetRDS(I2Cx);
	    // Send out empty data
		  /*
	    if (sendServiceName) sendServiceName(StationName);
	    if (sendText)        sendText("");
	    return;*/
	  }

	  // analyzing Block B
	  rdsGroup = 0x0A | ((RDS_BlockB & 0xF000) >> 8) | ((RDS_BlockB & 0x0800) >> 11);
	  //rdstp = (RDS_BlockB & 0x0400);
	  //rdspty = (RDS_BlockB & 0x0400);
	  tb=rdsGroup;
	  switch (rdsGroup) {
	  case 0x0A:
	  case 0x0B:
	    // The data received is part of the Service Station Name

	    i = 2 * (RDS_BlockB & 0x0003);
	    // new data is 2 chars from block 4
	    c1 = RDS_BlockD >> 8;
	    c2 = RDS_BlockD & 0x00FF;
	    // check that the data was received successfully twice
	    // before publishing the station name
	    //if ((StName1[i] == c1) && (StName1[i + 1] == c2)) {
	      // retrieved the text a second time: store to _PSName2
	      StName2[i] = c1;
	      StName2[i + 1] = c2;
	      StName2[8] = '\0';
	      if ((i == 6) && strcmp(StName1, StName2) == 0) {
	        if (strcmp(StName2, StationName) != 0) {

	          if(strlen(StName2)<9){strcpy(StationName, StName2);
	          strcpy(StationNametmp, StName1);}
	          /*if (sendServiceName)
	            sendServiceName(StationName);*/
	        }
	      }
	    //}
	    if ((StName1[i] != c1) || (StName1[i + 1] != c2)) {
	    	StName1[i] = c1;
	    	StName1[i + 1] = c2;
	    	StName1[8] = '\0';
	    }
	    break;

	  case 0x2A:
	  //  if(rdsblockerror > 1){memset(RDSText, '\0', sizeof(RDSText));lastTextIDX = 0;}
	    messageAB = (RDS_BlockB & 0x0010);
	    i = 4 * (RDS_BlockB & 0x000F);

	    if (i < lastMessage) {
	      /*if (sendText)
	        sendText(RDSText);*/
	    }
	    messageABlast = i;

	    if (messageAB != messageABlast) {
	    	messageABlast = messageAB;
	    }
	    RDSmessage[i] = (RDS_BlockC >> 8);     i++;
	    RDSmessage[i] = (RDS_BlockC & 0x00FF); i++;
	    RDSmessage[i] = (RDS_BlockD >> 8); i++;
	    RDSmessage[i] = (RDS_BlockD & 0x00FF); i++;

	    break;

	  case 0x4A:
	    offset = (RDS_BlockD) & 0x3F; // 6 bits
	    mins = (RDS_BlockD >> 6) & 0x3F; // 6 bits
	    mins += 60 * (((RDS_BlockC & 0x0001) << 4) | ((RDS_BlockD >> 12) & 0x0F));

	    if (offset & 0x20) {
	      mins -= 30 * (offset & 0x1F);
	    }
	    else {
	      mins += 30 * (offset & 0x1F);
	    }


	    sprintf(RDStime, "%d", mins);
	    //  }
	    break;

	  case 0x6A:
	    // IH
	    break;

	  case 0x8A:
	    // TMC
	    break;

	  case 0xAA:
	    // TMC
	    break;

	  case 0xCA:
	    // TMC
	    break;

	  case 0xEA:
	    // IH
	    break;

	  default:

	    break;
	  }
}


