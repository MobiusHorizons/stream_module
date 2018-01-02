#ifndef _stream_
#define _stream_

#include <sys/types.h>
#include <stdlib.h>

typedef struct {
  const char * message;
  int          code;
} stream_error_t;

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

int stream_register(const char * name);
stream_t * stream_error(stream_t * s, int code, const char * message);
ssize_t stream_read(stream_t * s, void * buf, size_t nbyte);
ssize_t stream_write(stream_t * s, const void * buf, size_t nbyte);
ssize_t stream_pipe(stream_t * from, stream_t * to);
ssize_t stream_printf(stream_t * s, char * fmt, ...);
ssize_t stream_close(stream_t * s);

#endif
