#ifndef _HEADER_UTILS_H
#define _HEADER_UTILS_H

#include <stdint.h>
#include <stdbool.h>

#define HEADER_START ( 0x100 )
#define HEADER_SIZE ( sizeof(struct header_t) )
#define HEADER_END ( HEADER_START + HEADER_SIZE )

#define HEADER_FIELD(n,s,e) uint8_t n[(e - s) + 1]

struct header_t{
  HEADER_FIELD(entrypoint, 0x0100, 0x0103);
  HEADER_FIELD(logo,       0x0104, 0x0133);
  HEADER_FIELD(title,      0x0134, 0x013e); // goes to 143 on old games, but then we'd have alignment issues, todo: check if `union` helps
  HEADER_FIELD(mancode,    0x013f, 0x0142);
  HEADER_FIELD(cgbflag,    0x0143, 0x0143);
  HEADER_FIELD(newlicense, 0x0144, 0x0145);
  HEADER_FIELD(sgbflag,    0x0146, 0x0146);
  HEADER_FIELD(carttype,   0x0147, 0x0147);
  HEADER_FIELD(romsize,    0x0148, 0x0148);
  HEADER_FIELD(ramsize,    0x0149, 0x0149);
  HEADER_FIELD(destcode,   0x014a, 0x014a);
  HEADER_FIELD(oldlicense, 0x014b, 0x014b);
  HEADER_FIELD(maskver,    0x014c, 0x014c);
  HEADER_FIELD(hchksum,    0x014d, 0x014d);
  HEADER_FIELD(gchksum,    0x014e, 0x014f);
} __attribute__ ((packed));

const uint8_t nintendo_logo[48];

const char* newlicensees[0xff];

#define CGB_ENABLED 0x80
#define CGB_REQUIRED 0xc0

#define SDB_ENABLED 0x03

const char* carttypes[0x100];

const int rombanks[0xff];

const int ramKs[0xff];
const int ram8Kbanks[0xff];

#define DEST_JP 0
#define DEST_NJP 1

#define OLDLICENSE_USENEW 0x33

#define HCHK_START 0x0134
#define HCHK_END 0x014c
#define HCHK_SIZE (HCHK_END-HCHK_START)
uint8_t calc_hchk(struct header_t* h);

//uint16_t* calc_gchk(uint8_t *start,size_t length);

#endif
