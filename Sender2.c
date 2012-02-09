/**************************************************

file: main.c
purpose: simple demo that receives characters from
the serial port and print them on the screen

**************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "rs232mod.h"
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

int Cport[28],
    error;

struct termios new_port_settings,
       old_port_settings[28];

char comports[28][13]={"/dev/ttyS0","/dev/ttyS1","/dev/ttyS2","/dev/ttyS3","/dev/ttyS4","/dev/ttyS5",
                       "/dev/ttyS6","/dev/ttyS7","/dev/ttyS8","/dev/ttyS9","/dev/ttyS10","/dev/ttyS11",
                       "/dev/ttyS12","/dev/ttyS13","/dev/ttyS14","/dev/ttyS15","/dev/ttyUSB0",
                       "/dev/ttyUSB1","/dev/ttyUSB2","/dev/ttyUSB3","/dev/ttyUSB4","/dev/ttyUSB5",
		       "/dev/ttyACM0","/dev/ttyACM1","/dev/ttyACM2","/dev/ttyACM3","/dev/ttyACM4",
		       "/dev/ttyACM5"};


int main(int argc, char *argv[])
{
  if(argc<3)
  {
    printf("Usage:\n Sender [port number] [gcode file]\n");
    printf("Port number\t Port name \tPort number\t Port name\n");
    printf("   0\t\t /dev/ttyS0 \t   14\t\t /dev/ttyS14\n");
    printf("   1\t\t /dev/ttyS1 \t   15\t\t /dev/ttyS15\n");
    printf("   2\t\t /dev/ttyS2 \t   16\t\t /dev/ttyUSB0\n");
    printf("   3\t\t /dev/ttyS3 \t   17\t\t /dev/ttyUSB1\n");
    printf("   4\t\t /dev/ttyS4 \t   18\t\t /dev/ttyUSB2\n");
    printf("   5\t\t /dev/ttyS5 \t   19\t\t /dev/ttyUSB3\n");
    printf("   6\t\t /dev/ttyS6 \t   20\t\t /dev/ttyUSB4\n");
    printf("   7\t\t /dev/ttyS7 \t   21\t\t /dev/ttyUSB5\n");
    printf("   8\t\t /dev/ttyS8 \t   22\t\t /dev/ttyACM0\n");
    printf("   9\t\t /dev/ttyS9 \t   23\t\t /dev/ttyACM1\n");
    printf("   10\t\t /dev/ttyS10 \t   24\t\t /dev/ttyACM2\n");
    printf("   11\t\t /dev/ttyS11 \t   25\t\t /dev/ttyACM3\n");
    printf("   12\t\t /dev/ttyS12 \t   26\t\t /dev/ttyACM4\n");
    printf("   13\t\t /dev/ttyS13 \t   27\t\t /dev/ttyACM5\n\n");
  }
  else
  {
    int i, n=0,
      cport_nr,        /* /dev/ttyACM0 (COM1 on windows) */
      bdrate=9600;       /* 9600 baud */
    cport_nr=atoi(argv[1]);
    unsigned char buf[4096];
    unsigned char line[100];
    FILE *fdCode;
    fdCode=fopen(argv[2],"r");
    if(OpenComport(cport_nr, bdrate))
    {
      printf("Can not open comport\n");
      return(0);
    }
    while(n==0)
    {
      n = PollComport(cport_nr, buf, 4095);
      usleep(100000);  /* sleep for 100 milliSeconds */
    }
    //printf("Success\n");
    while(1)
    {
      if(fgets(line,100,fdCode)==NULL)
         break;
      if((line[0]=='(')||(line[1]=='('))
      {}
      else
      {
         usleep(100000);
         for(i=0;i<100;i++)
            if(line[i]=='\r')
               break;
         SendBuf(cport_nr,line,i+1);
         usleep(100000);
         printf("%s",(char *)line);
         n = PollComport(cport_nr, buf, 4095);
         if(n > 0)
         {
            buf[n] = 0;   /* always put a "null" at the end of a string! */
            for(i=0; i < n; i++)
            {
               if(buf[i] < 32)  /* replace unreadable control-codes by dots */
               {
                  buf[i] = '.';
               }
            }
            //printf("received %i bytes: %s\n", n, (char *)buf);
	    printf("%s\n",(char *)buf);
         }
         else
         {
            //printf("received %i bytes.\n", n);
         }
         usleep(50000);  /* sleep for 100 milliSeconds */
         //while(n==0)
         //while((n==0)||((buf[0]!='o')&&(buf[1]!='k')))
         while((n==0)||(find_txt(buf)==0))
         {
            n = PollComport(cport_nr, buf, 4095);
            usleep(100000);
         }
      }
    }
  }
  return(0);
}

