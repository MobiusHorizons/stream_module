#ifndef _package_file_
#define _package_file_

#include "stream.h"

#include <fcntl.h>

int file_type();

/**
 * Frees all resources held by the file, and returns the file
 * descriptor.
 */
int file_free(stream_t * s);

stream_t * file_new(int fd);
int file_descriptor(stream_t * s);
stream_t * file_open(const char * path, int oflag);

#endif
