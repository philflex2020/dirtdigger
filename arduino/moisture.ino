initialize serial communication at 9600 bits per second:
  pinMode(2,INPUT);
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  // read digital on D2
  int digin = (digitalRead(2) * 500 ) + 500;
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  // print out the value you read:
   Serial.print(500);
   Serial.print(",");
   Serial.print(1500);
   Serial.print(",");
   Serial.print(digin);
   Serial.print(",");
   Serial.println(sensorValue);
  delay(1);        // delay in between reads for stability
}
