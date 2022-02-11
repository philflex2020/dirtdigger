
//Function for initializate stages
void DefaultInit(){
  servoSelected=0;
  
}

void EncoderRawControl_Init(){
  DefaultInit();
  for (int i = 0; i < NumOfServo; i++){
    AngularServos[i].value = 1500;
    AngularServos[i].idle = 0;
    
  }
}

void EncoderAngleControl_Init(){
  DefaultInit();
  for (int i = 0; i < NumOfServo; i++){
    AngularServos[i].Angle = AngularServos[i].INITANGLE;
    AngularServos[i].idle = 0;
    
  }
}

void EncoderLogicAngleControl_Init(){
  DefaultInit();
  //JointControl
  BaseAngle =90;
  Shoulder=90;
  Elbow=90;
  Claw=45;
}

void EncoderARHControl_Init (){
  DefaultInit();
  A=90;
  R=80;
  H=0;
  Claw=45; 
}


void EncoderXYZControl_Init (){
  DefaultInit();
  Claw=45; 
  X=00;
  Y=80;
  Z=0;
}
