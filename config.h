//! Config file for Flog

//! @file config.h
//! @author Nabeel Sowan (nabeel.sowan@vibes.se)
//!
//! Set these defines according to the needs of your project.


//! @def FLOG_ALLOW_NULL_MESSAGES
//! If FLOG_ALLOW_NULL_MESSAGES is defined, then messages with msg_id=0
//! and a NULL text string will be passed on as valid flog messages.
#define FLOG_ALLOW_NULL_MESSAGES


//! @def FLOG_MSG_TYPE_ENUM_API
//! FLOG_MSG_TYPE_ENUM_API allows using an enum API for flog message types.
//! What this means is that FLOG_MSG_TYPE_T will be defined as an int
//! (instead of unsigned char) In return, enums may yield stronger
//! type checking and therefore easier debugging.
#define FLOG_MSG_TYPE_ENUM_API


//! @def FLOG_ABORT_ON_ASSERT
//! If FLOG_ABORT_ON_ASSERT is defined then flog_assert() will
//! call abort() on assertion failure. This behaviour can be
//! switched off for deeply embedded systems where calling abort()
//! doesn't make any sense.
//#define FLOG_ABORT_ON_ASSERT


//! @def FLOG_TIMESTAMP
//! If FLOG_TIMESTAMP is defined then this activates timestamping
//! of flog messages, and time.h This requires working time routines
//! in libc.
#define FLOG_TIMESTAMP


//! @def FLOG_TIMESTAMP_USEC
//! If FLOG_TIMESTAMP_USEC is defined then this activates timestamping
//! of flog messages with millisecond (usec) accuracy. Requires
//! sys/time.h support.
//#define FLOG_TIMESTAMP_USEC


//! @def FLOG_SRC_INFO
//! If FLOG_SRC_INFO is defined then each flog message will contain
//! information about the specific file, line and function name
//! where flog_print(f) was called.
#define FLOG_SRC_INFO


//! @def FLOG_MSG_ID_STRINGS
//! If FLOG_MSG_ID_STRINGS is defined then the FLOG_MSG_ID string
//! data will be included. This can be omitted for deeply embedded
//! systems where string generation isn't strictly necessary,
//! and can be decoded by the receiver.
#define FLOG_MSG_ID_STRINGS


//! @def FLOG_STRING_HANDLING
//! If FLOG_STRING_HANDLING is defined then string handling routines
//! will be included in the flog_string module. Omitting this will
//! save a few k by avoiding some code from stdio etc. Useful for
//! cases where a deeply embedded system should only transmit flog
//! messages in binary form over a network.
#define FLOG_STRING_HANDLING


//! @def FLOG_ERRNO_STRINGS
//! If FLOG_ERRNO_STRINGS is defined then setting msg_id to a valid
//! libc errno will fetch the correct string from libc when outputting
//! a string.
#define FLOG_ERRNO_STRINGS


//! @def FLOG_MSG_ID_OUTPUT_SHOW_ID
//! If FLOG_MSG_ID_OUTPUT_SHOW_ID is defined then output of msg_id
//! messages will print the msg_id number (be it errno or a flog msg id)
#define FLOG_MSG_ID_SHOW_ID


//! @def FLOG_OUTPUT_STDIO
//! If FLOG_OUTPUT_STDIO is defined then flog will include the
//! stdio output module (stdout and stderr)
#define FLOG_OUTPUT_STDIO


//! @def FLOG_OUTPUT_FILE
//! If FLOG_OUTPUT_FILE is defined then flog will include the
//! simple file output module.
#define FLOG_OUTPUT_FILE
