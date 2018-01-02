#define _GNU_SOURCE
export {
#include <sys/types.h>
#include <stdlib.h>
}

#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>

export typedef struct {
  const char * message;
  int          code;
} error_t;

static struct {
  const char ** names;
  size_t        size;
} registered = {0};

export typedef ssize_t (*read_t )(void * ctx, void       * buf, size_t nbyte, error_t *error);
export typedef ssize_t (*write_t)(void * ctx, const void * buf, size_t nbyte, error_t *error);
export typedef ssize_t (*pipe_t )(void * ctx, void       * dest,              error_t *error);
export typedef ssize_t (*close_t)(void * ctx, error_t *error);

export typedef struct {
  void *   ctx;
  read_t   read;
  write_t  write;
  pipe_t   pipe;
  close_t  close;
  error_t  error;
  int      type;
} stream_t as t;

export int register(const char * name){
  if (registered.size == 0) { // init
    registered.names    = malloc(1 * sizeof(char*));
    registered.names[0] = "error";
    registered.size     = 1;
  }

  int next = registered.size;
  registered.names = realloc(registered.names, (registered.size + 1) * sizeof(char*));

  registered.names[next] = name;
  registered.size++;

  return next;
}

export stream_t * error(stream_t * s, int code, const char * message) {
  if (s == NULL) {
    s = calloc(1, sizeof(stream_t));
  }
  s->error.code    = code;
  s->error.message = message;
  return s;
}

export ssize_t read(stream_t * s, void * buf, size_t nbyte) {

  return s->read(s->ctx, buf, nbyte, &s->error);
}

export ssize_t write(stream_t * s, const void * buf, size_t nbyte) {
  return s->write(s->ctx, buf, nbyte, &s->error);
}

export ssize_t pipe(stream_t * from, stream_t * to) {
  if (from->type == to->type && from->pipe){
    return from->pipe(from->ctx, to->ctx, &from->error);
  }

  size_t page_size = sysconf(_SC_PAGESIZE);
  char * buffer = malloc(page_size);

  ssize_t len = 0;
  ssize_t last_size;

  do {
    last_size = read(from, buffer, page_size);
    if (last_size < 0 || from->error.code != 0) {
      free(buffer);
      return -1;
    }

    if (last_size > 0){
      last_size = write(to, buffer, last_size);
      if (last_size < 0 || to->error.code != 0) {
        free(buffer);
        return -2;
      }
    }
    len += last_size;

  } while (last_size > 0);

  free(buffer);
  return len;
}

export ssize_t printf(stream_t * s, char * fmt, ...) {
  va_list args;
  va_start(args, fmt);
  char * buf;
  ssize_t length = vasprintf(&buf, fmt, args);

  if (length < 0) {
    s->error.code    = errno;
    s->error.message = strerror(s->error.code);
    return length;
  }

  return write(s, buf, length);
}

export ssize_t close(stream_t * s) {
  if (s->close == NULL) {
    s->error.code    = -1;
    s->error.message = "The provided stream does not implement 'close'";
    return -1;
  }

  return s->close(s->ctx, &s->error);
}
