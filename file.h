
#include <fcntl.h>

int file_type() ;
#include "./stream.h"
stream_t file_new(int fd) ;
stream_t file_open(const char * path, int oflag) ;
