// TEST OF EXTERNAL INTERRUPTS 
// ESP32 DEVKIT - ARDUINO IDE 1.8.5
// Gustavo Murta 18/02/2018

int GPIO2 = 2;      // Output pulse   
int GPIO4 = 4;      // SCOPE CH 1   
int GPIO5 = 5;      // SCOPE CH 2   
int GPIO12 = 12;      // SCOPE CH 2  
int GPIO22 = 22;
int GPIO23 = 23;

void setup()
{
  pinMode (GPIO4, INPUT);                 // Interrupt PIN - ESP32 GPIO_4
  pinMode (GPIO2, OUTPUT);                // Scope PIN - ESP32 GPIO_12
  pinMode (GPIO12, OUTPUT);                // Scope PIN - ESP32 GPIO_5
  pinMode (GPIO22, OUTPUT);                // Scope PIN - ESP32 GPIO_5
  pinMode (GPIO23, OUTPUT);                // Scope PIN - ESP32 GPIO_5

  // Generating 8 MHz Pulses - For test, attach GPIO_2 to GPIO_4
  pinMode(GPIO5, OUTPUT);                     // GPIO_2 as Output
  ledcAttachPin(GPIO5, 0);                    // GPIO_2 attached to PWM Channel 0
  //ledcSetup(0, 8000000, 2);               // Channel 0 , freq 8 MHz , 2 bit resolution
  ledcSetup(0, 200000, 2);               // Channel 0 , freq 200 Hz , 2 bit resolution
  ledcWrite(0, 2);                        // Enable frequency with duty cycle 50%

  //attachInterrupt(digitalPinToInterrupt(GPIO4), setGPIO_12, RISING);
  attachInterrupt(digitalPinToInterrupt(GPIO5), setGPIO_12, FALLING|RISING);
  
  Serial.begin(115200);
  Serial.println(" esp32Fastirq");
  Serial.println(" Monitoring Interrupts ");
}
int irqs = 0;

void  setGPIO_12 ()
{
  //irqs++;
  //if (irqs > 100000) {

  //Serial.print(".");
  //irqs = 0;    
  //}
  //digitalWrite(GPIO23, LOW);        // Monitor GPIO_5 with SCOPE CH 2 
  digitalWrite(GPIO23, HIGH);
  //digitalWrite(GPIO5, LOW);        // Monitor GPIO_5 with SCOPE CH 2 
  //digitalWrite(GPIO5, HIGH);
  //delayMicroseconds(1);          // change nothing
  digitalWrite(GPIO23, LOW);
  //digitalWrite(GPIO5, LOW);        // Monitor GPIO_5 with SCOPE CH 2 
}

void PrintData ()
{
  //Serial.println(" An interrupt has occurred. "); // For slow tests 
  setGPIO_12 ();
}


void loop()
{
  //Serial.println(" blink "); // For slow tests 
  digitalWrite(GPIO22, LOW);        // Monitor GPIO_5 with SCOPE CH 2 
  //digitalWrite(GPIO5, LOW);        // Monitor GPIO_5 with SCOPE CH 2 
  //delay(1);
  digitalWrite(GPIO22, HIGH);        // Monitor GPIO_5 with SCOPE CH 2 
  //digitalWrite(GPIO5, HIGH);        // Monitor GPIO_5 with SCOPE CH 2 
  //delay(1);
}

