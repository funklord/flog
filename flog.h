//! Flog - The F logging library

//! @file flog.h
//! @author Nabeel Sowan (nabeel.sowan@vibes.se)
//!
//! Useful as the main logger of a program or embedded system.
//! Requires C99 + GNU support


#ifndef FLOG_H
#define FLOG_H

//! Ask the libc for everything it has, before any system header is pulled in.
//!
//! UNCONDITIONAL AND FIRST, and neither is negotiable. A feature-test macro
//! only works if it is set before the headers it affects are read, so it
//! cannot be decided by a test that needs those headers to have been read
//! already -- the request has to come first and the finding out comes after.
//!
//! It was briefly made conditional on the fallbacks not being wanted, on the
//! reasoning that a strict-C99 target should not be asking for extensions.
//! That is wrong twice. It gates far more than the two functions below --
//! every GNU feature this file or a later one might use disappears with it,
//! silently and at a distance from anything that mentions asprintf. And a
//! consumer that includes a libc header before this one gets no _GNU_SOURCE
//! effect at all, so the macro has to be as early and as unconditional as it
//! can be to work even that well.
//!
//! Asking for extensions costs nothing where they do not exist. What is
//! selectable is whether flog USES the platform's asprintf and strdup, which
//! is decided below, after a header has been read.
#define _GNU_SOURCE

#include "config.h"
#include "flog_msg_id.h"
//! stdio.h is included here for its side effect as much as its contents: it
//! is what brings in the libc's own identification macros, and the test below
//! cannot ask which libc this is until one of its headers has been read.
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

//! @def FLOG_HAVE_ASPRINTF
//! Whether the platform supplies asprintf() and vasprintf().
//!
//! DETECTED RATHER THAN CONFIGURED, because the platform already knows and a
//! hand-edited switch is a second place for the same fact to be wrong. Where
//! the system has them flog uses the system's; where it does not, flog
//! supplies its own out of C99 alone (see flog.c).
//!
//! They are a GNU extension that the BSDs also carry, and POSIX did not
//! specify them until POSIX.1-2024 -- so a strict C99 target, or an older
//! POSIX one, is the case the fallback exists for.
//!
//! Define FLOG_HAVE_ASPRINTF or FLOG_NO_ASPRINTF ahead of this header to
//! settle it by hand where the detection is wrong. Getting it wrong in the
//! quiet direction costs nothing but flog's own implementation being used on
//! a platform that had one.
//! @def FLOG_HOSTED_LIBC
//! A libc known to carry both asprintf() and strdup().
//!
//! One list for both because it is one question in practice: these are the
//! hosted platforms flog is built on, and every one of them has had both for
//! years. Splitting the detection would suggest a platform exists with one
//! and not the other, which would be a distinction invented for tidiness.
#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || \
    defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__) || \
    defined(__BIONIC__) || defined(__CYGWIN__)
#define FLOG_HOSTED_LIBC
#endif

#if !defined(FLOG_HAVE_ASPRINTF) && !defined(FLOG_NO_ASPRINTF)
#if defined(FLOG_HOSTED_LIBC) || \
    (defined(_POSIX_VERSION) && _POSIX_VERSION >= 202405L)
#define FLOG_HAVE_ASPRINTF
#endif
#endif

//! @def FLOG_HAVE_STRDUP
//! Whether the platform supplies strdup().
//!
//! FOUND BY BUILDING THE FALLBACK PATH RATHER THAN BY READING. flog uses
//! strdup() 21 times, and strdup is POSIX.1-2001 and C23 -- not C99. So
//! switching asprintf to the fallback and compiling with -std=c99 did not
//! produce a strict-C99 build, it produced a build that failed on strdup.
//! A switch that does not work in the case it exists for is worse than no
//! switch, which is why this is here rather than left to the porter.
#if !defined(FLOG_HAVE_STRDUP) && !defined(FLOG_NO_STRDUP)
#if defined(FLOG_HOSTED_LIBC) || \
    (defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200809L) || \
    (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L)
#define FLOG_HAVE_STRDUP
#endif
#endif

