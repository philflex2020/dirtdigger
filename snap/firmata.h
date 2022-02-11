
// #! /usr/bin/env python



#define VERSION  "0.1"



// Message command bytes - straight outta Pd_firmware.pde
// send data for a digital pin
#define DIGITAL_MESSAGE 0x90 

//# send data for an analog pin (or PWM)
#define  ANALOG_MESSAGE 0xE0 

// proposed pulseIn/Out message (SysEx)
#define PULSE_MESSAGE 0xA0 

//# proposed shiftOut message (SysEx)
#define SHIFTOUT_MESSAGE 0xB0 

//# enable analog input by pin #
#define REPORT_ANALOG_PIN 0xC0 

//# enable digital input by port pair
#define REPORT_DIGITAL_PORTS 0xD0 

# start a MIDI SysEx message
#define START_SYSEX 0xF0 

//# set a digital pin to INPUT or OUTPUT
#define SET_DIGITAL_PIN_MODE 0xF4 

//# end a MIDI SysEx message
#define END_SYSEX 0xF7 

//# report firmware version
#define REPORT_VERSION 0xF9 

//# reset from MIDI
#define SYSTEM_RESET 0xFF 

//# Pin modes
#define DIGITAL_INPUT 0
#define DIGITAL_OUTPUT 1
#define DIGITAL_PWM 2

#include "array.h"

int PWM_PINS[] = {9, 10, 11};

class Firmata
{
  //"""Base class for the arduino board"""

  Firmata(int port){
    int i;
    //self.sp = serial.Serial(port, 57600, timeout=0.02)
    int sockfd = OpenPort(port);
    Array *digital = new Array (16,16, "digital");
    Array *analog = new Array (16,16, "analog");

    for (i = 0; i < 14; i++){
      digital->addItem(Digital(sockfd,i));
    }
    for (i = 0; i < 6; i++){
      analog->addItem(Analog(sockfd, i));
    }
    //#Obtain firmata version
    write(sockfd, REPORT_VERSION, strlen(REPORT_VERSION));
  };

  ~Firmata() {
    close (sockfd);
    Digital *dig = NULL;
    Analog *anal = NULL;
    int idx = 0;
    while(digital->findNext((void **)&dig, idx, 0)) {
      delete dig;
      dig = NULL;
    }
    idx = 0;
    while(analog->findNext((void **)&anal, idx, 0)) {
      delete anal;
      anal = NULL;
    }
    delete digital;
    delete analog;

  };
  //def __str__(self):
  //     return "Arduino: %s"% self.sp.port
  char data[1024];
  
  void iterate(void)
  {
    //        """Read and handle a command byte from Arduino's serial port"""
    unsigned char data[1024];
    int rc = read(sockfd, data, sizeof(data));
    int i;
    for ( i = 0 ;i < rc; i++ ) {
      i += _process_input(data[i], rc - i);
    }		     
  };

  void _process_input(unsigned char *data, int len)
  {
    //     """Process a command byte and any additional information bytes"""
    int idx = 0;
    if (data[idx] < 0xF0) {
      //#Multibyte
      message = data & 0xF0;
      pin = data & 0x0F;
      if (message == DIGITAL_MESSAGE ) {
	//#Digital in
	char lsb = data[++idx];
        char msb = data[++idx];
	
	//lsb = ord(lsb)
	//      msb = ord(msb)
	Digital.mask = lsb + (msb << 7);
      } else if (message == ANALOG_MESSAGE ) {
	//#Analog in
	char lsb = data[++idx];
        char msb = data[++idx];
        //        lsb = ord(lsb)
        //        msb = ord(msb)
	analog[pin].value = msb << 7 | lsb ;
      } else if (data == REPORT_VERSION ) {
	major = data[++idx]; 
	minor = data[++idx];
	firmata_version = major<<8 + minor;
      }
    }
    return idx;
  };

  int get_firmata_version(void){
    //"""Return a (major, minor) version tuple for the firmata firmware"""
    return firmata_version;
  };
  //    def exit(self):
  //    """Exit the application cleanly"""
  //    self.sp.close()
};

