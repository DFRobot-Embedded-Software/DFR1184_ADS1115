#include <DFRobot_ADS1115.h>

#define I2C_COMMUNICATION  //use I2C for communication, but use the serial port for communication if the line of codes were masked

#define UART_BAUDRATE 9600
#define MODULE_I2C_ADDRESS 0x48

#ifdef  I2C_COMMUNICATION
  DFRobot_ADS1115_I2C ads1115(&Wire, MODULE_I2C_ADDRESS);
 /* ---------------------------------------------------------------------------------------------------------------------
  *    board   |             MCU                | Leonardo/Mega2560    |    UNO    |  ESP32 |  microbit  |
  *     VCC    |            3.3V/5V             |        VCC           |    VCC    |   VCC  |     X      |
  *     GND    |              GND               |        GND           |    GND    |   GND  |     X      |
  *     RX     |              TX                |     Serial1 TX1      |     5     |   D2   |     X      |
  *     TX     |              RX                |     Serial1 RX1      |     4     |   D3   |     X      |
  * ----------------------------------------------------------------------------------------------------------------------*/
  /* Baud rate cannot be changed */

#elif defined(ARDUINO_AVR_UNO) || defined(ESP8266)
  SoftwareSerial mySerial1(4, 5); 
  DFRobot_ADS1115_UART ads1115(&mySerial1, UART_BAUDRATE);
#elif defined(ESP32)
  DFRobot_ADS1115_UART ads1115(&Serial1, UART_BAUDRATE,/*rx*/D2,/*tx*/D3);
#else
  DFRobot_ADS1115_UART ads1115(&Serial1, UART_BAUDRATE);
#endif
 
void setup() {
    Serial.begin(9600);
    Serial.println("star!");
    while (!ads1115.begin())
    {
      Serial.println("EEROR!");
      delay(1000);  
    }  
    Serial.println("ok!"); 
}

void loop() {
  unsigned long int data;
  data= ads1115.get_value(1);
  Serial.println(data,DEC);
  delay(100);
}