#ifndef FLOG_HAVE_ASPRINTF
//! Supplied by flog where the platform has none. Same contract as GNU
//! asprintf(): on success a newly allocated NUL-terminated string is stored
//! in *strp and its length excluding the terminator returned; on failure -1
//! is returned and *strp is not touched.
int asprintf(char **strp, const char *fmt, ...);
int vasprintf(char **strp, const char *fmt, va_list ap);
#endif //FLOG_HAVE_ASPRINTF

#ifndef FLOG_HAVE_STRDUP
//! Supplied by flog where the platform has none. Same contract as POSIX
//! strdup(): a newly allocated copy of s, or NULL if it cannot be made.
char *strdup(const char *s);
#endif //FLOG_HAVE_STRDUP

#ifdef FLOG_CONFIG_TIMESTAMP
#ifdef FLOG_CONFIG_TIMESTAMP_USEC
#include <sys/time.h>
#include <time.h>
typedef struct timeval flog_timestamp_t;
#else //FLOG_CONFIG_TIMESTAMP_USEC
#include <time.h>
typedef time_t flog_timestamp_t;
#endif //FLOG_CONFIG_TIMESTAMP_USEC
#endif //FLOG_CONFIG_TIMESTAMP


#ifdef FLOG_CONFIG_MSG_TYPE_ENUM_API

//! @addtogroup flog_msg_type_t
//! @brief Types of messages supported by FLOG
//! @details Use where ever a @ref flog_msg_type_t is referred
//! @{

typedef enum flog_msg_type {
	FLOG_NONE        = 0x00,
	FLOG_NOTHING     = FLOG_NONE,

	FLOG_CRIT        = 0x01,
	FLOG_CRITICAL    = FLOG_CRIT,

	FLOG_ERR         = 0x02,
	FLOG_ERROR       = FLOG_ERR,

	FLOG_WARN        = 0x04,
	FLOG_WARNING     = FLOG_WARN,
	FLOG_ALERT       = FLOG_WARN,

	FLOG_NOTE        = 0x08,
	FLOG_NOTIFY      = FLOG_NOTE,
	FLOG_IMP         = FLOG_NOTE,
	FLOG_IMPORTANT   = FLOG_NOTE,

	FLOG_INFO        = 0x10,
	FLOG_INFORMATION = FLOG_INFO,
	FLOG_MSG         = FLOG_INFO,
	FLOG_MESSAGE     = FLOG_INFO,

	FLOG_VINFO       = 0x20,
	FLOG_VERBOSE     = FLOG_VINFO,

	FLOG_DEBUG       = 0x40,

	FLOG_DEEP_DEBUG  = 0x80,

	FLOG_ACCEPT_ONLY_CRITICAL     = FLOG_CRIT,
	FLOG_ACCEPT_ONLY_ERROR        = FLOG_CRIT | FLOG_ERR,
	FLOG_ACCEPT_ERROR_AND_WARNING = FLOG_CRIT | FLOG_ERR | FLOG_WARN,
	FLOG_ACCEPT_IMPORTANT_NOTES   = FLOG_CRIT | FLOG_ERR | FLOG_WARN | FLOG_NOTE,
	FLOG_ACCEPT_INFO              = FLOG_CRIT | FLOG_ERR | FLOG_WARN | FLOG_NOTE | FLOG_INFO,
	FLOG_ACCEPT_VERBOSE_INFO      = FLOG_CRIT | FLOG_ERR | FLOG_WARN | FLOG_NOTE | FLOG_INFO | FLOG_VINFO,
	FLOG_ACCEPT_DEBUG             = FLOG_CRIT | FLOG_ERR | FLOG_WARN | FLOG_NOTE | FLOG_INFO | FLOG_VINFO | FLOG_DEBUG,
	FLOG_ACCEPT_DEEP_DEBUG        = FLOG_CRIT | FLOG_ERR | FLOG_WARN | FLOG_NOTE | FLOG_INFO | FLOG_VINFO | FLOG_DEBUG | FLOG_DEEP_DEBUG,
	FLOG_ACCEPT_ALL               = FLOG_ACCEPT_DEBUG
} flog_msg_type_t;

//! @}

#else //FLOG_CONFIG_MSG_TYPE_ENUM_API


//! @addtogroup flog_msg_type_t
//! @brief Types of messages supported by FLOG
//! @details Use where ever a @ref flog_msg_type_t is referred
//! @{

typedef uint_fast8_t flog_msg_type_t;

