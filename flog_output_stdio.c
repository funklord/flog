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
int flog_output_stdout(flog_t *log,const flog_msg_t *msg)
{
	char str[FLOG_CONFIG_STR_MAX];
	if(flog_get_str_message(str,sizeof(str),msg)<0)
		return(-1);
	if(fputs(str,stdout)==EOF) {
		log->output_error=errno;
		flog_print(log->error_log,NULL,FLOG_ERROR,FLOG_MSG_CANNOT_WRITE_TO_STDOUT,strerror(log->output_error));
		return(log->output_error);
	}
	return(0);
}


//! Output function for log output to stderr

//! @retval 0 success
int flog_output_stderr(flog_t *log,const flog_msg_t *msg)
{
	char str[FLOG_CONFIG_STR_MAX];
	if(flog_get_str_message(str,sizeof(str),msg)<0)
		return(-1);
	if(fputs(str,stderr)==EOF) {
		log->output_error=errno;
		flog_print(log->error_log,NULL,FLOG_ERROR,FLOG_MSG_CANNOT_WRITE_TO_STDERR,strerror(log->output_error));
		return(log->output_error);
	}
	return(0);
}


//! create and return a log that writes to stdout

//! @retval NULL error
//! @brief Point a caller-owned flog_t at stdout
//!
//! The caller-owned counterpart of create_flog_output_stdout(): the flog_t
//! lives wherever the caller put it and the name is BORROWED rather than
//! copied, so it must outlive the log. There is nothing to destroy.
void init_flog_output_stdout(flog_t *p, const char *name, flog_msg_type_t accepted_msg_type)
{
	init_flog_t(p);
	p->name=(char *)name;
	p->accepted_msg_type=accepted_msg_type;
	p->output_func=flog_output_stdout;
}

//! @brief Point a caller-owned flog_t at stderr
//! @see init_flog_output_stdout()
void init_flog_output_stderr(flog_t *p, const char *name, flog_msg_type_t accepted_msg_type)
{
	init_flog_t(p);
	p->name=(char *)name;
	p->accepted_msg_type=accepted_msg_type;
	p->output_func=flog_output_stderr;
}

flog_t * create_flog_output_stdout(const char *name, flog_msg_type_t accepted_msg_type)
{
	flog_t *p;
	if((p=create_flog_t(name,accepted_msg_type))==NULL)
		return(NULL);
	p->output_func=flog_output_stdout;
	return(p);
}


//! create and return a log that writes to stderr

//! @retval NULL error
flog_t * create_flog_output_stderr(const char *name, flog_msg_type_t accepted_msg_type)
{
	flog_t *p;
	if((p=create_flog_t(name,accepted_msg_type))==NULL)
		return(NULL);
	p->output_func=flog_output_stderr;
	return(p);
}


#endif //FLOG_CONFIG_OUTPUT_STDIO

