
#ifndef MDN_STANDARD_STREAMS_REDIRECTION_H
#define MDN_STANDARD_STREAMS_REDIRECTION_H

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

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