//! Nothing
#define FLOG_NONE         0x00
#define FLOG_NOTHING      FLOG_NONE

//! Critical error
#define FLOG_CRIT         0x01
#define FLOG_CRITICAL     FLOG_CRIT

//! Error
#define FLOG_ERR          0x02
#define FLOG_ERROR        FLOG_ERR

//! Warning
#define FLOG_WARN         0x04
#define FLOG_WARNING      FLOG_WARN
#define FLOG_ALERT        FLOG_WARN

//! Note
#define FLOG_NOTE         0x08
#define FLOG_NOTIFY       FLOG_NOTE
#define FLOG_IMP          FLOG_NOTE
#define FLOG_IMPORTANT    FLOG_NOTE

//! Info
#define FLOG_INFO         0x10
#define FLOG_INFORMATION  FLOG_INFO
#define FLOG_MSG          FLOG_INFO
#define FLOG_MESSAGE      FLOG_INFO

//! Info in verbose mode
#define FLOG_VINFO        0x20
#define FLOG_VERBOSE      FLOG_VINFO

//! Debug info
#define FLOG_DEBUG        0x40

//! Deep debug info (such as when functions are started etc. )
#define FLOG_DEEP_DEBUG   0x80

//! @}


//! @addtogroup FLOG_ACCEPT_BITMASKS
//! @brief Bitmasks for filtering messages
//! @details Set the variable @ref flog_t->accepted_message_type
//! @{

//! Bitmask to accept only critical
#define FLOG_ACCEPT_ONLY_CRITICAL     FLOG_CRIT
//! Bitmask to accept only errors
#define FLOG_ACCEPT_ONLY_ERROR        FLOG_CRIT | FLOG_ERR
//! Bitmask to accept error and warning
#define FLOG_ACCEPT_ERROR_AND_WARNING FLOG_CRIT | FLOG_ERR | FLOG_WARN
//! Bitmask to accept all important messages
#define FLOG_ACCEPT_IMPORTANT_NOTE    FLOG_CRIT | FLOG_ERR | FLOG_WARN | FLOG_NOTE
//! Bitmask to accept informational messages
#define FLOG_ACCEPT_INFO              FLOG_CRIT | FLOG_ERR | FLOG_WARN | FLOG_NOTE | FLOG_INFO
//! Bitmask to accept verbose messages
#define FLOG_ACCEPT_VERBOSE_INFO      FLOG_CRIT | FLOG_ERR | FLOG_WARN | FLOG_NOTE | FLOG_INFO | FLOG_VINFO
//! Bitmask to accept all messages (except deep debug)
#define FLOG_ACCEPT_DEBUG             FLOG_CRIT | FLOG_ERR | FLOG_WARN | FLOG_NOTE | FLOG_INFO | FLOG_VINFO | FLOG_DEBUG
//! Bitmask to accept all messages
#define FLOG_ACCEPT_DEEP_DEBUG        FLOG_CRIT | FLOG_ERR | FLOG_WARN | FLOG_NOTE | FLOG_INFO | FLOG_VINFO | FLOG_DEBUG | FLOG_DEEP_DEBUG
//! Bitmask to accept all messages (except deep debug)
#define FLOG_ACCEPT_ALL               FLOG_ACCEPT_DEBUG

//! @}

#endif //FLOG_CONFIG_MSG_TYPE_ENUM_API


// Macros to insert source info into print strings
// Maybe it is better to use __func__ than __FUNCTION__ ?


//! emit an flog message

//! use this when you need to emit simple text messages and flog_printf() when formatting is needed
//! @param[in,out] p log to emit message to
//! @param[in] subsystem which part of the program is outputing this message
//! @param[in] type use one of the FLOG_* defines
//! @param[in] msg_id optionally use errno or one of the FLOG_MSG_* defines
//! @param[in] text message text
//! @retval 0 success
//! @retval 1 error while adding message to log
//! @retval 2 error unable to get time
//! @retval 3 did not add null message (flog is configured not to allow null messages)
//! @see _flog_print(), flog_printf(), flog_dprint()
#ifdef FLOG_CONFIG_SRC_INFO
#define flog_print(p, subsystem, type, msg_id, text) _flog_print(p,subsystem,__FILE__,__LINE__,__FUNCTION__,type,msg_id,text)
#else
#define flog_print(p, subsystem, type, msg_id, text) _flog_print(p,subsystem,type,msg_id,text)
#endif


