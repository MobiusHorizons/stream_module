#define _GNU_SOURCE

#include <sys/types.h>
#include <stdlib.h>


#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>

typedef struct {
  const char * message;
  int          code;
} stream_error_t;

static struct {
  const char ** names;
  size_t        size;
} registered = {0};

typedef ssize_t (*stream_read_t )(void * ctx, void       * buf, size_t nbyte, stream_error_t *error);
typedef ssize_t (*stream_write_t)(void * ctx, const void * buf, size_t nbyte, stream_error_t *error);
typedef ssize_t (*stream_pipe_t )(void * ctx, void       * dest,              stream_error_t *error);
typedef ssize_t (*stream_close_t)(void * ctx, stream_error_t *error);

typedef struct {
  void *   ctx;
  stream_read_t   read;
  stream_write_t  write;
  stream_pipe_t   pipe;
  stream_close_t  close;
  stream_error_t  error;
  int      type;
} stream_t;

int stream_register(const char * name){
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

stream_t * stream_error(stream_t * s, int code, const char * message) {
  if (s == NULL) {
    s = calloc(1, sizeof(stream_t));
  }
  s->error.code    = code;
  s->error.message = message;
  return s;
}

ssize_t stream_read(stream_t * s, void * buf, size_t nbyte) {

  return s->read(s->ctx, buf, nbyte, &s->error);
}

ssize_t stream_write(stream_t * s, const void * buf, size_t nbyte) {
  return s->write(s->ctx, buf, nbyte, &s->error);
}

ssize_t stream_pipe(stream_t * from, stream_t * to) {
  if (from->type == to->type && from->pipe){
    return from->pipe(from->ctx, to->ctx, &from->error);
  }

  size_t page_size = sysconf(_SC_PAGESIZE);
  char * buffer = malloc(page_size);

  ssize_t len = 0;
  ssize_t last_size;

  do {
    last_size = stream_read(from, buffer, page_size);
    if (last_size < 0 || from->error.code != 0) {
      free(buffer);
      return -1;
    }

    if (last_size > 0){
      last_size = stream_write(to, buffer, last_size);
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

ssize_t stream_printf(stream_t * s, char * fmt, ...) {
  va_list args;
  va_start(args, fmt);
  char * buf;
  ssize_t length = vasprintf(&buf, fmt, args);

  if (length < 0) {
    s->error.code    = errno;
    s->error.message = strerror(s->error.code);
    return length;
  }

  return stream_write(s, buf, length);
}

ssize_t stream_close(stream_t * s) {
  if (s->close == NULL) {
    s->error.code    = -1;
    s->error.message = "The provided stream does not implement 'close'";
    return -1;
  }

  return s->close(s->ctx, &s->error);
}