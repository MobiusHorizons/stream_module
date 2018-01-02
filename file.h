#ifndef _package_file_
#define _package_file_

#include <fcntl.h>
int file_type();

#include "stream.h"

stream_t * file_new(int fd);
int file_descriptor(stream_t * s);
stream_t * file_open(const char * path, int oflag);

#endif
