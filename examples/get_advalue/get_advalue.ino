#include <DFRobot_ADS1115.h>

//use I2C for communication, but use the serial port for communication if the line of codes were masked
//#define I2C_COMMUNICATION 

#define MODULE_I2C_ADDRESS 0x48
#ifdef  I2C_COMMUNICATION
  DFRobot_ADS1115_I2C ads1115(&Wire, MODULE_I2C_ADDRESS);
 /* ---------------------------------------------------------------------------------------------------------------------
  *    board   |             MCU                | Leonardo/Mega2560/M0 |    UNO    | ESP8266 | ESP32 |  microbit  |   m0  |
  *     VCC    |            3.3V/5V             |        VCC           |    VCC    |   VCC   |  VCC  |     X      |  vcc  |
  *     GND    |              GND               |        GND           |    GND    |   GND   |  GND  |     X      |  gnd  |
  *     RX     |              TX                |     Serial1 TX1      |     5     |   5/D6  | 25/D2 |     X      |  tx1  |
  *     TX     |              RX                |     Serial1 RX1      |     4     |   4/D7  | 26/D3 |     X      |  rx1  |
  * ----------------------------------------------------------------------------------------------------------------------*/

#elif defined(ARDUINO_AVR_UNO) || defined(ESP8266)
  SoftwareSerial mySerial1(4, 5); 
  DFRobot_ADS1115_UART ads1115(&mySerial1);
#elif defined(ESP32)
  DFRobot_ADS1115_UART ads1115(&Serial1,/*rxD2*/25,/*txD3*/26);
#else
  DFRobot_ADS1115_UART ads1115(&Serial1);
#endif
 
void setup() {
    Serial.begin(9600);
    Serial.println("star!");
    while (!ads1115.begin())
    {
      Serial.println("EEROR!");
      delay(1000);  
    }  
}

void loop() {
  double data;
  char buffer1[30];
  char buffer2[6];
  unsigned char channel = 2;
  data= ads1115.get_value(channel);
  dtostrf(data, 6, 2, buffer2); // 6是总宽度，2是小数位数  arduino uno 不能直接使用sprintf输出小数
  sprintf(buffer1, "channel%d adValue:%s mv",channel,buffer2);
  Serial.println(buffer1);
  delay(100);
}
