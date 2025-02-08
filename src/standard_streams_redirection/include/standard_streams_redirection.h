
#ifndef STANDARD_STREAMS_REDIRECTION_H
#define STANDARD_STREAMS_REDIRECTION_H

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#if defined(__linux__) || defined(__APPLE__)
# include <fcntl.h>
# include <unistd.h>
# define CROSS_OS_dup(...)    dup(__VA_ARGS__)
# define CROSS_OS_dup2(...)   dup2(__VA_ARGS__)
# define CROSS_OS_fileno(...) fileno(__VA_ARGS__)
#elif defined(_WIN32)
# include <io.h>
# define CROSS_OS_dup(...)    _dup(__VA_ARGS__)
# define CROSS_OS_dup2(...)   _dup2(__VA_ARGS__)
# define CROSS_OS_fileno(...) _fileno(__VA_ARGS__)
#endif  // OS

#include <stdio.h>

#include "errors.h"

typedef enum StandardStreamsRedirection_StreamID_t_ {
    STREAM_ID_STDIN,
    STREAM_ID_STDOUT,
    STREAM_ID_STDERR,
    STREAM_ID_COUNT,
} StandardStreamsRedirection_StreamID_t;

status_t StandardStreamsRedirection_start(StandardStreamsRedirection_StreamID_t streamID, FILE *redirectionFile);
status_t StandardStreamsRedirection_stop(StandardStreamsRedirection_StreamID_t streamID);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // STANDARD_STREAMS_REDIRECTION_H
