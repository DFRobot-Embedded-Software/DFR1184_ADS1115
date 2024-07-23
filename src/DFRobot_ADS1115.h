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

#ifndef __DFRobot_ADS1115_H
#define __DFRobot_ADS1115_H

#include "Arduino.h"
#include "Wire.h"

#if defined(ARDUINO_AVR_UNO) || defined(ESP8266)
#include "SoftwareSerial.h"
#else
#include "HardwareSerial.h"
#endif


class DFRobot_ADS1115_UART{
public:
  bool begin(void);
private:
  uint32_t _baud;
  // uint8_t _rxpin;
  // uint8_t _txpin;
public:
#if defined(ARDUINO_AVR_UNO) || defined(ESP8266)
  DFRobot_ADS1115_UART(SoftwareSerial* sSerial, uint32_t Baud);
  SoftwareSerial* mySerial;
#else
  DFRobot_ADS1115_UART(HardwareSerial* hSerial, uint32_t Baud, uint8_t rxpin = 0, uint8_t txpin = 0);
  HardwareSerial* mySerial;
#endif
uint32_t  get_value(uint8_t channel);
~DFRobot_ADS1115_UART() {};
};


class DFRobot_ADS1115_I2C{
public:
  DFRobot_ADS1115_I2C(TwoWire* Wire, uint8_t MODULE_I2C_ADDRESS);
   ~DFRobot_ADS1115_I2C() {};
  bool begin(void);
  uint32_t  get_value(uint8_t channel);
private:
	#define CHANNEL_SELECT_ADDRESS  0x20 //写选择ad通道寄存器地址
	#define CHANNEL_DATA_ADDRESS  0x30 //读ad数据地址 30数据位 31-33 数据
	TwoWire* mywire;
   uint8_t deviceAddr;
protected:
  virtual void writeReg(uint8_t _addr,uint8_t Reg ,uint8_t data);
  virtual void readReg(uint8_t _addr ,uint8_t Reg ,uint8_t *data ,uint8_t datalen);   

};

#endif