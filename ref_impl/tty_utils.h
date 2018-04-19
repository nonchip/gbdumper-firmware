#ifndef _TTY_UTILS_H
#define _TTY_UTILS_H

int set_interface_attribs (int fd, int speed, int parity, int to);
void set_blocking (int fd, int should_block, int to);

#endif
