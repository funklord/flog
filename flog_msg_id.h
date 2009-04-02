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
X(FLOG_MSG_ASSERTION_FAILED,       "Assertion failed"      )


//! Common message ids, used by the various output modules
#define FLOG_MSG_IDS_COMMON \
X(FLOG_MSG_CANNOT_READ_FROM_STDIN, "cannot read from stdin") \
X(FLOG_MSG_CANNOT_WRITE_TO_STDOUT, "cannot write to stdout") \
X(FLOG_MSG_CANNOT_WRITE_TO_STDERR, "cannot write to stderr") \
X(FLOG_MSG_CANNOT_OPEN_FILE,       "cannot open file"      ) \
X(FLOG_MSG_CANNOT_READ_FILE,       "cannot read from file" ) \
X(FLOG_MSG_CANNOT_WRITE_FILE,      "cannot write to file"  )


//! Extended message ids, useful but not entirely necessary
#define FLOG_MSG_IDS_EXTENDED \
X(FLOG_MSG_CANNOT_OPEN_SOCKET,     "cannot open socket"    )


// Custom message ids, if they are not defined, define to null
#ifndef FLOG_MSG_IDS_CUSTOM
#define FLOG_MSG_IDS_CUSTOM
#endif


//! List of message id lists to be used
#define FLOG_MSG_IDS \
FLOG_MSG_IDS_BUILTIN \
FLOG_MSG_IDS_COMMON \
FLOG_MSG_IDS_EXTENDED \
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