int find_txt(unsigned char *buf)
{
  int j;
  for(j=0;j<100;j++)
  {
    if(buf[j]=='\r')
       return 0;
    if((buf[j]=='o')&&(buf[j+1]=='k'))
       return 1;
  }
  return 0;
}

int OpenComport(int comport_number, int baudrate)
{
  int baudr;
  if((comport_number>27)||(comport_number<0))
  {
    printf("illegal comport number\n");
    return(1);
  }

  switch(baudrate)
  {
    case      50 : baudr = B50;
                   break;
    case      75 : baudr = B75;
                   break;
    case     110 : baudr = B110;
                   break;
    case     134 : baudr = B134;
                   break;
    case     150 : baudr = B150;
                   break;
    case     200 : baudr = B200;
                   break;
    case     300 : baudr = B300;
                   break;
    case     600 : baudr = B600;
                   break;
    case    1200 : baudr = B1200;
                   break;
    case    1800 : baudr = B1800;
                   break;
    case    2400 : baudr = B2400;
                   break;
    case    4800 : baudr = B4800;
                   break;
    case    9600 : baudr = B9600;
                   break;
    case   19200 : baudr = B19200;
                   break;
    case   38400 : baudr = B38400;
                   break;
    case   57600 : baudr = B57600;
                   break;
    case  115200 : baudr = B115200;
                   break;
    case  230400 : baudr = B230400;
                   break;
    case  460800 : baudr = B460800;
                   break;
    case  500000 : baudr = B500000;
                   break;
    case  576000 : baudr = B576000;
                   break;
    case  921600 : baudr = B921600;
                   break;
    case 1000000 : baudr = B1000000;
                   break;
    default      : printf("invalid baudrate\n");
                   return(1);
                   break;
  }

  Cport[comport_number] = open(comports[comport_number], O_RDWR | O_NOCTTY | O_NDELAY);
  if(Cport[comport_number]==-1)
  {
    perror("unable to open comport ");
    return(1);
  }

  error = tcgetattr(Cport[comport_number], old_port_settings + comport_number);
  if(error==-1)
  {
    close(Cport[comport_number]);
    perror("unable to read portsettings ");
    return(1);
  }
  memset(&new_port_settings, 0, sizeof(new_port_settings));  /* clear the new struct */

  new_port_settings.c_cflag = baudr | CS8 | CLOCAL | CREAD;
  new_port_settings.c_iflag = IGNPAR;
  new_port_settings.c_oflag = 0;
  new_port_settings.c_lflag = 0;
  new_port_settings.c_cc[VMIN] = 0;      /* block untill n bytes are received */
  new_port_settings.c_cc[VTIME] = 0;     /* block untill a timer expires (n * 100 mSec.) */
  error = tcsetattr(Cport[comport_number], TCSANOW, &new_port_settings);
  if(error==-1)
  {
    close(Cport[comport_number]);
    perror("unable to adjust portsettings ");
    return(1);
  }

  return(0);
}


int PollComport(int comport_number, unsigned char *buf, int size)
{
  int n;

#ifndef __STRICT_ANSI__                       /* __STRICT_ANSI__ is defined when the -ansi option is used for gcc */
  if(size>SSIZE_MAX)  size = (int)SSIZE_MAX;  /* SSIZE_MAX is defined in limits.h */
#else
  if(size>4096)  size = 4096;
#endif

  n = read(Cport[comport_number], buf, size);

  return(n);
}


int SendByte(int comport_number, unsigned char byte)
{
  int n;

  n = write(Cport[comport_number], &byte, 1);
  if(n<0)  return(1);

  return(0);
}


int SendBuf(int comport_number, unsigned char *buf, int size)
{
  return(write(Cport[comport_number], buf, size));
}


void CloseComport(int comport_number)
{
  close(Cport[comport_number]);
  tcsetattr(Cport[comport_number], TCSANOW, old_port_settings + comport_number);
}

/*
Constant  Description
TIOCM_LE  DSR (data set ready/line enable)
TIOCM_DTR DTR (data terminal ready)
TIOCM_RTS RTS (request to send)
TIOCM_ST  Secondary TXD (transmit)
TIOCM_SR  Secondary RXD (receive)
TIOCM_CTS CTS (clear to send)
TIOCM_CAR DCD (data carrier detect)
TIOCM_CD  Synonym for TIOCM_CAR
TIOCM_RNG RNG (ring)
TIOCM_RI  Synonym for TIOCM_RNG
TIOCM_DSR DSR (data set ready)
*/

int IsCTSEnabled(int comport_number)
{
  int status;

  status = ioctl(Cport[comport_number], TIOCMGET, &status);

  if(status&TIOCM_CTS) return(1);
  else return(0);
}
