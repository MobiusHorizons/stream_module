#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
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

static ssize_t file_close(void * ctx, stream.error_t * error) {
  int * fd = (int*)ctx;
  int e = global.close(*fd);
  if (e < 0 && error != NULL) {
    error->code    = errno;
    error->message = strerror(error->code);
  }
  global.free(ctx);
  return e;
}


export stream.t * new(int fd) {
  int * descriptor = malloc(sizeof(int));
  *descriptor = fd;

  stream.t * s = malloc(sizeof(stream.t));

  s->ctx   = descriptor;
  s->read  = file_read;
  s->write = file_write;
  s->pipe  = NULL;
  s->close = file_close;
  s->type  = type();

  s->error.code    = 0;
  s->error.message = NULL;

  return s;
}

export int descriptor(stream.t * s) {
  if (s == NULL || s->type != type()) return -1;
  int fd = *(int*)s->ctx;
  return fd;
}

export stream.t * open(const char * path, int oflag) {
  int fd = global.open(path, oflag);
  if ( fd < 0 ) {
    return stream.error(NULL, errno, strerror(errno));
  }

  return new(fd);
}
