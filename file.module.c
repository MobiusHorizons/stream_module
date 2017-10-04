#include <unistd.h>
#include <stdlib.h>
#export {
#include <fcntl.h>
}

import stream from "./stream.module.c";
static int _type;

export int type() {
  if (_type == 0) {
    _type = stream.register("file");
  }

  return _type;
}

static ssize_t file_read(void * ctx, void * buf, size_t nbyte, stream.error_t * error) {
  int * fd = (int*)ctx;
  int e = global.read(*fd, buf, nbyte);
  if (e < 0 && error != NULL) {
    error->code    = errno;
    error->message = strerror(error->code);
  }
  return e;
}

static ssize_t file_write(void * ctx, const void * buf, size_t nbyte, stream.error_t * error) {
  int * fd = (int*)ctx;
  int e = global.write(*fd, buf, nbyte);
  if (e < 0 && error != NULL) {
    error->code    = errno;
    error->message = strerror(error->code);
  }
  return e;
}


export stream.t new(int fd) {
  int * descriptor = malloc(sizeof(int));
  *descriptor = fd;

  stream.t s = {
    .ctx   = descriptor,
    .read  = file_read,
    .write = file_write,
    .pipe  = NULL,
    .error = {0},
    .type  = type(),
  };
  return s;
}

export stream.t open(const char * path, int oflag) {
  int fd = global.open(path, oflag);
  if ( fd < 0 ) {
    stream.t s = {0};
    s.error.code    = errno;
    s.error.message = strerror(error->code);
  }


  int * descriptor = malloc(sizeof(int));
  *descriptor = fd;

  stream.t s = {
    .ctx   = descriptor,
    .read  = file_read,
    .write = file_write,
    .pipe  = NULL,
    .error = {0},
    .type  = type(),
  };
  return s;
}
