
#ifndef MDN_STANDARD_STREAMS_REDIRECTION_H
#define MDN_STANDARD_STREAMS_REDIRECTION_H

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

#include "mdn/status.h"

typedef enum mdn_StandardStreamsRedirection_StreamID_t_ {
    MDN_STANDARD_STREAMS_REDIRECTION_STREAM_ID_STDIN,
    MDN_STANDARD_STREAMS_REDIRECTION_STREAM_ID_STDOUT,
    MDN_STANDARD_STREAMS_REDIRECTION_STREAM_ID_STDERR,
    MDN_STANDARD_STREAMS_REDIRECTION_STREAM_ID_COUNT,
} mdn_StandardStreamsRedirection_StreamID_t;

mdn_Status_t mdn_StandardStreamsRedirection_start(mdn_StandardStreamsRedirection_StreamID_t streamID, FILE *redirectionFile);
mdn_Status_t mdn_StandardStreamsRedirection_stop(mdn_StandardStreamsRedirection_StreamID_t streamID);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // MDN_STANDARD_STREAMS_REDIRECTION_H
