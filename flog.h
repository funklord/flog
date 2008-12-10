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

//#define FLOG_DEBUGGING_ON

#ifdef FLOG_DEBUGGING_ON
#define flog_dprint(type, subsystem, text) flog_print (type, subsystem, text)
#define flog_dprintf(type, subsystem, ...) flog_printf (type, subsystem, __VA_ARGS__)
#else
#define flog_dprint(type, subsystem, text) 0
#define flog_dprintf(type, subsystem, ...) 0
#endif

//! Contains flog main settings
struct struct_flog_opt
{
	//! @todo implement this
	char subsystem_prepend[512]; //!< string to prepend to subsystem
	
	//! @todo implement this
	int logbuffer_lines; //!< amount of log buffer lines
	
	int accepted_messages_stderr; //!< bitmask of which messages to write to stderr
	
	int accepted_messages_stdout; //!< bitmask of which messages to write to stdout

	//Logfile options
	int accepted_messages_file; //!< bitmask of which messages to write to file
	
	char logfile[261]; //!< log filename
	
	//! @todo implement this
	int buffered_write; //!< if set buffers writes in memory
	
	int write_error; //!< if set a write error has occured and no more writes will be done until unset
} flog_opt;

//! Initialise flog lib.
/*!
	must be called first.
	@param log_name name of log, appended to messages
	@param am_stderr accepted messages for stderr
	@param am_stdout accepted messages for stdout
	@param am_file accepted messages for file output
	@param filename_log name of logfile
	@return 0 = no errors occured
*/
int flog_init(const char *log_name,int am_stderr,int am_stdout,int am_file,const char *filename_log);

//! output an flog message
/*!
	@param type use one of the FLOG_* defines
	@param subsystem which part of the program is outputing this message (use __func__ typically)
	@param text message text
	@return 0 = no errors occured
*/
int flog_print(int type,const char *subsystem,const char *text);

//! formatted message output (calls flog_printf())
/*!
	@param type use one of the FLOG_* defines
	@param subsystem which part of the program is outputing this message (use __func__ typically)
	@param text_format formatted message text
	@return 0 = no errors occured
*/
int flog_printf(int type,const char *subsystem,const char *text_format, ...);

//! tests outputting all kinds of text messages
void flog_test(void);

const char * get_timestamp(void);

const char * get_msg_type(int type);
