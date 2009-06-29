//! Flog - Message ID definitions for the F logging library

//! @file flog_msg_id.h
//! @author Nabeel Sowan (nabeel.sowan@vibes.se)
//!
//! Useful for saving space on embedded systems, performance etc.
//! if FLOG_NO_MSG_ID_STRINGS is defined then the static string
//! data will not be included. This can be used for deeply embedded
//! systems where string generation isn't strictly necessary,
//! and can be decoded by the receiver.


#ifndef FLOG_MSG_ID_H
#define FLOG_MSG_ID_H

#include "config.h"

//! Amount of msg_id entries reserved for passing errno values
#define FLOG_MSG_ID_AMOUNT_RESERVED_FOR_ERRNO 8000


//! Built in message ids, these are used internally by flog
#define FLOG_MSG_IDS_BUILTIN \
X(FLOG_MSG_MARK,                   "Mark"                  ) \
X(FLOG_MSG_ASSERTION_FAILED,       "Assertion failed"      ) \
X(FLOG_MSG_FUNCTION_START,         "Function start"        ) \
X(FLOG_MSG_FUNCTION_END,           "Function end"          )


//! Extended message ids, useful but not entirely necessary
#define FLOG_MSG_IDS_EXTENDED \
X(FLOG_MSG_CANNOT_READ_FROM_STDIN, "Cannot read from stdin") \
X(FLOG_MSG_CANNOT_READ_FILE,       "Cannot read from file" ) \
X(FLOG_MSG_CANNOT_OPEN_SOCKET,     "Cannot open socket"    )


//! Message ids for stdio output module
#ifdef FLOG_CONFIG_OUTPUT_STDIO
#define FLOG_MSG_IDS_OUTPUT_STDIO \
X(FLOG_MSG_CANNOT_WRITE_TO_STDOUT, "Cannot write to stdout") \
X(FLOG_MSG_CANNOT_WRITE_TO_STDERR, "Cannot write to stderr")
#else
#define FLOG_MSG_IDS_OUTPUT_STDIO
#endif


//! Message ids for file output module
#ifdef FLOG_CONFIG_OUTPUT_FILE
#define FLOG_MSG_IDS_OUTPUT_FILE \
X(FLOG_MSG_CANNOT_OPEN_FILE,       "Cannot open file"      ) \
X(FLOG_MSG_CANNOT_WRITE_FILE,      "Cannot write to file"  ) \
X(FLOG_MSG_SET_OUTPUT_FILE,        "Please set output file")
#else
#define FLOG_MSG_IDS_OUTPUT_FILE
#endif


// Custom message ids, if they are not defined, define to null
#ifndef FLOG_MSG_IDS_CUSTOM
#define FLOG_MSG_IDS_CUSTOM
#endif


//! List of message id lists to be used
#define FLOG_MSG_IDS \
FLOG_MSG_IDS_BUILTIN \
FLOG_MSG_IDS_EXTENDED \
FLOG_MSG_IDS_OUTPUT_STDIO \
FLOG_MSG_IDS_OUTPUT_FILE \
FLOG_MSG_IDS_CUSTOM


#define X(id, str) id,
typedef enum {
	FLOG_MSG_NONE = 0,
	FLOG_MSG_ID_LOWERBOUND = (FLOG_MSG_ID_AMOUNT_RESERVED_FOR_ERRNO-1),
	FLOG_MSG_IDS
	FLOG_MSG_ID_AMOUNT
} FLOG_MSG_ID_T;
#undef X


#endif //FLOG_MSG_ID_H
