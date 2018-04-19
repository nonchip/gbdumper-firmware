#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

#include "../tty_utils.h"

#include "../header_utils.h"

void writes(int fd,char* str){
  ssize_t l;
  while((l=strlen(str))>0){
    //fprintf(stderr,"Writing[%3d]: %s\n",l,str);
    ssize_t s=write(fd,str,l);
    if(s<0){
      perror("Failed to write");
      abort();
    }
    usleep (l*500);
    str+=s;
  }
}

char cmd_rstr[256];
char* cmd(int fd,char* wstr){
  cmd_rstr[0]=0;
  writes(fd,wstr);
  writes(fd,"\r\n");
  char* sp=cmd_rstr;
  while(true){
    read(fd,sp,1);
    if(*sp=='\r' || *sp=='\n' || *sp<1){
      *sp=0;
      break;
    }
    sp++;
  }
  if(strlen(cmd_rstr)>0){
    fprintf(stderr,"%10s %s\n",wstr,cmd_rstr);
  }else{
    fprintf(stderr,"%10s OK\n",wstr);
  }
  if(0==strcmp(cmd_rstr,"ERR\r\n")){
    fprintf(stderr,"%10s ERROR\n",wstr);
    return NULL;
  }
  return cmd_rstr;
}

uint8_t read_from_addr(int fd, uint16_t addr){
  if(!cmd(fd,"C rC"))
    return 0;
  char buf[100];
  sprintf(buf,"A %04x",addr);
  if(!cmd(fd,buf))
    return 0;
  if(!cmd(fd,"C rcC"))
    return 0;
  if(!cmd(fd,"C rc"))
    return 0;
  char* r=cmd(fd,"R");
  if(!r)
    return 0;
  uint8_t ri;
  sscanf(r,"%x",&ri);
  return ri;
}

int main(int argc, char* argv[]){
  if(argc<1){
    fprintf(stderr,"require TTY as first argument.\n");
    return 1;
  }

  int fd = open (argv[1], O_RDWR | O_NOCTTY | O_SYNC);
  if (fd < 0)
  {
    fprintf(stderr,"error opening %s: %s\n", argv[1], strerror(errno));
    return 1;
  }

  set_interface_attribs (fd, B9600, 0, 1); // set speed to 115,200 bps, 8n1 (no parity), 0.1s timeout
  set_blocking (fd, 0, 0);                 // set no blocking, no timeout
  
  if(!cmd(fd,"C RC"))
    return 1;

  struct header_t* header = malloc(HEADER_SIZE);

  for (int i = 0; i < HEADER_SIZE; ++i)
  {
    ((uint8_t*)header)[i] = read_from_addr(fd,HEADER_START+i);
  }

  printf("Title: %s\n", header->title);

  //write (fd, "hello!\n", 7);            // send 7 character greeting

  //usleep ((7 + 25) * 100);              // sleep enough to transmit the 7 plus
                                          // receive 25:  approx 100 uS per char transmit
  //char buf [100];
  //int n = read (fd, buf, sizeof buf);   // read up to 100 characters if ready to read

  close(fd);
  return 0;
}