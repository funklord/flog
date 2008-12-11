/*! 
	@file flog.h
	@brief Flog library
	
	Useful as the main logger of a program 
*/

//! Nothing
#define FLOG_NOTHING			0x00
#define FLOG_NONE					FLOG_NOTHING

//! Critical error
#define FLOG_CRITICAL			0x01
#define FLOG_CRIT					FLOG_CRITICAL

//! Error
#define FLOG_ERROR				0x02
#define FLOG_ERR					FLOG_ERROR

//! Warning
#define FLOG_WARNING			0x04
#define FLOG_WARN					FLOG_WARNING
#define FLOG_ALERT				FLOG_WARNING

//! Note
#define FLOG_NOTE					0x08
#define FLOG_NOTIFY				FLOG_NOTE
#define FLOG_IMP					FLOG_NOTE
#define FLOG_IMPORTANT		FLOG_NOTE

//! Info
#define FLOG_INFO					0x10
#define FLOG_INFORMATION	FLOG_INFO
#define FLOG_MSG					FLOG_INFO
#define FLOG_MESSAGE			FLOG_INFO

//! Info in verbose mode
#define FLOG_VERBOSE			0x20
#define FLOG_VINFO				FLOG_VERBOSE

//! Debug info
#define FLOG_DEBUG				0x40

//! Debug info for flog itself
#define FLOG_FLOG_DEBUG		0x80

//! Bitmask to show only errors
#define FLOG_SHOW_ONLY_ERRORS					FLOG_CRIT | FLOG_ERR
#define FLOG_SHOW_ERRORS_AND_WARNINGS	FLOG_CRIT | FLOG_ERR | FLOG_WARN
#define FLOG_SHOW_IMPORTANT_NOTES			FLOG_CRIT | FLOG_ERR | FLOG_WARN | FLOG_NOTE
#define FLOG_SHOW_INFO								FLOG_CRIT | FLOG_ERR | FLOG_WARN | FLOG_NOTE | FLOG_INFO
#define FLOG_SHOW_VERBOSE_INFO				FLOG_CRIT | FLOG_ERR | FLOG_WARN | FLOG_NOTE | FLOG_INFO | FLOG_VINFO
#define FLOG_SHOW_ALL									FLOG_CRIT | FLOG_ERR | FLOG_WARN | FLOG_NOTE | FLOG_INFO | FLOG_VINFO | FLOG_DEBUG
#define FLOG_SHOW_FLOG_DEBUG					FLOG_CRIT | FLOG_ERR | FLOG_WARN | FLOG_NOTE | FLOG_INFO | FLOG_VINFO | FLOG_DEBUG | FLOG_FLOG_DEBUG

//! Macros to allow removal of messages from release builds
#ifdef DEBUG
#define flog_dprint(p, type, subsystem, text) flog_print (p, type, subsystem, text)
#define flog_dprintf(p, type, subsystem, ...) flog_printf (p, type, subsystem, __VA_ARGS__)
#else
#define flog_dprint(p, type, subsystem, text) 0
#define flog_dprintf(p, type, subsystem, ...) 0
#endif

typedef int FLOG_MSG_TYPE_T;
#ifdef FLOG_TIMESTAMP
typedef int FLOG_TIMESTAMP_T;
#endif

typedef struct {
#ifdef FLOG_TIMESTAMP
	FLOG_TIMESTAMP_T time;                  //!< timestamp
#endif
	FLOG_MSG_TYPE_T type;                   //!< type of message
	char *subsystem;                        //!< subsystem which is outputting the msg
	char *text;                              //!< message contents
} FLOG_MSG_T;

struct flog_t {
	char *name;                             //!< name of log
	FLOG_MSG_TYPE_T accepted_msg_type;      //!< bitmask of which messages to accept
	int (*output_func)(const FLOG_MSG_T *,void *); //!< function to output messages to
	int output_error;                       //!< errors occurred on output
	int output_stop_on_error;               //!< stop outputting messages on error
	struct flog_t *error_log;               //!< error log for flog errors
	FLOG_MSG_T **msg;                       //!< array of messages
	int msg_amount;                         //!< amount of messages in array
	int msg_max;                            //!< maximum amount of buffered messages
	struct flog_t **sublog;                 //!< array of sublogs
	int sublog_amount;                      //!< amount of sublogs in array
};

typedef struct flog_t FLOG_T;

void init_flog_msg_t(FLOG_MSG_T *p);
FLOG_MSG_T * create_flog_msg_t(FLOG_MSG_TYPE_T msg_type,const char *subsystem,const char *text);
void destroy_flog_msg_t(FLOG_MSG_T *p);

void init_flog_t(FLOG_T *p);
FLOG_T * create_flog_t(const char *name, FLOG_MSG_TYPE_T accepted_msg_type);
void destroy_flog_t(FLOG_T *p);

int flog_add_msg(FLOG_T *p,FLOG_MSG_T *msg);
void flog_clear_msg_buffer(FLOG_T *p);
int flog_append_sublog(FLOG_T *p,FLOG_T *sublog);

int flog_print(FLOG_T *p,FLOG_MSG_TYPE_T type,const char *subsystem,const char *text);
int flog_printf(FLOG_T *p,FLOG_MSG_TYPE_T type,const char *subsystem,const char *textf, ...);

char * flog_msg_t_to_str(const FLOG_MSG_T *p);
char * flog_get_msg_type_str(FLOG_MSG_TYPE_T type);
void flog_test(FLOG_T *p);

#ifdef FLOG_TIMESTAMP
const char * get_timestamp(void);
#endif
