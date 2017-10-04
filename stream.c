
#include <sys/types.h>
#include <stdlib.h>


#include <unistd.h>

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

typedef struct {
  void *   ctx;
  stream_read_t   read;
  stream_write_t  write;
  stream_pipe_t   pipe;
  stream_error_t  error;
  int      type;
} stream_stream_t;
typedef stream_stream_t stream_t;

int stream_register(const char * name){
  int next = registered.size;
  registered.names = realloc(registered.names, (registered.size + 1) * sizeof(char*));

  registered.names[next] = name;
  registered.size++;

  return next;
}

ssize_t stream_read(stream_stream_t s, void * buf, size_t nbyte) {
  return s.read(s.ctx, buf, nbyte, &s.error);
}

ssize_t stream_write(stream_stream_t s, const void * buf, size_t nbyte) {
  return s.write(s.ctx, buf, nbyte, &s.error);
}

ssize_t stream_pipe(stream_stream_t from, stream_stream_t to) {
  if (from.type == to.type && from.pipe){
    return from.pipe(from.ctx, to.ctx, &from.error);
  }

  size_t page_size = getpagesize();
  char * buffer = malloc(page_size);
  
  ssize_t len = 0;
  ssize_t last_size;

  do {
    last_size = stream_read(from, buffer, page_size);
    if (last_size < 0) {
      free(buffer);
      return last_size;
    }

    if (last_size > 0){
      last_size = stream_write(to, buffer, last_size);
      if (last_size < 0) {
        free(buffer);
        return last_size;
      }
    } 
    len += last_size;

  } while (last_size > 0);

  free(buffer);
  return len;
}
