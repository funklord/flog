//! Flog - The F logging library

//! @file flog.h
//! @author Nabeel Sowan (nabeel.sowan@vibes.se)
//!
//! Useful as the main logger of a program or embedded system.
//! Requires C99 + GNU support


#ifndef FLOG_H
#define FLOG_H

//! We need asprintf() for flog.c and flog_string.c
#define _GNU_SOURCE

#include "config.h"
#include "flog_msg_id.h"
#include <stdint.h>

#ifdef FLOG_CONFIG_TIMESTAMP
#ifdef FLOG_CONFIG_TIMESTAMP_USEC
#include <sys/time.h>
#include <time.h>
typedef struct timeval FLOG_TIMESTAMP_T;
#else //FLOG_CONFIG_TIMESTAMP_USEC
#include <time.h>
typedef time_t FLOG_TIMESTAMP_T;
#endif //FLOG_CONFIG_TIMESTAMP_USEC
#endif //FLOG_CONFIG_TIMESTAMP


#ifdef FLOG_CONFIG_MSG_TYPE_ENUM_API

//! @addtogroup FLOG_MSG_TYPE_T
//! @brief Types of messages supported by FLOG
//! @details Use where ever a @ref FLOG_MSG_TYPE_T is referred
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
} FLOG_MSG_TYPE_T;

//! @}

#else //FLOG_CONFIG_MSG_TYPE_ENUM_API


//! @addtogroup FLOG_MSG_TYPE_T
//! @brief Types of messages supported by FLOG
//! @details Use where ever a @ref FLOG_MSG_TYPE_T is referred
//! @{

typedef uint_fast8_t FLOG_MSG_TYPE_T;

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
//! @details Set the variable @ref FLOG_T->accepted_message_type
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
#define flog_dprint(p, subsystem, type, msg_id, text) (0)
#endif


//! Same as flog_printf() but only defined if DEBUG is set

//! Use this macro to allow removal of messages from release builds
//! @see flog_printf()
#ifdef DEBUG
#define flog_dprintf(p, subsystem, type, msg_id, ...) flog_printf(p,subsystem,type,msg_id,__VA_ARGS__)
#else
#define flog_dprintf(p, subsystem, type, msg_id, ...) (0)
#endif


//! Same as flog_function_start() but only defined if DEBUG is set

//! Use this macro to allow removal of messages from release builds
//! @see flog_function_start()
#ifdef DEBUG
#define flog_debug_function_start(p, subsystem) flog_function_start(p, subsystem)
#else
#define flog_debug_function_start(p, subsystem) (0)
#endif


//! Same as flog_function_end() but only defined if DEBUG is set

//! Use this macro to allow removal of messages from release builds
//! @see flog_function_end()
#ifdef DEBUG
#define flog_debug_function_end(p, subsystem) flog_function_end(p, subsystem)
#else
#define flog_debug_function_end(p, subsystem) (0)
#endif


//! @}


//! Message structure - Holds all data related to a single message
typedef struct {
	char *subsystem;                        //!< subsystem which is outputting the msg
#ifdef FLOG_CONFIG_TIMESTAMP
	FLOG_TIMESTAMP_T timestamp;             //!< timestamp
#endif
#ifdef FLOG_CONFIG_SRC_INFO
	char *src_file;                         //!< source file emitting message
	uint_fast16_t src_line;                 //!< source line number emitting message
	char *src_func;                         //!< source function emitting message
#endif
	FLOG_MSG_TYPE_T type;                   //!< type of message
	FLOG_MSG_ID_T msg_id;                   //!< message id (instead of, or with text) see flog_msg_id.h
	char *text;                             //!< message text
} FLOG_MSG_T;


//! Main log structure - typedefined as @ref FLOG_T

//! These can be appended to each other in a tree structure (by using flog_append_sublog())
//! to form good flow and structure in software.
//! Sublogs are created for 3 main purposes: namespacing, multiple outputs and filtering
typedef struct flog_t {
	char *name;                             //!< name of log
	FLOG_MSG_TYPE_T accepted_msg_type;      //!< bitmask of which messages to accept
	int (*output_func)(struct flog_t *,const FLOG_MSG_T *); //!< function to output messages to
	void *output_func_data;                 //!< data passed to output func
	uint_fast8_t output_error;              //!< errors occurred on output
	uint_fast8_t output_stop_on_error;      //!< stop outputting messages on error
	struct flog_t *error_log;               //!< error log for flog errors
	FLOG_MSG_T **msg;                       //!< array of messages
	uint_fast16_t msg_amount;               //!< amount of messages in array
	uint_fast16_t msg_max;                  //!< maximum amount of buffered messages
	struct flog_t **sublog;                 //!< array of sublogs
	uint_fast8_t sublog_amount;             //!< amount of sublogs in array
} FLOG_T;


void init_flog_msg_t(FLOG_MSG_T *p);

FLOG_MSG_T * create_flog_msg_t(const char *subsystem,
#ifdef FLOG_CONFIG_TIMESTAMP
                               FLOG_TIMESTAMP_T timestamp,
#endif
#ifdef FLOG_CONFIG_SRC_INFO
                               const char *src_file,int src_line,const char *src_func,
#endif
                               FLOG_MSG_TYPE_T msg_type,FLOG_MSG_ID_T msg_id,const char *text);

void destroy_flog_msg_t(FLOG_MSG_T *p);

void init_flog_t(FLOG_T *p);
FLOG_T * create_flog_t(const char *name, FLOG_MSG_TYPE_T accepted_msg_type);
void destroy_flog_t(FLOG_T *p);

int flog_add_msg(FLOG_T *p,FLOG_MSG_T *msg);
void flog_clear_msg_buffer(FLOG_T *p);
int flog_append_sublog(FLOG_T *p,FLOG_T *sublog);

#ifdef FLOG_CONFIG_SRC_INFO
int _flog_print(FLOG_T *p,const char *subsystem,const char *src_file,int src_line,const char *src_func,FLOG_MSG_TYPE_T type,FLOG_MSG_ID_T msg_id,const char *text);
int _flog_printf(FLOG_T *p,const char *subsystem,const char *src_file,int src_line,const char *src_func,FLOG_MSG_TYPE_T type,FLOG_MSG_ID_T msg_id,const char *textf, ...);
#else
int _flog_print(FLOG_T *p,const char *subsystem,FLOG_MSG_TYPE_T type,FLOG_MSG_ID_T msg_id,const char *text);
int _flog_printf(FLOG_T *p,const char *subsystem,FLOG_MSG_TYPE_T type,FLOG_MSG_ID_T msg_id,const char *textf, ...);
#endif

#ifdef DEBUG
void flog_test(FLOG_T *p);
#endif

#endif
