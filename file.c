#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#
#include <fcntl.h>


#include "./stream.h"

static int _type;

int file_type() {
  if (_type == 0) {
    _type = stream_register("file");
  }

  return _type;
}

static ssize_t file_read(void * ctx, void * buf, size_t nbyte, stream_error_t * error) {
  int * fd = (int*)ctx;
  int e = read(*fd, buf, nbyte);
  if (e < 0 && error != NULL) {
    error->code    = errno;
    error->message = strerror(error->code);
  }
  return e;
}

static ssize_t file_write(void * ctx, const void * buf, size_t nbyte, stream_error_t * error) {
  int * fd = (int*)ctx;
  int e = write(*fd, buf, nbyte);
  if (e < 0 && error != NULL) {
    error->code    = errno;
    error->message = strerror(error->code);
  }
  return e;
}


stream_t file_new(int fd) {
  int * descriptor = malloc(sizeof(int));
  *descriptor = fd;

  stream_t s = {
    .ctx   = descriptor,
    .read  = file_read,
    .write = file_write,
    .pipe  = NULL,
    .error = {0},
    .type  = file_type(),
  };
  return s;
}

stream_t file_open(const char * path, int oflag) {
  int fd = open(path, oflag);
  if ( fd < 0 ) {
    stream_t s = {0};
    s.error.code    = errno;
    s.error.message = strerror(s.error.code);
  }


  int * descriptor = malloc(sizeof(int));
  *descriptor = fd;

  stream_t s = {
    .ctx   = descriptor,
    .read  = file_read,
    .write = file_write,
    .pipe  = NULL,
    .error = {0},
    .type  = file_type(),
  };
  return s;
}
