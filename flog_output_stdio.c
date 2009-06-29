//! stdio output for Flog

//! @file flog_output_stdio.c
//! @author Nabeel Sowan (nabeel.sowan@vibes.se)
//!
//! When you want flog to write to stdout or stderr


#include "flog_output_stdio.h"

#ifdef FLOG_CONFIG_OUTPUT_STDIO

#include "flog_string.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


//! Output function for log output to stdout

//! @retval 0 success
int flog_output_stdout(FLOG_T *log,const FLOG_MSG_T *msg)
{
	char *str;
	if(flog_get_str_message(&str,msg))
		return(-1);
	if(fputs(str,stdout)==EOF) {
		log->output_error=errno;
		free(str);
		flog_print(log->error_log,NULL,FLOG_ERROR,FLOG_MSG_CANNOT_WRITE_TO_STDOUT,strerror(log->output_error));
		return(log->output_error);
	}
	free(str);
	return(0);
}


//! Output function for log output to stderr

//! @retval 0 success
int flog_output_stderr(FLOG_T *log,const FLOG_MSG_T *msg)
{
	char *str;
	if(flog_get_str_message(&str,msg))
		return(-1);
	if(fputs(str,stderr)==EOF) {
		log->output_error=errno;
		free(str);
		flog_print(log->error_log,NULL,FLOG_ERROR,FLOG_MSG_CANNOT_WRITE_TO_STDERR,strerror(log->output_error));
		return(log->output_error);
	}
	free(str);
	return(0);
}


//! create and return a log that writes to stdout

//! @retval NULL error
FLOG_T * create_flog_output_stdout(const char *name, FLOG_MSG_TYPE_T accepted_msg_type)
{
	FLOG_T *p;
	if((p=create_flog_t(name,accepted_msg_type))==NULL)
		return(NULL);
	p->output_func=flog_output_stdout;
	return(p);
}


//! create and return a log that writes to stderr

//! @retval NULL error
FLOG_T * create_flog_output_stderr(const char *name, FLOG_MSG_TYPE_T accepted_msg_type)
{
	FLOG_T *p;
	if((p=create_flog_t(name,accepted_msg_type))==NULL)
		return(NULL);
	p->output_func=flog_output_stderr;
	return(p);
}


#endif //FLOG_CONFIG_OUTPUT_STDIO
