
#include "standard_streams_redirection.h"

#include <stdbool.h>

#include "mdn_status.h"

#ifdef C_STANDARD_STREAMS_REDIRECTION_SAFE_MODE
# define IS_VALID_STREAM_ID(streamID) ((0 <= (streamID)) && ((streamID) < STREAM_ID_COUNT))
#endif  // C_STANDARD_STREAMS_REDIRECTION_SAFE_MODE

typedef struct StandardStreamsRedirection_StreamData_t_ {
    int  backupFD;
    bool isRedirectionActivated;
} StandardStreamsRedirection_StreamData_t;

typedef struct StandardStreamsRedirection_InternalState_t_ {
    StandardStreamsRedirection_StreamData_t streamsData[STREAM_ID_COUNT];
} StandardStreamsRedirection_InternalState_t;

static StandardStreamsRedirection_InternalState_t  g_StandardStreamsRedirection_internalState_;
static StandardStreamsRedirection_InternalState_t *g_StandardStreamsRedirection_internalState = &g_StandardStreamsRedirection_internalState_;

static void StandardStreamsRedirection_flushStream(StandardStreamsRedirection_StreamID_t streamID) {
    switch (streamID) {
        case STREAM_ID_STDOUT:
            (void)fflush(stdout);
            break;
        case STREAM_ID_STDERR:
            (void)fflush(stderr);
            break;
    }
}

static void StandardStreamsRedirection_clearEOF(StandardStreamsRedirection_StreamID_t streamID) {
    switch (streamID) {
        case STREAM_ID_STDIN:
            clearerr(stdin);
            break;
        case STREAM_ID_STDOUT:
            clearerr(stdout);
            break;
        case STREAM_ID_STDERR:
            clearerr(stderr);
            break;
    }
}

mdn_Status_t StandardStreamsRedirection_start(StandardStreamsRedirection_StreamID_t streamID, FILE *redirectionFile) {
    int redirectionFD, backupFD, result;

#ifdef C_STANDARD_STREAMS_REDIRECTION_SAFE_MODE
    if (!IS_VALID_STREAM_ID(streamID)) {
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
    if (file == NULL) {
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
    if (g_StandardStreamsRedirection_internalState->streamsData[streamID].isRedirectionActivated == true) {
        return MDN_STATUS_ERROR_INVALID_OPERATION;
    }
#endif  // C_STANDARD_STREAMS_REDIRECTION_SAFE_MODE

    StandardStreamsRedirection_flushStream(streamID);

    redirectionFD = CROSS_OS_fileno(redirectionFile);
    if (redirectionFD == -1) {
        return MDN_STATUS_ERROR_CHECK_ERRNO;
    }

    backupFD = CROSS_OS_dup((int)streamID);
    if (backupFD == -1) {
        return MDN_STATUS_ERROR_CHECK_ERRNO;
    }
    g_StandardStreamsRedirection_internalState->streamsData[streamID].backupFD = backupFD;

    result = CROSS_OS_dup2(redirectionFD, (int)streamID);
    if (result == -1) {
        return MDN_STATUS_ERROR_CHECK_ERRNO;
    }

    return MDN_STATUS_SUCCESS;
}

mdn_Status_t StandardStreamsRedirection_stop(StandardStreamsRedirection_StreamID_t streamID) {
    int result;

#ifdef C_STANDARD_STREAMS_REDIRECTION_SAFE_MODE
    if (!IS_VALID_STREAM_ID(streamID)) {
        return MDN_STATUS_ERROR_BAD_ARGUMENT;
    }
    if (g_StandardStreamsRedirection_internalState->streamsData[streamID].isRedirectionActivated == false) {
        return MDN_STATUS_ERROR_INVALID_OPERATION;
    }
#endif  // C_STANDARD_STREAMS_REDIRECTION_SAFE_MODE

    StandardStreamsRedirection_flushStream(streamID);

    result = CROSS_OS_dup2(g_StandardStreamsRedirection_internalState->streamsData[streamID].backupFD, (int)streamID);
    if (result == -1) {
        return MDN_STATUS_ERROR_CHECK_ERRNO;
    }

    StandardStreamsRedirection_clearEOF(streamID);

    return MDN_STATUS_SUCCESS;
}
