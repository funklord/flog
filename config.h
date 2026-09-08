//! Config file for Flog

//! @file config.h
//! @author Nabeel Sowan (nabeel.sowan@vibes.se)
//!
//! Set these defines according to the needs of your project.


//! @def FLOG_CONFIG_ALLOW_NULL_MESSAGES
//! If defined, then messages with msg_id=0 and a NULL text string will
//! be passed on as valid flog messages.
#define FLOG_CONFIG_ALLOW_NULL_MESSAGES


//! @def FLOG_CONFIG_MSG_TYPE_ENUM_API
//! If defined, allows using an enum API for flog message types.
//! What this means is that flog_msg_type_t will be defined as an int
//! (instead of unsigned char) In return, enums may yield stronger
//! type checking and therefore easier debugging.
#define FLOG_CONFIG_MSG_TYPE_ENUM_API


//! @def FLOG_CONFIG_RECURSIVE_MAX_STACK_DEPTH
//! If defined, this option is used by the recursive functions flog_add_msg()
//! and flog_is_message_used(). It specifies the maximum stack depth they
//! are allowed to reach and thereby indirectly setting a limit on the
//! stack usage of flog. Usually each stack level allocates a flog_t struct
//! and a couple of arguments. To avoid infinite recursion caused by circular
//! sublog structures, set this to a sane value like 8-32.
#define FLOG_CONFIG_RECURSIVE_MAX_STACK_DEPTH 16


//! @def FLOG_CONFIG_ALLOCATION
//! If defined, flog may allocate. If not defined -- the default -- flog's
//! MESSAGE PATH performs no allocation: text is formatted into a fixed
//! buffer, the subsystem path is composed in place, sublogs are an intrusive
//! list, and a flog_t or flog_msg_t lives wherever the caller puts it.
//!
//! The whole library, not just the message path: with this undefined, `nm`
//! shows no reference to malloc, calloc, realloc, free, strdup, asprintf or
//! vasprintf in any of flog's objects.
//!
//! THE API IS THE SAME EITHER WAY. create_flog_t(), destroy_flog_t(), the
//! output constructors and the print macros keep their signatures and their
//! meaning; without an allocator the storage comes from fixed pools instead
//! of the heap, and an exhausted pool returns NULL exactly as a failed
//! malloc did. A program is written once and builds both ways.
//!
//! OFF BY DEFAULT, which is the deliberate part. A logger is the thing a
//! program reaches for when something has already gone wrong, and "already
//! gone wrong" includes being out of memory. It is also the piece most often
//! wanted on a target that has no allocator to offer, and by a library whose
//! own design is caller-owned storage and which cannot take a dependency
//! that mallocs behind its back.
//!
//! What is lost is unbounded message text: with allocation off, text longer
//! than FLOG_CONFIG_TEXT_MAX is truncated rather than growing a buffer.
//! Nothing else changes -- sublog trees, filtering, message ids, timestamps,
//! source info and the output plugins all work the same.
//#define FLOG_CONFIG_ALLOCATION


//! @def FLOG_CONFIG_MSG_BUFFER
//! If defined, a flog_t may hold messages in a buffer rather than passing
//! each to its output function immediately. Requires FLOG_CONFIG_ALLOCATION.
//!
//! BUFFERING IS THE ONLY PLACE FLOG ALLOCATES, and that is the whole shape of
//! the rule rather than a coincidence. A message passed straight to an output
//! borrows its strings from the caller's stack and is done with them before
//! the call returns. A message KEPT outlives the frame that made it, so its
//! strings have to be owned, and owning them is what needs an allocator.
//! Everything else flog does can be answered from fixed storage.
//!
//! So the two options are not independent: asking for buffering is asking for
//! allocation, and turning allocation off turns buffering off with it.
//#define FLOG_CONFIG_MSG_BUFFER


