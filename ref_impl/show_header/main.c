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

#define _BV(bit) \
  (1 << (bit)) 

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

void printha(char* pre, uint8_t* a, ssize_t len){
  printf("%s",pre);
  for (int i = 0; i < len; ++i)
  {
    printf(" %02x",a[i]);
  }
  printf("\n");
}

void printhca(char* pre, uint8_t* a, ssize_t len){
  printf("%s",pre);
  for (int i = 0; i < len; ++i)
  {
    printf(" %02x",a[i]);
  }
  for (int i = 0; i < len; ++i)
  {
    printf("%c",a[i]);
  }
  printf("\n");
}

void printca(char* pre, uint8_t* a, ssize_t len){
  printf("%s",pre);
  for (int i = 0; i < len; ++i)
  {
    printf("%c",a[i]);
  }
  printf("\n");
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

  printha("Entry point:", header->entrypoint, 4);
  if(header->entrypoint[0]==0x00 && header->entrypoint[1]==0xc3){
    uint16_t to = (header->entrypoint[2]) | (header->entrypoint[3]<<8);
    printf("  found standard NOP;JP pattern pointing to %sstandard location %#06x\n",(to==0x0150?"":"non-"),to);
  }

  printf("Logo: %s\n",(0==memcmp(header->logo,nintendo_logo,48)?"nintendo":"custom"));

  printf("Title: %s\n", header->title);

  printha("Manufacturer code:",header->mancode,4);

  printf("CGB: %02x ",header->cgbflag[0]);
  if((header->cgbflag[0] & _BV(7))!=0){
    if(header->cgbflag[0]==0x80){
      printf("supported\n");
    }else if(header->cgbflag[0]==0xC0){
      printf("required\n");
    }else if( (header->cgbflag[0] & _BV(3))!=0 || (header->cgbflag[0] & _BV(2))!=0 ){
      printf("unsupported, without palettes\n");
    }
  }else{
    printf("unsupported, with palettes\n");
  }

  printf("SGB: %02x %sabled\n",header->sgbflag[0],(header->sgbflag[0]==0x30?"en":"dis"));

  printf("Cartridge type: %02x ",header->carttype[0]);
  if(0!=carttypes[header->carttype[0]])
    printf("%s\n",carttypes[header->carttype[0]]);
  else
    printf("unknown\n");

  printf("ROM size: %02x ",header->romsize[0]);
  if(0!=rombanks[header->romsize[0]])
    printf("%d banks = %dK\n",rombanks[header->romsize[0]],rombanks[header->romsize[0]]*32);
  else
    printf("unknown\n");

  printf("RAM size: %02x ",header->ramsize[0]);
  if(0==header->ramsize[0])
    printf("none\n");
  else if(0!=ramKs[header->ramsize[0]]){
    printf("%dK",ramKs[header->ramsize[0]]);
    if(0!=ram8Kbanks[header->ramsize[0]])
      printf(" in %d 8K banks\n",ram8Kbanks[header->ramsize[0]]);
    else
      printf("without banking\n");
  }else
    printf("unknown\n");

  printf("Destination code: %02x %s\n",header->destcode[0],(0==header->destcode[0]?"Japan":"Not Japan"));

  printf("Licensee: %02x ",header->oldlicense[0]);
  if(0x33==header->oldlicense[0]){
    printf("new code: %02x ",header->newlicense[0]);
    if(0!=newlicensees[header->newlicense[0]])
      printf("%s\n",newlicensees[header->newlicense[0]]);
    else
      printf("unknown\n");
  }else{
    printf("old code\n");
  }

  printf("Mask ROM version: %02x\n",header->maskver[0]);

  uint8_t chk=calc_hchk(header);
  printf("Header checksum: %02x %scorrect, calculated %02x\n",header->hchksum[0],(header->hchksum[0] == chk ?"":"in"),chk);

  printha("Global checksum:",header->gchksum,2);

  //write (fd, "hello!\n", 7);            // send 7 character greeting

  //usleep ((7 + 25) * 100);              // sleep enough to transmit the 7 plus
                                          // receive 25:  approx 100 uS per char transmit
  //char buf [100];
  //int n = read (fd, buf, sizeof buf);   // read up to 100 characters if ready to read

  close(fd);
  return 0;
}