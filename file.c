#define _package_file_
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <fcntl.h>


#include "stream.h"
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

static ssize_t file_close(void * ctx, stream_error_t * error) {
	int * fd = (int*)ctx;
	int e = close(*fd);
	if (e < 0 && error != NULL) {
		error->code    = errno;
		error->message = strerror(error->code);
	}
	free(ctx);
	return e;
}

/**
 * Frees all resources held by the file, and returns the file
 * descriptor.
 */
int file_free(stream_t * s) {
	if (s->type != file_type()) {
		errno = EINVAL;
		return -1;
	}

	int fd = *(int*)s->ctx;
	free(s->ctx);
	free(s);
	return fd;
}

stream_t * file_new(int fd) {
	int * descriptor = malloc(sizeof(int));
	*descriptor = fd;

	stream_t * s = malloc(sizeof(stream_t));

	s->ctx   = descriptor;
	s->read  = file_read;
	s->write = file_write;
	s->pipe  = NULL;
	s->close = file_close;
	s->type  = file_type();

	s->error.code    = 0;
	s->error.message = NULL;

	return s;
}

int file_descriptor(stream_t * s) {
	if (s == NULL || s->type != file_type()) return -1;
	int fd = *(int*)s->ctx;
	return fd;
}

stream_t * file_open(const char * path, int oflag) {
	int fd = open(path, oflag, 0666);
	if ( fd < 0 ) {
		return stream_error(NULL, errno, strerror(errno));
	}

	return file_new(fd);
}
