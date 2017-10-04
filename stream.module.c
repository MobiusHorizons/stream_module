export {
#include <sys/types.h>
#include <stdlib.h>
}

#include <unistd.h>

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

export typedef struct {
  void *   ctx;
  read_t   read;
  write_t  write;
  pipe_t   pipe;
  error_t  error;
  int      type;
} stream_t;
export typedef stream_t t;

export int register(const char * name){
  int next = registered.size;
  registered.names = realloc(registered.names, (registered.size + 1) * sizeof(char*));

  registered.names[next] = name;
  registered.size++;

  return next;
}

export ssize_t read(stream_t s, void * buf, size_t nbyte) {
  return s.read(s.ctx, buf, nbyte, &s.error);
}

export ssize_t write(stream_t s, const void * buf, size_t nbyte) {
  return s.write(s.ctx, buf, nbyte, &s.error);
}

export ssize_t pipe(stream_t from, stream_t to) {
  if (from.type == to.type && from.pipe){
    return from.pipe(from.ctx, to.ctx, &from.error);
  }

  size_t page_size = getpagesize();
  char * buffer = malloc(page_size);
  
  ssize_t len = 0;
  ssize_t last_size;

  do {
    last_size = read(from, buffer, page_size);
    if (last_size < 0) {
      free(buffer);
      return last_size;
    }

    if (last_size > 0){
      last_size = write(to, buffer, last_size);
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
