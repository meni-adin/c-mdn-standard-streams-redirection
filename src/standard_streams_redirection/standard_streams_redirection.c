
#include "mdn/standard_streams_redirection.h"

#include <stdbool.h>

#include "mdn/status.h"

#if defined(__linux__) || defined(__APPLE__)
# include <fcntl.h>
# include <unistd.h>
# define MDN_STANDARD_STREAMS_REDIRECTION_CROSS_OS_dup(...)    dup(__VA_ARGS__)
# define MDN_STANDARD_STREAMS_REDIRECTION_CROSS_OS_dup2(...)   dup2(__VA_ARGS__)
# define MDN_STANDARD_STREAMS_REDIRECTION_CROSS_OS_fileno(...) fileno(__VA_ARGS__)
#elif defined(_WIN32)
# include <io.h>
# define MDN_STANDARD_STREAMS_REDIRECTION_CROSS_OS_dup(...)    _dup(__VA_ARGS__)
# define MDN_STANDARD_STREAMS_REDIRECTION_CROSS_OS_dup2(...)   _dup2(__VA_ARGS__)
# define MDN_STANDARD_STREAMS_REDIRECTION_CROSS_OS_fileno(...) _fileno(__VA_ARGS__)
#endif  // OS

#ifdef MDN_STANDARD_STREAMS_REDIRECTION_SAFE_MODE
# define MDN_STANDARD_STREAMS_REDIRECTION_IS_VALID_STREAM_ID(streamID) ((0 <= (streamID)) && ((streamID) < MDN_STANDARD_STREAMS_REDIRECTION_STREAM_ID_COUNT))
#endif  // MDN_STANDARD_STREAMS_REDIRECTION_SAFE_MODE

typedef struct mdn_StandardStreamsRedirection_StreamData_t_ {
    int  backupFD;
    bool isRedirectionActivated;
} mdn_StandardStreamsRedirection_StreamData_t;

typedef struct mdn_StandardStreamsRedirection_InternalState_t_ {
    mdn_StandardStreamsRedirection_StreamData_t streamsData[MDN_STANDARD_STREAMS_REDIRECTION_STREAM_ID_COUNT];
} mdn_StandardStreamsRedirection_InternalState_t;

static mdn_StandardStreamsRedirection_InternalState_t  g_StandardStreamsRedirection_internalState_;
static mdn_StandardStreamsRedirection_InternalState_t *g_StandardStreamsRedirection_internalState = &g_StandardStreamsRedirection_internalState_;

static void mdn_StandardStreamsRedirection_flushStream(mdn_StandardStreamsRedirection_StreamID_t streamID) {
    switch (streamID) {
        case MDN_STANDARD_STREAMS_REDIRECTION_STREAM_ID_STDOUT:
            (void)fflush(stdout);
            break;
        case MDN_STANDARD_STREAMS_REDIRECTION_STREAM_ID_STDERR:
            (void)fflush(stderr);
            break;
    }
}

static void mdn_StandardStreamsRedirection_clearEOF(mdn_StandardStreamsRedirection_StreamID_t streamID) {
    switch (streamID) {
        case MDN_STANDARD_STREAMS_REDIRECTION_STREAM_ID_STDIN:
            clearerr(stdin);
            break;
        case MDN_STANDARD_STREAMS_REDIRECTION_STREAM_ID_STDOUT:
            clearerr(stdout);
            break;
        case MDN_STANDARD_STREAMS_REDIRECTION_STREAM_ID_STDERR:
            clearerr(stderr);
            break;
    }
}

mdn_Status_t mdn_StandardStreamsRedirection_start(mdn_StandardStreamsRedirection_StreamID_t streamID, FILE *redirectionFile) {
    int redirectionFD, backupFD, result;

#ifdef MDN_STANDARD_STREAMS_REDIRECTION_SAFE_MODE
    if (!MDN_STANDARD_STREAMS_REDIRECTION_IS_VALID_STREAM_ID(streamID)) {
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
    if (redirectionFile == NULL) {
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
    if (g_StandardStreamsRedirection_internalState->streamsData[streamID].isRedirectionActivated == true) {
        return MDN_STATUS_ERROR_INVALID_OPERATION;
    }
#endif  // MDN_STANDARD_STREAMS_REDIRECTION_SAFE_MODE

    mdn_StandardStreamsRedirection_flushStream(streamID);

    redirectionFD = MDN_STANDARD_STREAMS_REDIRECTION_CROSS_OS_fileno(redirectionFile);
    if (redirectionFD == -1) {
        return MDN_STATUS_ERROR_CHECK_ERRNO;
    }

    backupFD = MDN_STANDARD_STREAMS_REDIRECTION_CROSS_OS_dup((int)streamID);
    if (backupFD == -1) {
        return MDN_STATUS_ERROR_CHECK_ERRNO;
    }
    g_StandardStreamsRedirection_internalState->streamsData[streamID].backupFD = backupFD;

    result = MDN_STANDARD_STREAMS_REDIRECTION_CROSS_OS_dup2(redirectionFD, (int)streamID);
    if (result == -1) {
        return MDN_STATUS_ERROR_CHECK_ERRNO;
    }

#ifdef MDN_STANDARD_STREAMS_REDIRECTION_SAFE_MODE
    g_StandardStreamsRedirection_internalState->streamsData[streamID].isRedirectionActivated = true;
#endif  // MDN_STANDARD_STREAMS_REDIRECTION_SAFE_MODE
    return MDN_STATUS_SUCCESS;
}

mdn_Status_t mdn_StandardStreamsRedirection_stop(mdn_StandardStreamsRedirection_StreamID_t streamID) {
    int result;

#ifdef MDN_STANDARD_STREAMS_REDIRECTION_SAFE_MODE
    if (!MDN_STANDARD_STREAMS_REDIRECTION_IS_VALID_STREAM_ID(streamID)) {
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
    if (g_StandardStreamsRedirection_internalState->streamsData[streamID].isRedirectionActivated == false) {
        return MDN_STATUS_ERROR_INVALID_OPERATION;
    }
#endif  // MDN_STANDARD_STREAMS_REDIRECTION_SAFE_MODE

    mdn_StandardStreamsRedirection_flushStream(streamID);

    result = MDN_STANDARD_STREAMS_REDIRECTION_CROSS_OS_dup2(g_StandardStreamsRedirection_internalState->streamsData[streamID].backupFD, (int)streamID);
    if (result == -1) {
        return MDN_STATUS_ERROR_CHECK_ERRNO;
    }

    mdn_StandardStreamsRedirection_clearEOF(streamID);

#ifdef MDN_STANDARD_STREAMS_REDIRECTION_SAFE_MODE
    g_StandardStreamsRedirection_internalState->streamsData[streamID].isRedirectionActivated = false;
#endif  // MDN_STANDARD_STREAMS_REDIRECTION_SAFE_MODE
    return MDN_STATUS_SUCCESS;
}
