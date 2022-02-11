// TEST OF EXTERNAL INTERRUPTS 
// ESP32 DEVKIT - ARDUINO IDE 1.8.5
// Gustavo Murta 18/02/2018
//https://www.flickr.com/photos/jgustavoam/25478286157/in/album-72157665409004158/
int GPIO4 = 4;      // SCOPE CH 1   
int GPIO5 = 5;      // SCOPE CH 2  

void setup()
{
  pinMode (GPIO4, INPUT);                 // Interrupt PIN - ESP32 GPIO_4
  pinMode (GPIO5, OUTPUT);                // Scope PIN - ESP32 GPIO_5

  // Generating 8 MHz Pulses - For test, attach GPIO_2 to GPIO_4
  pinMode(2, OUTPUT);                     // GPIO_2 as Output
  ledcAttachPin(2, 0);                    // GPIO_2 attached to PWM Channel 0
  ledcSetup(0, 8000000, 2);               // Channel 0 , freq 8 MHz , 2 bit resolution
  ledcWrite(0, 2);                        // Enable frequency with duty cycle 50%

  attachInterrupt(digitalPinToInterrupt(GPIO4), PrintData, RISING);
  
  Serial.begin(115200);
  Serial.println(" Monitoring Interrupts ");
}

void setGPIO_5 (){
  digitalWrite(GPIO5, LOW);        // Monitor GPIO_5 with SCOPE CH 2 
  digitalWrite(GPIO5, HIGH);
  //delayMicroseconds(1);          // change nothing
  digitalWrite(GPIO5, LOW);
}

void PrintData () {
  //Serial.println(" An interrupt has occurred. "); // For slow tests 
  setGPIO_5 ();
}



void loop() {

}
