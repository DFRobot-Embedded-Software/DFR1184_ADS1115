#include <Wire.h>
#include <DFRobot_ADS1115.h>
#define _addr 0x48                     // 从机iic设备号
uint8_t rxbuf[200]   = {0x00} ;       //接收缓存区
uint8_t version = 0;                // 版本长度
uint8_t mode_cmd=0;                //模式
uint8_t star_flag=0;       //开始标志位
uint32_t ad_value=0;
/*  @param  Reg_addr: 寄存器地址
 *  @param  data:     写入的数据
 */
void iic_wirte(uint8_t Reg ,uint8_t data){
  Wire.beginTransmission(_addr);                 // transmit to device Address
  Wire.write(Reg);                               // sends one byte
  Wire.write(data);
  Wire.endTransmission();
}
/*  @param  Reg_addr: 寄存器地址
 *  @param      data: 读取的数据
 *           datalen：数据长度
 */
void iic_read(uint8_t Reg ,uint8_t *data ,uint8_t datalen){
  uint8_t i = 0;
  Wire.beginTransmission(_addr);                 // transmit to device Address
  Wire.write(Reg);                               // sends one byte
  Wire.endTransmission();
  Wire.requestFrom(_addr, datalen);
  while (Wire.available())                       // slave may send less than requested
    data[i++] = Wire.read();
}


void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.beginTransmission(_addr);
  while(Wire.endTransmission() != 0) {
    Serial.println("i2c connect error");
    delay(1000);
  }
  Serial.println("i2c connect success");
//  //读取版本号
  iic_wirte(0x30,5);
  iic_read(0x30 ,&rxbuf[0] ,4);
//  version = rxbuf[0];
  Serial.print(rxbuf[0]);
   Serial.print(" ");
  Serial.print(rxbuf[1]);
   Serial.print(" ");
  Serial.print(rxbuf[2]);
  Serial.print(" ");
  Serial.print(rxbuf[3]);
  Serial.print(" ");
  ad_value=(rxbuf[1]<<16)|(rxbuf[2]<<8)|rxbuf[3];
  Serial.print(ad_value,DEC);
  
//  iic_read(51 ,&rxbuf[0] ,version);
//  for(uint8_t j = 0; j < version; j++) {
//    Serial.print((char)rxbuf[j]);
//  } Serial.println();

}

void loop(){
 
//  iic_read(20 ,&rxbuf[0] ,1);
//  Serial.print(rxbuf[0]);
//  version = rxbuf[0];
//  iic_read(71 ,&rxbuf[0] ,version);
//  for(uint8_t j = 0; j < version; j++) {
//    Serial.print((char)rxbuf[j]);
//  } 
//  Serial.println("!");
  delay(1000);//特别小心延时！！！

//  iic_wirte(0x20,1);
  iic_read(0x30 ,&rxbuf[0] ,4);
//  version = rxbuf[0];
  Serial.print(rxbuf[0]);
   Serial.print(" ");
  Serial.print(rxbuf[1]);
   Serial.print(" ");
  Serial.print(rxbuf[2]);
  Serial.print(" ");
  Serial.print(rxbuf[3]);
  Serial.print(" ");
  ad_value=(rxbuf[1]<<16)|(rxbuf[2]<<8)|rxbuf[3];
   Serial.println(ad_value,DEC);
  



}