//! emit a formatted flog message (calls flog_print())

//! use this when you need to emit formatted text messages and flog_print() when no formatting is needed
//! @param[in,out] p log to emit message to
//! @param[in] subsystem which part of the program is outputing this message
//! @param[in] type use one of the FLOG_* defines
//! @param[in] msg_id optionally use errno or one of the FLOG_MSG_* defines
//! @param[in] ... formatted message text
//! @retval 0 success
//! @retval 1 error while adding message to log
//! @retval 2 error unable to get time
//! @retval 3 did not add null message (flog is configured not to allow null messages)
//! @see _flog_printf(), flog_print(), flog_dprintf()
#ifdef FLOG_CONFIG_SRC_INFO
#define flog_printf(p, subsystem, type, msg_id, ...) _flog_printf(p,subsystem,__FILE__,__LINE__,__FUNCTION__,type,msg_id,__VA_ARGS__)
#else
#define flog_printf(p, subsystem, type, msg_id, ...) _flog_printf(p,subsystem,type,msg_id,__VA_ARGS__)
#endif


//! @addtogroup flog_runtime_debug_macros
//! @brief Macros for runtime debugging
//! @details Use these macros for various debugging purposes
//! @{


//! Macro to signify function start

//! Use this macro for deep debugging of program flow
#define flog_function_start(p, subsystem) flog_printf(p,subsystem,FLOG_DEEP_DEBUG,FLOG_MSG_FUNCTION_START,"%s()",__FUNCTION__)


//! Macro to signify function end

//! Use this macro for deep debugging of program flow
#define flog_function_end(p, subsystem) flog_printf(p,subsystem,FLOG_DEEP_DEBUG,FLOG_MSG_FUNCTION_END,"%s()",__FUNCTION__)


//! Macro for flog assert functionality

