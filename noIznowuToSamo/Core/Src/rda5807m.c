#include "rda5807m.h"
//#include "lcd1602.h"

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
	ich=0;
	uint8_t buf[2] = {0, 0};
	HAL_StatusTypeDef ret;
	 hasStationName =0;
	//TODO: czy to konieczne
	//nie wiem
	ret = HAL_I2C_Mem_Read(I2Cx, RDA5807M_Rand_Address << 1, 0, I2C_MEMADD_SIZE_8BIT, buf, 2, 1000);
	if(ret != HAL_OK || buf[0] != 0x58){
		//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 1);
	}

	//TODO: czy to konieczne, byc moze wystarczy tylko raz sprawdzac
	ret = HAL_I2C_Mem_Read(I2Cx, RDA5807M_Rand_Address << 1, 0, I2C_MEMADD_SIZE_8BIT, buf, 1, 1000);
	if(ret != HAL_OK || buf[0] != 0x58){
		//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 1);


		NVIC_SystemReset(); //jak się nagle rozpimpasi całe te, to przez te linijke

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
	 hasStationName =0;
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
	 hasStationName =0;
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
	RDA5807M_read(I2Cx, 0x0A, (uint16_t *) &r0A, 1);

	if(!r0A.RDS_Ready) return false;
	RDA5807M_read(I2Cx, 0x0C, (uint16_t *) &RDS_BlockA, 1);
	RDA5807M_read(I2Cx, 0x0D, (uint16_t *) &RDS_BlockB, 1);
	RDA5807M_read(I2Cx, 0x0E, (uint16_t *) &RDS_BlockC, 1);
	RDA5807M_read(I2Cx, 0x0F, (uint16_t *) &RDS_BlockD, 1);

	return true;
}
bool RDA5807M_readRDSmkII(I2C_HandleTypeDef *I2Cx){
	Reg0Ah r0A;
	Reg0Bh r0B;
	Reg02h r02;
	RDA5807M_read(I2Cx, 0x0A, (uint16_t *) &r0A, 1);
	RDA5807M_read(I2Cx, 0x0B, (uint16_t *) &r0B, 1);


	RDA5807M_read(I2Cx, 0x0C, (uint16_t *) &RDS_BlockA, 1);
	RDA5807M_read(I2Cx, 0x0D, (uint16_t *) &RDS_BlockB, 1);
	RDA5807M_read(I2Cx, 0x0E, (uint16_t *) &RDS_BlockC, 1);
	RDA5807M_read(I2Cx, 0x0F, (uint16_t *) &RDS_BlockD, 1);
	//czeksuma
		/*if(r0B.BLERB ==3 || r0B.BLERA ==3){
			return false;
		}
		if(r0B.BLERB ==2 || r0B.BLERA ==2){
					return false;
				}
		if(r0B.BLERB ==1 || r0B.BLERA ==1){
							return false;
						}*/
	if(r0B.BLERB >0 || r0B.BLERA >0){
						return false;
	}
	//amuskezc
	if(!r0A.RDS_Ready){
		return false;
	}
	//RDA5807M_resetRDS_settings(I2Cx); //reset RDSA
	if(r0A.RDS_Ready){
		if(!r0B.ABCD_E){

			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13,1); //tu weszlismy
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12,1);
			RDA5807M_processRDSmkV();
		}
	}
	return true;
}
void RDA5807M_processRDSmkII(I2C_HandleTypeDef *I2Cx){
	uint8_t offs;
	char c1,c2;

	switch(RDS_BlockB & 0xF800){ //rds group było && zamiast &
		case 0x0000 : //group A
				break;
		case 0x0800 : //group B

				offs=(RDS_BlockB & 0x03) << 1;
				 c1 = (char)(RDS_BlockD >> 8);
				 c2 = (char)(RDS_BlockD & 0xFF);

				if (StationName[offs] == c1) {
								ActualStationName[offs] = c1;
								HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15,1);
								 hasStationName =1;
							} else {
								StationName[offs] = c1;
							}

							if (StationName[offs + 1] == c2) {
								ActualStationName[offs + 1] = c2;
								 hasStationName =1;
								HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15,1);
							} else {
								StationName[offs + 1] = c2;
							}

				break;
	}
	/*LCD1602_setCursor(1,7);
		LCD1602_print(c1);
		LCD1602_setCursor(1,9);
				LCD1602_print(c2);*/
}
char* RDA5807M_getStationName(){

	char* tmp;
	tmp=malloc(sizeof (char)*8);
	tmp=StationName;
	/*if(hasStationName == 1){
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14,0);
		return tmp;

	}else{
		return "niedzia";
	}*/
	return ActualStationName;
}
//------------------------------------------------------------------------------------------------------------------------------
void RDA5807M_resetRDS(){ //reset zmiennych RDSowych
	 strcpy(PSName1, "        ");
	 strcpy(PSName2, PSName1);
	 strcpy(StationName, "        ");
	 memset(RDSmessage, '\0', sizeof(RDSmessage));
	 lastMessage = 0;
	 RadReady = 0;
	 strcpy(ActualStationName , "RadPogod");
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
	RDA5807M_resetRDS();
}