//! @def FLOG_CONFIG_MAX_LOGS
//! How many flog_t objects create_flog_t() can hand out when not allocating.
//!
//! THE API IS THE SAME EITHER WAY, which is the point of a pool rather than
//! a second set of functions. create_flog_t() and destroy_flog_t() mean the
//! same thing and have the same signatures in both configurations; what
//! changes is where the storage comes from. A consumer writes one program
//! and it builds on a target with an allocator and on one without.
//!
//! Exhausting the pool returns NULL, which is exactly what create_flog_t()
//! already does when malloc fails, so callers need no new error path.
#define FLOG_CONFIG_MAX_LOGS 16


//! @def FLOG_CONFIG_NAME_MAX
//! Longest log name kept when not allocating, including the terminator.
#define FLOG_CONFIG_NAME_MAX 32


//! @def FLOG_CONFIG_FILENAME_MAX
//! Longest output filename kept when not allocating, including the
//! terminator.
#define FLOG_CONFIG_FILENAME_MAX 128


//! @def FLOG_CONFIG_TEXT_MAX
//! Longest message text flog will format when not allocating, including the
//! terminator. Text beyond this is truncated.
#define FLOG_CONFIG_TEXT_MAX 512


//! @def FLOG_CONFIG_STR_MAX
//! Longest rendered message line flog will build when not allocating,
//! including the terminator. This bounds the whole formatted output line:
//! timestamp, source info, subsystem, type and text together.
#define FLOG_CONFIG_STR_MAX 1024


//! @def FLOG_CONFIG_ABORT_ON_ASSERT
//! If defined then flog_assert() will call abort() on assertion failure.
//! This behaviour can be switched off for deeply embedded systems where
//! calling abort() doesn't make any sense.
#define FLOG_CONFIG_ABORT_ON_ASSERT


//! @def FLOG_CONFIG_TIMESTAMP
//! If defined, then this activates timestamping of flog messages,
//! using time.h. This requires working time routines in libc.
#define FLOG_CONFIG_TIMESTAMP


//! @def FLOG_CONFIG_TIMESTAMP_USEC
//! If defined, then this activates timestamping of flog messages with
//! millisecond (usec) accuracy. Requires sys/time.h support.
#define FLOG_CONFIG_TIMESTAMP_USEC


//! @def FLOG_CONFIG_SRC_INFO
//! If defined, then each flog message will contain information about the
//! specific file, line and function name where flog_print(f) was called.
#define FLOG_CONFIG_SRC_INFO


//! @def FLOG_CONFIG_STRING_OUTPUT
//! If defined, then string output routines will be included in the
//! flog_string module. Omitting this will save a few k by avoiding
//! some code from stdio etc. Useful for cases where a deeply embedded
//! system should only transmit flog messages in binary form over a network.
#define FLOG_CONFIG_STRING_OUTPUT


//! @def FLOG_CONFIG_MSG_ID_STRINGS
//! If defined, then the FLOG_MSG_ID string data will be included.
//! This can be omitted for deeply embedded systems where string generation
//! isn't strictly necessary, and can be decoded by the receiver.
#define FLOG_CONFIG_MSG_ID_STRINGS


//! @def FLOG_CONFIG_MSG_ID_STRINGS_EXTENDED
//! If defined, a couple of useful message strings will be included in flog.
//! Undefine this to save a little bit of memory.
#define FLOG_CONFIG_MSG_ID_STRINGS_EXTENDED


//! @def FLOG_CONFIG_ERRNO_STRINGS
//! If defined, then setting msg_id to a valid libc errno will fetch the
//! correct string from libc when outputing a string.
#define FLOG_CONFIG_ERRNO_STRINGS


//! @def FLOG_CONFIG_OUTPUT_SHOW_MSG_ID
//! If defined, then output of msg_id messages will print the msg_id
//! number (be it errno or a flog msg id)
#define FLOG_CONFIG_OUTPUT_SHOW_MSG_ID


//! @def FLOG_CONFIG_OUTPUT_STDIO
//! If defined, then flog will include the stdio output module (stdout and stderr)
#define FLOG_CONFIG_OUTPUT_STDIO


//! @def FLOG_CONFIG_OUTPUT_FILE
//! If defined, then flog will include the simple file output module.
#define FLOG_CONFIG_OUTPUT_FILE
