/*
   FILE: gps1_5.c
   AUTH: P.OH   and Phil Wilshire
   DESC: Garmin EMap connected to COM1
   REFS: Uses ibmcom serial libraries
   NOTE: To compile: tcc -ml gps1_5.c ibmcom3.obj
   NOTE: To compile: gcc -o gps  gps1_5.c 
*/

/* Defines required for serial i/o */
#define COM_PORT   1		/* Serial device connected to COM 1 */
#define SPEED      4800		/* baud rate = 4800 */
#define CR         0x0d
#define LF         0x0a
#define ESC        0x1b
#define BEEP       0x07

/* Some helpful defines */
#define SPACE	  0x20
#define COMMA	  0x2C
#define MAXSIZE    100		/* GPS at most, sends 80 or so chars per message string.  So set maximum to 100 */

#include <stdio.h>
#include <ctype.h>  		/* required for the isalnum function */
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

//#include < conio.h >
//#include < math.h >
//#include < dos.h >
//#include "ibmcom3.h"  		/* for serial */

/* Prototypes */
int comm_setting(void);   	/* Set com port */
void close_comm(void);      	/* Close com port */
char comm_rx(void);

char *comm_device = NULL;
int comm_fd = 0;

int main(int argc, char * argv[]) 
{
  
  unsigned char  charRead;	     		/* char read from COM port */
  unsigned char	 stringRead[MAXSIZE]; 		/* Buffer collects chars read from GPS */
  unsigned char  tempString[MAXSIZE];
  unsigned char  timeString[12];
  unsigned char  latitudeString[11];
  unsigned char  latitudeCardinalString[3];
  unsigned char  longitudeString[12];
  unsigned char  longitudeCardinalString[3];

  unsigned char  *pChar;
  unsigned char  dummyChar;

  unsigned  long utcTime, estTime;		/* Coordinated Universal Time and Eastern Standard Time */
  unsigned  long utcHour, estHour;
  unsigned  long utcMinutes, estMinutes;
  unsigned  long utcSeconds, estSeconds;
  unsigned  char lastCommaPosition;

  float     	 latitude;
  int	    	 latDegrees;
  float	    	 latMinutes;

  float 	 longitude;
  int		 longDegrees;
  float		 longMinutes;

  FILE           *gpsFile;	     		/* Text file of GPS strings read */
  unsigned int   j, k;				/* dummy variable */
  unsigned int	 i;		     		/* Number of chars read per GPS message string */
  unsigned int	 numLinesRead;        		/* Number of GPS strings read */

  if (argc > 1)
    {
      comm_device = strdup(argv[1]);
      printf(" comm port [%s]\n", comm_device);
    }
  else
    {
      printf(" please supply a comm port arg\n");
      exit (0);
    }

  printf("Initializing port [%s] ...\n", comm_device);

  dummyChar = 'A'; pChar = &dummyChar;
  gpsFile = fopen("gpsData.txt", "w");

  printf("Initializing port...\n");
  comm_setting();
  printf("done\n");

  numLinesRead = 0;

  printf("Entering while loop...\n");
  do {
      
      charRead = comm_rx();  	/* read char from serial port */
      if(charRead == '$') {     /* GPS messages start with $ char */
	  i = 0;
	  numLinesRead++;
	  stringRead[i] = charRead;
	  do {
	     charRead = comm_rx();
	     if( (charRead != 0) && (isalnum(charRead) ||  isspace(charRead) || ispunct(charRead)) ) {
		i++;
		stringRead[i] = charRead;
	     }
	  } while(charRead != CR);

	  /* By this point, a complete GPS string has been read so save it to file */
	  /* Append the null terminator to the string read */
	  stringRead[i+1] = '\0';

	  /* Analyze string that we collected */
	  j = 0;
	  pChar = stringRead;
	  while(*(pChar+j) != COMMA) {
	       tempString[j] = *(pChar+j);
	       j++;
	  }
	  tempString[j] = 0;

	  /* Check if string we collected is the $GPGGA message */
	  if(tempString[3] == 'G' && tempString[4] == 'G' && tempString[5] == 'A') {
	      /*
		 Found GPGGA string.  It has 14 commas total.  Its NMEA sentence structure is:

		 $GPGAA,hhmmss.ss,ddmm.mmmm,n,dddmm.mmmm,e,q,ss,y.y,a.a,z,g.g,z,t.t,iii*CC
		 |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
		 0   	   1         2         3         4         5         6         7
		 0123456789012345678901234567890123456789012345678901234567890123456789012

		 where:

		 GPGAA		: GPS fixed data identifier
		 hhmmss.ss	: Coordinated Universal Time (UTC), also known as GMT
		 ddmm.mmmm,n	: Latitude in degrees, minutes and cardinal sign
		 dddmm.mmmm,e	: Longitude in degrees, minutes and cardinal sign
		 q		: Quality of fix.  1 = there is a fix
		 ss		: Number of satellites being used
		 y.y		: Horizontal dilution of precision
		 a.a,M		: GPS antenna altitude in meters
		 g.g,M		: geoidal separation in meters
		 t.t		: Age of the defferential correction data
		 iiii		: Deferential station's ID
		 *CC		: checksum for the sentence
	      */

	      pChar = stringRead;

	      /* Get UTC time */
	      j = 7;  /* start of time field */
	      k = 0;
	      while(*(pChar+j) != COMMA) {
		   timeString[k] = *(pChar+j);
		   j++;
		   k++;
	      }
	      lastCommaPosition = j;
	      timeString[k] = '\0';
	      sscanf(timeString, "%ld", &utcTime);
	      utcHour = (utcTime/10000);   /* extract Hours from long */
	      utcMinutes = (utcTime - (utcHour*10000))/100;  /* extract minutes from long */
	      utcSeconds = utcTime - (utcHour*10000) - (utcMinutes*100); /* extract seconds from long */

	      if(utcHour >= 4 && utcHour <= 23) estHour = utcHour - 4;
		else estHour = utcHour + 20;
	      estMinutes = utcMinutes;
	      estSeconds = utcSeconds;

	      /* NB: %02ld formats long to print 2 chars wide, padding with 0 if necessary */
	      printf("%02ld:%02ld:%02ld UTC = %02ld:%02ld:%02ld EST", utcHour, utcMinutes, utcSeconds, estHour, estMinutes, estSeconds);

	      /* Get lattitude: ddmm.mmmm */
	      pChar = stringRead;
	      j = lastCommaPosition + 1;
	      k = 0;
	      while(*(pChar+j) != COMMA) {
		   latitudeString[k] = *(pChar+j);
		   j++;
		   k++;
	      }
	      lastCommaPosition = j;
	      latitudeString[k] = '\0';

	      sscanf(latitudeString, "%f", &latitude);
	      latDegrees = (int)(latitude/100);
	      latMinutes = (float)(latitude - latDegrees*100);
	      printf("\t%02d DEG\t%2.4f MIN", latDegrees, latMinutes);

	      /* Get lattitude Cardinal direction */
	      pChar = stringRead;
	      j = lastCommaPosition + 1;
	      k = 0;
	      while(*(pChar+j) != COMMA) {
		   latitudeCardinalString[k] = *(pChar+j);
		   j++;
		   k++;
	      }
	      lastCommaPosition = j;
	      latitudeCardinalString[k] = '\0';
	      printf(" %s", latitudeCardinalString);

	      /* Get longitude: dddmm.mmmm */
	      pChar = stringRead;
	      j = lastCommaPosition + 1;
	      k = 0;
	      while(*(pChar+j) != COMMA) {
		   longitudeString[k] = *(pChar+j);
		   j++;
		   k++;
	      }
	      lastCommaPosition = j;
	      longitudeString[k] = '\0';

	      sscanf(longitudeString, "%f", &longitude);
	      longDegrees = (int)(longitude/100);
	      longMinutes = (float)(longitude - longDegrees*100);
	      printf("\t%03d DEG\t%2.4f MIN", longDegrees, longMinutes);

	      printf("/n");
	  } /* else not a GPGGA sentence */

	  fprintf(gpsFile, "%d: (%d) %s\n", numLinesRead, i, stringRead);

      } /* otherwise not a $ character... so loop back until one arrives */
  } while(1);

  printf("Exiting...");
  close_comm();   /* Finished with serial port so close it */
  fclose(gpsFile);
  printf("done/n");
  return (0);

} /* end of main */

int comm_setting(void) {

  comm_fd = open (comm_device,O_RDWR);
  if(comm_fd <= 0) {
    printf("Invalid comm_device [%s]\n", comm_device);
    //_exit(1);
  } 
  //com_raise_dtr();

  //com_set_speed(SPEED);
  //com_set_parity(COM_NONE, STOP_BIT_1);
}

void close_comm(void) {
  if(comm_fd  > 0) close(comm_fd);
  comm_fd = -1;
  //com_lower_dtr();
  //com_deinstall();
}

char comm_rx(void)
{
  char c;
  int rc;
  rc = read(comm_fd, &c, 1);
  if(rc < 1) 
    {
      printf(" unable to read port [%s]\n", comm_device);
      close(comm_fd);
      _exit(1);
    }
  return c;
}
