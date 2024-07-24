/*!
 * @file DFRobot_ADS1115.h
 * @brief 
 * @details 
 * @copyright	Copyright (c) 2021 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author LR
 * @version V1.0
 * @date 2024-07-19
 * @url https://github.com/DFRobot-Embedded-Software/DFR1184_ADS1115.git
 */
#include"DFRobot_ADS1115.h"

#if defined(ARDUINO_AVR_UNO) || defined(ESP8266)
DFRobot_ADS1115_UART::DFRobot_ADS1115_UART(SoftwareSerial* sSerial, uint32_t Baud)
{
	mySerial = sSerial;
	_baud=Baud;
}
#else
DFRobot_ADS1115_UART::DFRobot_ADS1115_UART(HardwareSerial* hSerial, uint32_t Baud, uint8_t txpin, uint8_t rxpin)
{
	mySerial = hSerial;
	_baud=Baud;
	_txpin = txpin;
	_rxpin = rxpin;
}
#endif

bool DFRobot_ADS1115_UART::begin(void)
{
#ifdef ESP32
  mySerial->begin(_baud, SERIAL_8N1, _txpin, _rxpin);
  // #elif defined(ARDUINO_AVR_UNO) || defined(ESP8266)
    // nothing use software
#else
  mySerial->begin(_baud);  // M0 cannot create a begin in a construct
#endif
	return 1;
}


uint32_t DFRobot_ADS1115_UART:: get_value(uint8_t channel)
{
  uint8_t rxbuf[5];
  unsigned long int ad_value=0;
  static uint8_t i=0;
  mySerial->write(channel);
  uint32_t nowtime = millis();
  while (millis() - nowtime < 200) 
  {
    while (mySerial->available()) {
      rxbuf[i++] = mySerial->read();
      if(i>=3)break;
    } 
    if (i == 3) {
         i=0;
         ad_value=rxbuf[1];
         ad_value=rxbuf[0]*65536+(ad_value*256)+rxbuf[2];
		 // ad_value=(uint32_t)(rxbuf[0]<<16)|(uint32_t)(rxbuf[1]<<8)|rxbuf[2];
      break;
    }
  }
  delay(20);
  return ad_value;
}




DFRobot_ADS1115_I2C::DFRobot_ADS1115_I2C(TwoWire* Wire, uint8_t MODULE_I2C_ADDRESS)
{
	mywire=Wire;
	deviceAddr=MODULE_I2C_ADDRESS;
}

bool DFRobot_ADS1115_I2C::begin(void){
	mywire->begin();
	mywire->beginTransmission(deviceAddr);
	if(mywire->endTransmission())
		return 0;
	return 1;	
}

uint32_t DFRobot_ADS1115_I2C:: get_value(uint8_t channel)
{
	uint8_t rxbuf[4];
	uint32_t ad_value=0;
	writeReg(deviceAddr,CHANNEL_SELECT_ADDRESS,channel);
	readReg(deviceAddr,CHANNEL_DATA_ADDRESS ,&rxbuf[0] ,4);
	// ad_value=(uint32_t)(rxbuf[1]<<16)|(uint32_t)(rxbuf[2]<<8)|rxbuf[3];
	ad_value=rxbuf[2];
    ad_value=rxbuf[1]*65536+(ad_value*256)+rxbuf[3];
	delay(20);
	return ad_value;
}

/*  @param  Reg_addr: 寄存器地址
 *  @param  data:     写入的数据
 */
void DFRobot_ADS1115_I2C:: writeReg(uint8_t _addr,uint8_t Reg ,uint8_t data)
{
	 Wire.beginTransmission(_addr);                 // transmit to device Address
	 Wire.write(Reg);                               // sends one byte
	 Wire.write(data);
	 Wire.endTransmission();
}
/*  @param  Reg_addr: 寄存器地址
 *  @param      data: 读取的数据
 *           datalen：数据长度
 */
void DFRobot_ADS1115_I2C:: readReg(uint8_t _addr ,uint8_t Reg ,uint8_t *data ,uint8_t datalen)
{
	uint8_t i = 0;
	Wire.beginTransmission(_addr);                 // transmit to device Address
	Wire.write(Reg);                               // sends one byte
	Wire.endTransmission();
	Wire.requestFrom(_addr, datalen);
	while (Wire.available())                       // slave may send less than requested
		data[i++] = Wire.read();
}