//! @param[in,out] p log to emit message to
//! @param[in] cond statement to evaluate
#ifdef FLOG_CONFIG_ABORT_ON_ASSERT
#define flog_assert(p, cond) \
{ \
	if(!(cond)) { \
		flog_printf(p,NULL,FLOG_ERROR,FLOG_MSG_ASSERTION_FAILED,#cond); \
		abort(); \
	} \
}
#else //FLOG_CONFIG_ABORT_ON_ASSERT
#define flog_assert(p, cond) \
{ \
	if(!(cond)) \
		flog_printf(p,NULL,FLOG_ERROR,FLOG_MSG_ASSERTION_FAILED,#cond); \
}
#endif //FLOG_CONFIG_ABORT_ON_ASSERT


//! @}


//! @addtogroup flog_buildtime_debug_macros
//! @brief Macros that are only defined when DEBUG is defined
//! @details Use these macros to allow removal of messages from release builds
//! @{


//! Same as flog_print() but only defined if DEBUG is set

//! Use this macro to allow removal of messages from release builds
//! @see flog_print()
#ifdef DEBUG
#define flog_dprint(p, subsystem, type, msg_id, text) flog_print(p,subsystem,type,msg_id,text)
#else
#define flog_dprint(p, subsystem, type, msg_id, text) (void)(0)
#endif


//! Same as flog_printf() but only defined if DEBUG is set

//! Use this macro to allow removal of messages from release builds
//! @see flog_printf()
#ifdef DEBUG
#define flog_dprintf(p, subsystem, type, msg_id, ...) flog_printf(p,subsystem,type,msg_id,__VA_ARGS__)
#else
#define flog_dprintf(p, subsystem, type, msg_id, ...) (void)(0)
#endif


//! Same as flog_function_start() but only defined if DEBUG is set

//! Use this macro to allow removal of messages from release builds
//! @see flog_function_start()
#ifdef DEBUG
#define flog_debug_function_start(p, subsystem) flog_function_start(p, subsystem)
#else
#define flog_debug_function_start(p, subsystem) (void)(0)
#endif


//! Same as flog_function_end() but only defined if DEBUG is set

//! Use this macro to allow removal of messages from release builds
//! @see flog_function_end()
#ifdef DEBUG
#define flog_debug_function_end(p, subsystem) flog_function_end(p, subsystem)
#else
#define flog_debug_function_end(p, subsystem) (void)(0)
#endif


//! @}


//! Message structure - Holds all data related to a single message

//! The string fields are `const` because most producers of a flog_msg_t
//! (_flog_print(), _flog_printf(), flog_add_msg()'s propagation to
//! sublogs) borrow pointers they do not own (string literals, __FILE__/
//! __FUNCTION__, a caller's buffer) rather than copying them -- only
//! create_flog_msg_t()/destroy_flog_msg_t() form an owning pair (via
//! strdup()/free()), and destroy_flog_msg_t() casts the const away
//! explicitly at that one legitimate free site. Do not free these fields
//! through any other path.
typedef struct {
	const char *subsystem;                  //!< subsystem which is outputting the msg
#ifdef FLOG_CONFIG_TIMESTAMP
	flog_timestamp_t timestamp;             //!< timestamp
#endif
#ifdef FLOG_CONFIG_SRC_INFO
	const char *src_file;                   //!< source file emitting message
	uint_fast16_t src_line;                 //!< source line number emitting message
	const char *src_func;                   //!< source function emitting message
#endif
	flog_msg_type_t type;                   //!< type of message
	flog_msg_id_t msg_id;                   //!< message id (instead of, or with text) see flog_msg_id.h
	const char *text;                       //!< message text
} flog_msg_t;


//! Main log structure - typedefined as @ref flog_t

//! These can be appended to each other in a tree structure (by using flog_append_sublog())
//! to form good flow and structure in software.
//! Sublogs are created for 3 main purposes: namespacing, multiple outputs and filtering
typedef struct flog_t {
	char *name;                             //!< name of log
	flog_msg_type_t accepted_msg_type;      //!< bitmask of which messages to accept
	int (*output_func)(struct flog_t *,const flog_msg_t *); //!< function to output messages to
	void *output_func_data;                 //!< data passed to output func
	uint_fast16_t output_error;             //!< errors occurred on output
	uint_fast8_t output_stop_on_error;      //!< stop outputting messages on error
	struct flog_t *error_log;               //!< error log for flog errors
	flog_msg_t **msg;                       //!< array of messages
	uint_fast16_t msg_amount;               //!< amount of messages in array
	uint_fast16_t msg_max;                  //!< maximum amount of buffered messages
	struct flog_t **sublog;                 //!< array of sublogs
	uint_fast8_t sublog_amount;             //!< amount of sublogs in array
} flog_t;


void init_flog_msg_t(flog_msg_t *p);

flog_msg_t * create_flog_msg_t(const char *subsystem,
#ifdef FLOG_CONFIG_TIMESTAMP
                               flog_timestamp_t timestamp,
#endif
#ifdef FLOG_CONFIG_SRC_INFO
                               const char *src_file,uint_fast16_t src_line,const char *src_func,
#endif
                               flog_msg_type_t msg_type,flog_msg_id_t msg_id,const char *text);

void destroy_flog_msg_t(flog_msg_t *p);

void init_flog_t(flog_t *p);
flog_t * create_flog_t(const char *name, flog_msg_type_t accepted_msg_type);
void destroy_flog_t(flog_t *p);

int flog_add_msg(flog_t *p,flog_msg_t *msg);
void flog_clear_msg_buffer(flog_t *p);
int flog_append_sublog(flog_t *p,flog_t *sublog);

#ifdef FLOG_CONFIG_SRC_INFO
int _flog_print(flog_t *p,const char *subsystem,const char *src_file,uint_fast16_t src_line,const char *src_func,flog_msg_type_t type,flog_msg_id_t msg_id,const char *text);
int _flog_printf(flog_t *p,const char *subsystem,const char *src_file,uint_fast16_t src_line,const char *src_func,flog_msg_type_t type,flog_msg_id_t msg_id,const char *textf, ...);
#else
int _flog_print(flog_t *p,const char *subsystem,flog_msg_type_t type,flog_msg_id_t msg_id,const char *text);
int _flog_printf(flog_t *p,const char *subsystem,flog_msg_type_t type,flog_msg_id_t msg_id,const char *textf, ...);
#endif

#ifdef DEBUG
void flog_test(flog_t *p);
#endif

#endif
