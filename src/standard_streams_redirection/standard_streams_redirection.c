
#include "standard_streams_redirection.h"

#include <stdbool.h>

#include "errors.h"

#ifdef C_STANDARD_STREAMS_REDIRECTION_SAFE_MODE
# define IS_VALID_STREAM_ID(streamID) ((0 <= (streamID)) && ((streamID) < STREAM_ID_COUNT))
#endif  // C_STANDARD_STREAMS_REDIRECTION_SAFE_MODE

typedef struct StandardStreamsRedirection_SteamData_t_ {
    int  backupFD;
    bool isRedirectionActivated;

} StandardStreamsRedirection_SteamData_t;

typedef struct StandardStreamsRedirection_InternalState_t_ {
    StandardStreamsRedirection_SteamData_t streamsData[STREAM_ID_COUNT];
} StandardStreamsRedirection_InternalState_t;

static StandardStreamsRedirection_InternalState_t  g_StandardStreamsRedirection_internalState_;
static StandardStreamsRedirection_InternalState_t *g_StandardStreamsRedirection_internalState = &g_StandardStreamsRedirection_internalState_;

static void StandardStreamsRedirection_flushStream(StandardStreamsRedirection_StreamID_t streamID) {
    switch (streamID) {
        case STREAM_ID_STDOUT:
            fflush(stdout);
            break;
        case STREAM_ID_STDERR:
            fflush(stderr);
            break;
        default:
            break;
    }
}

status_t StandardStreamsRedirection_start(StandardStreamsRedirection_StreamID_t streamID, FILE *redirectionFile) {
    int redirectionFD, backupFD, result;

#ifdef C_STANDARD_STREAMS_REDIRECTION_SAFE_MODE
    if (!IS_VALID_STREAM_ID(streamID)) {
        return ERR_BAD_ARGUMENT;
    }
    if (file == NULL) {
        return ERR_BAD_ARGUMENT;
    }
    if (g_StandardStreamsRedirection_internalState->streamsData[streamID].isRedirectionActivated == true) {
        return ERR_INVALID_OPERATION;
    }
#endif  // C_STANDARD_STREAMS_REDIRECTION_SAFE_MODE

    StandardStreamsRedirection_flushStream(streamID);

    redirectionFD = CROSS_OS_fileno(redirectionFile);
    if (redirectionFD == -1) {
        return ERR_CHECK_ERRNO;
    }

    backupFD = CROSS_OS_dup((int)streamID);
    if (backupFD == -1) {
        return ERR_CHECK_ERRNO;
    }
    g_StandardStreamsRedirection_internalState->streamsData[streamID].backupFD = backupFD;

    result = CROSS_OS_dup2(redirectionFD, (int)streamID);
    if (result == -1) {
        return ERR_CHECK_ERRNO;
    }

    return SUCCESS;
}

status_t StandardStreamsRedirection_stop(StandardStreamsRedirection_StreamID_t streamID){
    int result;

#ifdef C_STANDARD_STREAMS_REDIRECTION_SAFE_MODE
    if (!IS_VALID_STREAM_ID(streamID)) {
        return ERR_BAD_ARGUMENT;
    }
    if (g_StandardStreamsRedirection_internalState->streamsData[streamID].isRedirectionActivated == false) {
        return ERR_INVALID_OPERATION;
    }
#endif  // C_STANDARD_STREAMS_REDIRECTION_SAFE_MODE

    StandardStreamsRedirection_flushStream(streamID);

    result = CROSS_OS_dup2(g_StandardStreamsRedirection_internalState->streamsData[streamID].backupFD, (int)streamID);
    if (result == -1) {
        return ERR_CHECK_ERRNO;
    }

    return SUCCESS;
}
