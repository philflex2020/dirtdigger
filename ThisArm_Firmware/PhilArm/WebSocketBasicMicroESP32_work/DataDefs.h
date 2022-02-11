#ifndef _DATA_DEFS
#define _DATA_DEFS

//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


//UART2  GPIO16  GPIO17  GPIO8 GPIO7 conflict

//SX1276 LoRa  ESP32
//MISO  GPIO 19
//MOSI  GPIO 27
//SCK GPIO 5
//CS  GPIO 18
//IRQ GPIO 26
//RST GPIO 14


//SX1276 LoRa  ESP32
//MISO  GPIO 19
//MOSI  GPIO 27
//SCK GPIO 5
//CS  GPIO 18
//IRQ GPIO 26
//RST GPIO 14
// Serial.begin(115200);
//  Serial1.begin(115200,SERIAL_8N1, 4, 2);    //Baud rate, parity mode, RX, TX
//  Serial2.begin(115200,SERIAL_8N1, 16, 17);
//  Serial1.begin(115200,SERIAL_8N1, 34, 35);    //Baud rate, parity mode, RX, TX

// ttgo lora32-oled v1
#define ADC1_0  36
#define ADC1_1  37
#define ADC1_2  38
#define ADC1_3  39
//     GND -                                    - GND
//     5v                                       - 5v5
//     3v3                                      - 3v3
//     adc1_0 - 36                              - GND
//     adc1_1 - 37                              3 RX
//     adc1_2 - 38                              1 TX
//     adc1_3 - 39                                rst
//rcrx adc1_6 - 34                              0 - btn
//     adc1_7 - 35                             22 - SCL
//     adc1_4 - 32 xtal                        19 - LORA_MISO
//     adc1_5 - 33 xtal                        23 -
//     dac2   - 25                             18 - LORA_CS
//dac1 LORAirq- 26                              5 - LORA_SCLK
//     LORMOSI- 27                             15 - OLED_SCL
//     LORARST- 14                              2 - LED
//     adc2_5 - 12 Rc S2  Rx                    4 - OLED_SDA
//     adc2_4 - 13 Rc S1  Tx                   17 - 
//     SDA    - 21                             16 - OLED_RST


// espwroom32 this may not work..
//     En  3 -                                    - 37 gpio23
//  GPI36  4 -                                    - 36 gpio22
//  GPI39  5 -                                    - 35 gpio1
//  GPI34  6 -                                    - 34 gpio3
//  GPI35  7 -                                    - 33 gpio21
//  GPO32  8 -                                    - 31 gpio19
//  GPO33  9 -                                    - 30 gpio18
// GPIO25 10 -                                    - 29 gpio5
// GPIO26 11 -                                    - 28 gpio17
// GPIO27 12 -                                    - 27 gpio16
// GPIO14 13 -                                    - 26 gpio4
// GPIO12 14 -                                    - 24 gpio2
// GPIO13 16 -                                    - 23 gpio15
//   GND   1                                      - 38  GND
//       Vin                                       - 3.3 v

//1-Respiratory Rate (RR) range   of 8-40Bpm 
//2-inspiratory /expiration (I/E) Ratio     range 1:1- 1:4
//3-Tidal Volume (TV) range    200-800ml        needed volume is 6 -8ml/kg of persons mass
//A test on my setup  showed 800ml at full stroke varies slightly depending on what brand BVM used
//so I think we need to make a calibration feature for others to accommodate their BVM

//Hi Phil, That sounds excellent.On a another note i did some pressure testing yesterday using a manometer and found 
//that one of the 3 BVMs I bought, had a child over pressure relieve valve  instead of an  adult one. 
//40cmh20 vs. 60cmh20 .
//That makes a good argument for a pressure sensor input for the controller. 
//Have you looked at the MIT E-vent code?

#endif
