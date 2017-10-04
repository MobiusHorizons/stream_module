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
typedef struct {
  void *   ctx;
  stream_read_t   read;
  stream_write_t  write;
  stream_pipe_t   pipe;
  stream_error_t  error;
  int      type;
} stream_stream_t;
typedef stream_stream_t stream_t;
int stream_register(const char * name);
ssize_t stream_read(stream_stream_t s, void * buf, size_t nbyte) ;
ssize_t stream_write(stream_stream_t s, const void * buf, size_t nbyte) ;
ssize_t stream_pipe(stream_stream_t from, stream_stream_t to) ;

#endif
