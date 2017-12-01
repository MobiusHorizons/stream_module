#ifndef _file_
#define _file_

#include <fcntl.h>

#include "./stream.h"

int file_type();
stream_t * file_new(int fd);
int file_descriptor(stream_t * s);
stream_t * file_open(const char * path, int oflag);

#endif
