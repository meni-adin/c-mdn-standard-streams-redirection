
#ifndef STANDARD_STREAMS_REDIRECTION_H
#define STANDARD_STREAMS_REDIRECTION_H

#if defined(__linux__) || defined(__APPLE__)
 #include <fcntl.h>
 #include <unistd.h>
 #define CROSS_OS_dup(...) dup(__VA_ARGS__)
 #define CROSS_OS_dup2(...) dup2(__VA_ARGS__)
 #define CROSS_OS_fileno(...) fileno(__VA_ARGS__)
#elif defined(_WIN32)
 #include <io.h>
 #define CROSS_OS_dup(...) _dup(__VA_ARGS__)
 #define CROSS_OS_dup2(...) _dup2(__VA_ARGS__)
 #define CROSS_OS_fileno(...) _fileno(__VA_ARGS__)
#endif // OS
#include <stdbool.h>
#include "errors.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// void StandardStreamsRedirection_init(const char *tempFilePrefix, bool separateStderr, bool keepTempFiles);
status_t StandardStreamsRedirection_init(void);
status_t StandardStreamsRedirection_deinit(void);
void StandardStreamsRedirection_startRedirection(void);
void StandardStreamsRedirection_stopRedirection(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // STANDARD_STREAMS_REDIRECTION_H