void RDA5807M_processRDSmkIII(){//I2C_HandleTypeDef *I2Cx
	  // DEBUG_FUNC0("process");
	  uint8_t  idx; // index of rdsText
	  char c1, c2;
	  char *p;
	  char jarek[50];
	  uint16_t mins; ///< RDS time in minutes
	  uint8_t off;   ///< RDS time offset and sign

	  // Serial.print('('); Serial.print(block1, HEX); Serial.print(' '); Serial.print(block2, HEX); Serial.print(' '); Serial.print(block3, HEX); Serial.print(' '); Serial.println(block4, HEX);

	  if (RDS_BlockA == 0) {
	    // reset all the RDS info.
		  RDA5807M_resetRDS();
	    // Send out empty data
			 /* LCD1602_setCursor(1,6);
			  		LCD1602_print(StationName);*/
	  //  if (_sendServiceName) _sendServiceName(programServiceName);
	  //  if (_sendText)        _sendText("");
	  //  return;
	  } // if

	  // analyzing Block 2
	  rdsGroupType = 0x0A | ((RDS_BlockB & 0xF000) >> 8) | ((RDS_BlockB & 0x0800) >> 11);
	  rdsTP = (RDS_BlockB & 0x0400);
	  rdsPTY = (RDS_BlockB & 0x0400);
	 // LCD1602_setCursor(1,10);
	  char* cstam;
	  char adrian[10];

	  sprintf(cstam,"%d",rdsGroupType);
	  	         //LCD1602_print(cstam);
	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14,1);
	  switch (rdsGroupType) {
	  case 0x0A:
	  case 0x0B:
	    // The data received is part of the Service Station Name
	    idx = 2 * (RDS_BlockB & 0x0003);
	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15,1);
	    // new data is 2 chars from block 4
	    c1 = RDS_BlockD >> 8;
	    c2 = RDS_BlockD & 0x00FF;

	    // check that the data was received successfully twice
	    // before publishing the station name
	    if(ich<49){
	   	    	  HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
	   	      jarek[ich]=c1;
	   	      jarek[ich+1]=c2;
	   	      ich+=2;}
	   	      if(ich>30){
	   	    	  delay(1);
	   	      }

	    if ((PSName1[idx] == c1) && (PSName1[idx + 1] == c2)) {
	      // retrieved the text a second time: store to _PSName2
	      PSName2[idx] = c1;
	      PSName2[idx + 1] = c2;
	      PSName2[8] = '\0';

	      if ((idx == 6) && strcmp(PSName1, PSName2) == 0) {
	        if (strcmp(PSName2, StationName) != 0) {
	          // publish station name
	          strcpy(StationName, PSName2);
	          strcpy(adrian, PSName2);
	          //LCD1602_setCursor(1,7);
	         //LCD1602_print(StationName);
	         // if (_sendServiceName)
	            //_sendServiceName(programServiceName);
	        } // if
	      } // if
	    } // if

	    if ((PSName1[idx] != c1) || (PSName1[idx + 1] != c2)) {
	      PSName1[idx] = c1;
	      PSName1[idx + 1] = c2;
	      PSName1[8] = '\0';
	      // Serial.println(_PSName1);
	    } // if
	    break;

	  case 0x2A:
	    // The data received is part of the RDS Text.
	   textAB = (RDS_BlockB & 0x0010);
	    idx = 4 * (RDS_BlockB & 0x000F);

	    if (idx < lastTextIDX) {
	      // the existing text might be complete because the index is starting at the beginning again.
	      // now send it to the possible listener.
	      //if (_sendText)
	        //_sendText(_RDSText);
	    }
	    lastTextIDX = idx;

	    if (textAB != last_textAB) {
	      // when this bit is toggled the whole buffer should be cleared.
	      last_textAB = textAB;
	      memset(RDSText, 0, sizeof(RDSText));
	      // Serial.println("T>CLEAR");
	    } // if


	    // new data is 2 chars from block 3
	    RDSText[idx] = (RDS_BlockC >> 8);     idx++;
	    RDSText[idx] = (RDS_BlockC & 0x00FF); idx++;

	    // new data is 2 chars from block 4
	    RDSText[idx] = (RDS_BlockD >> 8);     idx++;
	    RDSText[idx] = (RDS_BlockD & 0x00FF); idx++;

	    // Serial.print(' '); Serial.println(_RDSText);
	    // Serial.print("T>"); Serial.println(_RDSText);
	    break;

	  case 0x4A:
	    // Clock time and date
	    off = (RDS_BlockD)& 0x3F; // 6 bits
	    mins = (RDS_BlockD >> 6) & 0x3F; // 6 bits
	    mins += 60 * (((RDS_BlockC & 0x0001) << 4) | ((RDS_BlockD >> 12) & 0x0F));

	    // adjust offset
	    if (off & 0x20) {
	      mins -= 30 * (off & 0x1F);
	    } else {
	      mins += 30 * (off & 0x1F);
	    }

	   // if ((sendTime) && (mins != lastRDSMinutes)) {
	     // lastRDSMinutes = mins;
	      //sendTime(mins / 60, mins % 60);
	    //} // if
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
	    // Serial.print("RDS_GRP:"); Serial.println(rdsGroupType, HEX);
	    break;
	  }
}
void RDA5807M_processRDSmkIV(){//unsigned short *rxData, char *stationTempBuffer, char *StationName

	unsigned short rdsB, rdsD;
	unsigned char offset, char1, char2;
	int pom;
	uint16_t pom2=0;
	pom2=RDS_BlockB;
	pom = RDS_BlockB & 0xF800;
	bool hasStationInfo = 0;
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
	//if((rxData[0] & RDA5807_HAS_RDS_INFO))
	//{
		// Extract RDS data blocks which need to display station name.
		//rdsB = rxData[3];
		//rdsD = rxData[5];

		switch(RDS_BlockB & 0xF800)
		{
			//case RDS_GROUP_A0:
			case 0x0800:
				// Station information extraction routine.

				//hasStationInfo = TRUE; /to tam niepotrzebne tak se jest

				offset = (RDS_BlockB & 0x03) << 1;
				char1 = (char)(RDS_BlockD >> 8);
				char2 = (char)(RDS_BlockD & 0xFF);
				HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
				// Check for valid array limits.
				if(offset >= 10)
				{
					HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
					//return 0;
				}

				// Verify the 1st bit with last received character.
				if (stationTempBuffer[offset] == char1)
				{
					StationName[offset] = char1;
				}
				else if((char1 >= 0x20) && (char1 <= 0x7D))
				{
					stationTempBuffer[offset] = char1;
				}

				// Verify the 2nd bit with last received character.
				if (stationTempBuffer[offset + 1] == char2)
				{
					StationName[offset + 1] = char2;
				}
				else if((char2 >= 0x20) && (char2 <= 0x7D))
				{
					stationTempBuffer[offset + 1] = char2;
				}

				break;
		}
	//}

	//return 1;
}
void RDA5807M_processRDSmkV(){

	unsigned short rdsB, rdsD;
	unsigned char offset, char1, char2;
	int pom;
	uint16_t pom3, pom4;
	pom3 = 0xffff;
	pom4 = pom3 >>11;
	uint16_t pom2=0;
	pom2=RDS_BlockB;
	pom = RDS_BlockB & 0xF800;
	bool hasStationInfo = 0;
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
	//if((rxData[0] & RDA5807_HAS_RDS_INFO))
	//{
		// Extract RDS data blocks which need to display station name.
		//rdsB = rxData[3];
		//rdsD = rxData[5];

	pom= RDS_BlockB >>10;
	if(pom == 0x0001){//0x0001
		//coś nie wiem

		offset = (RDS_BlockB & 0x03);//<<1
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
			//RadReady =0;
		}
		// Check for valid array limits.
		/*if(offset >= 10)
		{
			HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
			//return 0;
		}

		// Verify the 1st bit with last received character.
		if (stationTempBuffer[offset] == char1)
		{
			StationName[offset] = char1;
		}
		else if((char1 >= 0x20) && (char1 <= 0x7D))
		{
			stationTempBuffer[offset] = char1;
		}

		// Verify the 2nd bit with last received character.
		if (stationTempBuffer[offset + 1] == char2)
		{
			StationName[offset + 1] = char2;
		}
		else if((char2 >= 0x20) && (char2 <= 0x7D))
		{
			stationTempBuffer[offset + 1] = char2;
		}*/

	}
		/*switch(RDS_BlockB & 0xF800)
		{
			//case RDS_GROUP_A0:
			case 0x0800:
				// Station information extraction routine.

				//hasStationInfo = TRUE; /to tam niepotrzebne tak se jest

				offset = (RDS_BlockB & 0x03) << 1;
				char1 = (char)(RDS_BlockD >> 8);
				char2 = (char)(RDS_BlockD & 0xFF);
				HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
				// Check for valid array limits.
				if(offset >= 10)
				{
					HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
					//return 0;
				}

				// Verify the 1st bit with last received character.
				if (stationTempBuffer[offset] == char1)
				{
					StationName[offset] = char1;
				}
				else if((char1 >= 0x20) && (char1 <= 0x7D))
				{
					stationTempBuffer[offset] = char1;
				}

				// Verify the 2nd bit with last received character.
				if (stationTempBuffer[offset + 1] == char2)
				{
					StationName[offset + 1] = char2;
				}
				else if((char2 >= 0x20) && (char2 <= 0x7D))
				{
					stationTempBuffer[offset + 1] = char2;
				}

				break;
		}*/
	//}

	//return 1;
}