class Digital {
  //    """Digital pin on the arduino board"""
  
  static int Cmask = 0;
  
  Digital(int sp, int pin) {
    sp(sp);
    pin (pin);
    is_active = 0;
    value = 0;
    mode = DIGITAL_INPUT;
  };
  
  //def __str__(self):
  //    return "Digital Port %i"% self.pin
  
  void set_active(int active) {
    //        """Set the pin to report values"""
    is_active = 1;
    pin = REPORT_DIGITAL_PORTS + pin; 
    write (sp,pin, 1);
    write (sp,active, 1);
  };
  
  int get_active(void) {
    //  """Return whether the pin is reporting values"""
    return is_active;
  };
  
  int  set_mode(int mode) {
    //"""Set the mode of operation for the pin
    
    //  Argument:
    //    mode, takes a value of: - DIGITAL_INPUT
    // DIGITAL_OUTPUT 
    //                            - DIGITAL_PWM
    //    """
    if ((mode == DIGITAL_PWM) && pin_not_in (pin, PWM_PINS)) {
      //error_message = "Digital pin %i does not have PWM capabilities" \
      //                  % (self.pin)
      //raise IOError, error_message
      return -1;
    }
    if (pin < 2 ) {
      //raise IOError, "Cannot set mode for Rx/Tx pins" 
      return -1;
    }
    mode = mode;
    command[0] = SET_DIGITAL_PIN_MODE;
    command[1] =pin;
    command[2] =mode;
    write(sp, command , 3);
  };
  
  int get_mode(void) {
    //"""Return the pin mode, values explained in set_mode()"""
    return mode;
  };
  int  read(void) {
    //"""Return the output value of the pin, values explained in write()"""
    if (mode == DIGITAL_PWM ) {
      return value;
    } else {
      return ((mask & 1 << pin) > 0);
    }
  };

  int write(int value) {
    //
    //  """Output a voltage from the pin
    //
    //Argument:
    //value, takes a boolean if the pin is in output mode, or a value from 0
    //to 255 if the pin is in PWM mode
    
    //"""
    if (mode == DIGITAL_INPUT) {
      //error_message = "Digital pin %i is not an output"% self.pin
      //raise IOError, error_message
      return -1;
    } else if  (value != read()) {
      if (mode == DIGITAL_OUTPUT) {
	//        #Shorter variable dammit!
	mask = Cmask;
	mask ^= 1 << pin;
	message[0] = DIGITAL_MESSAGE; 
	message[1]= mask % 128;		
	message[2]= mask >> 7;
	rc = write(sp, message, 3);
	//#Set the attribute to the new mask
	Cmask = mask;
      } else if (mode == DIGITAL_PWM ) {
	value = value;
	pin = ANALOG_MESSAGE + pin;
	message[0] = pin; 
	message[1]= value % 128;		
	message[2]= value >> 7;
	write(sp, message, 3);
      }
    }
    return 0;
  };

  unsigned char message[3];
  int sp;
  int pin;
  int active;
  int value;
  int mode;
  int mask;

};

class Analog {
  //"""Analog pin on the arduino board"""

  Analog(int sp, int pin) {
    sp(sp);
    pin (pin);
    active = 0;
    value = 0;
    mode = ANALOG_INPUT;
  };

  //    def __str__(self):
  //    return "Analog Input %i"% self.pin
  
  void set_active(int xactive) {
    //"""Set the pin to report values"""
    active = xactive;
    pin = REPORT_ANALOG_PIN + pin;
    message[0] = pin;
    message[1] = active;

    write(sp, message, 2);
  };

  int get_active(void) {
    //"""Return whether the pin is reporting values"""
    return active;
  };
  int read(void) {
    //"""Return the input in the range 0-1024"""
    return value;
  };

  unsigned char message[3];
  int sp;
  int pin;
  int active;
  int value;
  int mode;

};


	
	
	
