//! Flog - The F logging library

//! @file flog.c
//! @author Nabeel Sowan (nabeel.sowan@vibes.se)
//!
//! Useful as the main logger of a program or embedded system.
//! Requires C99 + GNU support.

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "flog.h"


#ifndef FLOG_HAVE_ASPRINTF

//! @brief vasprintf() for platforms without one
//!
//! Every use of asprintf() in flog is the same shape: format a short string
//! and allocate exactly enough to hold it. C99 says vsnprintf() returns the
//! length it WOULD have written when given a null buffer and a size of zero,
//! which is that operation with no extension involved -- so the fallback
//! needs nothing the language does not already promise.
//!
//! The va_list is copied before the measuring pass, because a va_list may not
//! be reused after being passed to a vprintf-family call. Measuring with the
//! caller's own list and then formatting with it again is undefined, and it
//! happens to work on x86-64 while failing on other calling conventions,
//! which is the worst way for it to be wrong.
//!
//! @param[out] **strp receives the allocated string on success
//! @param[in] *fmt printf format string
//! @param[in] ap arguments for fmt
//! @retval -1 failure, *strp untouched
int vasprintf(char **strp, const char *fmt, va_list ap)
{
	va_list measure;
	va_copy(measure,ap);
	int len=vsnprintf(NULL,0,fmt,measure);
	va_end(measure);
	if(len<0)
		return(-1);
	char *buf=malloc((size_t)len+1);
	if(buf==NULL)
		return(-1);
	int written=vsnprintf(buf,(size_t)len+1,fmt,ap);
	if(written<0) {
		free(buf);
		return(-1);
	}
	*strp=buf;
	return(written);
}

//! @brief asprintf() for platforms without one
//! @param[out] **strp receives the allocated string on success
//! @param[in] *fmt printf format string
//! @retval -1 failure, *strp untouched
int asprintf(char **strp, const char *fmt, ...)
{
	va_list ap;
	va_start(ap,fmt);
	int rc=vasprintf(strp,fmt,ap);
	va_end(ap);
	return(rc);
}

#endif //FLOG_HAVE_ASPRINTF

#ifndef FLOG_HAVE_STRDUP

//! @brief strdup() for platforms without one
//! @param[in] *s string to copy
//! @retval NULL allocation failed
char *strdup(const char *s)
{
	size_t len=strlen(s)+1;
	char *p=malloc(len);
	if(p==NULL)
		return(NULL);
	memcpy(p,s,len);
	return(p);
}

#endif //FLOG_HAVE_STRDUP


//! initialise a flog_msg_t to defaults

//! internal use only, or when extending flog
void init_flog_msg_t(flog_msg_t *p)
{
	memset(p,0,sizeof(flog_msg_t));
#ifdef FLOG_CONFIG_TIMESTAMP
	//p->time=0;
#endif
#ifdef FLOG_CONFIG_SRC_INFO
	//p->src_file=NULL;
	//p->src_line=0;
	//p->src_func=NULL;
#endif
	//p->subsystem=NULL;
	//p->type=FLOG_NONE;
	//p->msg_id=0;
	//p->text=NULL;
}


//! create and return a flog_msg_t type

//! internal use only, or when creating flog output function
//! @retval NULL error
flog_msg_t * create_flog_msg_t(const char *subsystem,
#ifdef FLOG_CONFIG_TIMESTAMP
                               flog_timestamp_t timestamp,
#endif
#ifdef FLOG_CONFIG_SRC_INFO
                               const char *src_file,uint_fast16_t src_line,const char *src_func,
#endif
                               flog_msg_type_t type,flog_msg_id_t msg_id,const char *text)
{
	flog_msg_t *p;
	if((p=malloc(sizeof(flog_msg_t)))!=NULL) {
		init_flog_msg_t(p);
		p->type=type;
		if(subsystem && subsystem[0]) {
			if((p->subsystem=strdup(subsystem))==NULL) {
				destroy_flog_msg_t(p);
				return(NULL);
			}
		}
#ifdef FLOG_CONFIG_TIMESTAMP
		p->timestamp=timestamp;
#endif
#ifdef FLOG_CONFIG_SRC_INFO
		if(src_file && src_file[0]) {
			if((p->src_file=strdup(src_file))==NULL) {
				destroy_flog_msg_t(p);
				return(NULL);
			}
		}
		p->src_line=src_line;
		if(src_func && src_func[0]) {
			if((p->src_func=strdup(src_func))==NULL) {
				destroy_flog_msg_t(p);
				return(NULL);
			}
		}
#endif
		p->msg_id=msg_id;
		if(text!=NULL) {
			if((p->text=strdup(text))==NULL) {
				destroy_flog_msg_t(p);
				return(NULL);
			}
		}
	}
	return(p);
}


//! free a flog_msg_t

//! internal use only, or when creating flog output function
void destroy_flog_msg_t(flog_msg_t *p)
{
	if(p) {
		//This is the one site allowed to free these fields -- see flog_msg_t's
		//doc comment in flog.h. The cast is deliberate, not a workaround.
		free((void *)p->subsystem);
#ifdef FLOG_CONFIG_SRC_INFO
		free((void *)p->src_file);
		free((void *)p->src_func);
#endif
		free((void *)p->text);
		free(p);
		p=NULL;
	}
}


//! initialise a flog_t to defaults

//! mainly internal use, or when extending flog
void init_flog_t(flog_t *p)
{
	memset(p,0,sizeof(flog_t));
	//p->name=NULL;
	p->accepted_msg_type=FLOG_ACCEPT_ALL;
	//p->output_func=NULL;
	//p->output_func_data=NULL;
	//p->output_error=0;
	p->output_stop_on_error=1;
	//p->error_log=NULL;
	//p->msg=NULL;
	//p->msg_amount=0;
	//p->msg_max=0;
	//p->sublog=NULL;
	//p->sublog_amount=0;
}


//! create and return a flog_t type

//! @retval NULL error
flog_t * create_flog_t(const char *name, flog_msg_type_t accepted_msg_type)
{
	flog_t *p;
	if((p=malloc(sizeof(flog_t)))!=NULL) {
		init_flog_t(p);
		p->accepted_msg_type=accepted_msg_type;
		if(name && name[0]) {
			if((p->name=strdup(name))==NULL) {
				destroy_flog_t(p);
				return(NULL);
			}
		}
	}
	return(p);
}


//! free a flog_t
void destroy_flog_t(flog_t *p)
{
	if(p) {
		free(p->name);
		if(p->msg) {
			uint_fast16_t i;
			for(i=0;i<p->msg_amount;i++)
				destroy_flog_msg_t(p->msg[i]);
			free(p->msg);
		}
		free(p->sublog); //! Note that sublogs are not freed
		free(p);
		p=NULL;
	}
}


#ifdef FLOG_CONFIG_RECURSIVE_MAX_STACK_DEPTH
int stack_depth;
#endif


//! add a flog_msg_t to flog_t and do all required logic (used by flog_print[f] functions)

//! internal use only, or when extending flog
//! @param[in,out] *p target log
//! @param[in] *msg message to add
//! @retval 0 success
int flog_add_msg(flog_t *p,flog_msg_t *msg)
{
	//compare if accepted message type
	if(!(msg->type & p->accepted_msg_type))
		return(0);

	//copy the input msg into a flog_msg_t struct
	flog_msg_t outmsg;
	outmsg=*msg;

	//append name to subsystem
	char *appended_subsystem=NULL;
	if(p->name) {
		if(outmsg.subsystem) {
			if(asprintf(&appended_subsystem,"%s/%s",p->name,outmsg.subsystem)!=-1) { //We don't care if we can't allocate memory
				outmsg.subsystem = appended_subsystem;
			}
		} else {
			outmsg.subsystem=p->name;
		}
	}

	//! @todo add message to buffer
	/*
	if(p->msg_amount<p->msg_max) {
		flog_msg_t **new_msg;
		if((new_msg=realloc(p->msg,(p->msg_amount+1)*sizeof(flog_msg_t *)))!=NULL) {
			p->msg=new_msg;
			p->msg[p->msg_amount]=msg;
			p->msg_amount++;
		}
	}*/

	//! @todo invent a suitable error output strategy
	int e=0;

	//run output function
	if(p->output_func) {
		if(p->output_stop_on_error ? !p->output_error : 1) {
			if((e=p->output_func(p,&outmsg)))
				p->output_error=e;
		}
	}

#ifdef FLOG_CONFIG_RECURSIVE_MAX_STACK_DEPTH
	if(stack_depth+1 < FLOG_CONFIG_RECURSIVE_MAX_STACK_DEPTH) {
		stack_depth++;
#endif
		//add message to sublogs
		uint_fast8_t i;
		for(i=0;i<p->sublog_amount;i++)
			e+=flog_add_msg(p->sublog[i],&outmsg);
#ifdef FLOG_CONFIG_RECURSIVE_MAX_STACK_DEPTH
		stack_depth--;
	}
#endif

	//if we allocated a string, free it
	free(appended_subsystem);

	return(e);
}


//! clear all messages stored in log
void flog_clear_msg_buffer(flog_t *p)
{
	if(p && p->msg) {
		uint_fast16_t i;
		for(i=0;i<p->msg_amount;i++)
			destroy_flog_msg_t(p->msg[i]);
		free(p->msg);
		p->msg=NULL;
		p->msg_amount=0;
	}
}


//! add a sublog to a log

//! @param[in,out] *p target log
//! @param[in] *sublog log to add
//! @retval 0 success
int flog_append_sublog(flog_t *p,flog_t *sublog)
{
	if(!p)
		return(1);
	if(p==sublog) {
		flog_print(p->error_log,NULL,FLOG_ERROR,0,"cannot append log to itself (causes circular dependency)");
		return(1);
	}
	flog_t **new_sublog;
	if((new_sublog=realloc(p->sublog,(p->sublog_amount+1)*sizeof(flog_t *)))==NULL)
		return(1);
	p->sublog=new_sublog;
	p->sublog[p->sublog_amount]=sublog;
	p->sublog_amount++;
	return(0);
}


//! Is the message used in any way if put in this log?

//! This function can be used to decide whether or not to drop a message immediately
//! @param[in] *p the log receiving the message
//! @param[in] type the type from message
//! @retval 0 Message is never used
//! @retval 1 Message is used
int flog_is_message_used(flog_t *p,flog_msg_type_t type)
{
	if(type & p->accepted_msg_type) {
		if(p->msg_amount<p->msg_max)
			return(1);
		if(p->output_func) {
			if(p->output_stop_on_error ? !p->output_error : 1)
				return(1);
		}
#ifdef FLOG_CONFIG_RECURSIVE_MAX_STACK_DEPTH
		if(stack_depth+1 < FLOG_CONFIG_RECURSIVE_MAX_STACK_DEPTH) {
			stack_depth++;
#endif
			uint_fast8_t i;
			for(i=0;i<p->sublog_amount;i++) {
				if(flog_is_message_used(p->sublog[i],type)) {
#ifdef FLOG_CONFIG_RECURSIVE_MAX_STACK_DEPTH
					stack_depth--;
#endif
					return(1);
				}
			}
#ifdef FLOG_CONFIG_RECURSIVE_MAX_STACK_DEPTH
			stack_depth--;
		}
#endif
	}
	return(0);
}


//! do not call directly, use the flog_print() macro instead

//! emit an flog message
//! @param[in,out] *p log to emit message to
//! @param[in] *subsystem which part of the program is outputing this message
//! @param[in] *src_file source code file (flog_print() macro uses __FILE__ to fill this in)
//! @param[in] src_line source code line (flog_print() macro uses __LINE__ to fill this in)
//! @param[in] *src_func source code function (flog_print() macro uses __FUNCTION__ to fill this in)
//! @param[in] type use one of the FLOG_* defines
//! @param[in] msg_id optionally use errno or one of the FLOG_MSG_* defines
//! @param[in] *text message text
//! @retval 0 success
//! @retval 1 error while adding message to log
//! @retval 2 error unable to get time
//! @retval 3 did not add null message (flog is configured not to allow null messages)
//! @see flog_print()
int _flog_print(flog_t *p,const char *subsystem,
#ifdef FLOG_CONFIG_SRC_INFO
                const char *src_file,uint_fast16_t src_line,const char *src_func,
#endif
                flog_msg_type_t type,flog_msg_id_t msg_id,const char *text)
{
	if(!p)
		return(1);
	//Only add message if it will be used
	if(!flog_is_message_used(p,type))
		return(0);

	//Convert the input into a flog_msg_t struct
	flog_msg_t msg;
	init_flog_msg_t(&msg);
	msg.msg_id = msg_id;
	if(text && text[0])
		msg.text = text;
#ifndef FLOG_CONFIG_ALLOW_NULL_MESSAGES
	if(!msg.msg_id && !msg.text)
		return(3);
#endif //FLOG_CONFIG_ALLOW_NULL_MESSAGES
	if(subsystem && subsystem[0])
		msg.subsystem = subsystem;
#ifdef FLOG_CONFIG_TIMESTAMP
#ifdef FLOG_CONFIG_TIMESTAMP_USEC
	if(gettimeofday(&msg.timestamp,NULL))
		return(2);
#else //FLOG_CONFIG_TIMESTAMP_USEC
	msg.timestamp = time(NULL);
#endif //FLOG_CONFIG_TIMESTAMP_USEC
#endif //FLOG_CONFIG_TIMESTAMP
#ifdef FLOG_CONFIG_SRC_INFO
	if(src_file && src_file[0])
		msg.src_file = src_file;
	msg.src_line = src_line;
	if(src_func && src_func[0])
		msg.src_func = src_func;
#endif //FLOG_CONFIG_SRC_INFO
	msg.type = type;

	//Add message to log
	if(flog_add_msg(p,&msg)) {
		return(1);
	}
	return(0);
}


//! do not call directly, use the flog_printf() macro instead

//! emit a formatted flog message (calls _flog_print())
//! @param[in,out] *p log to emit message to
//! @param[in] *subsystem which part of the program is outputing this message
//! @param[in] *src_file source code file (flog_printf() macro uses __FILE__ to fill this in)
//! @param[in] src_line source code line (flog_printf() macro uses __LINE__ to fill this in)
//! @param[in] *src_func source code function (flog_printf() macro uses __FUNCTION__ to fill this in)
//! @param[in] type use one of the FLOG_* defines
//! @param[in] msg_id optionally use errno or one of the FLOG_MSG_* defines
//! @param[in] *textf formatted message text
//! @retval 0 success
//! @retval 1 error while adding message to log
//! @retval 2 error unable to get time
//! @retval 3 did not add null message (flog is configured not to allow null messages)
//! @see flog_printf()
int _flog_printf(flog_t *p,const char *subsystem,
#ifdef FLOG_CONFIG_SRC_INFO
                 const char *src_file,uint_fast16_t src_line,const char *src_func,
#endif
                 flog_msg_type_t type,flog_msg_id_t msg_id,const char *textf, ...)
{
	if(!p)
		return(1);
	//Only add message if it will be used
	if(!flog_is_message_used(p,type))
		return(0);

	//Parse format string
	char *text;
	va_list ap;
	va_start(ap,textf);
	if(vasprintf(&text,textf,ap)==-1)
		return(1);
	va_end(ap);

	//Convert the input into a flog_msg_t struct
	flog_msg_t msg;
	init_flog_msg_t(&msg);
	msg.msg_id = msg_id;
	if(text && text[0])
		msg.text = text;
#ifndef FLOG_CONFIG_ALLOW_NULL_MESSAGES
	if(!msg.msg_id && !msg.text)
		return(1);
#endif //FLOG_CONFIG_ALLOW_NULL_MESSAGES
	if(subsystem && subsystem[0])
		msg.subsystem = subsystem;
#ifdef FLOG_CONFIG_TIMESTAMP
#ifdef FLOG_CONFIG_TIMESTAMP_USEC
	if(gettimeofday(&msg.timestamp,NULL))
		return(1);
#else //FLOG_CONFIG_TIMESTAMP_USEC
	msg.timestamp = time(NULL);
#endif //FLOG_CONFIG_TIMESTAMP_USEC
#endif //FLOG_CONFIG_TIMESTAMP
#ifdef FLOG_CONFIG_SRC_INFO
	if(src_file && src_file[0])
		msg.src_file = src_file;
	msg.src_line = src_line;
	if(src_func && src_func[0])
		msg.src_func = src_func;
#endif //FLOG_CONFIG_SRC_INFO
	msg.type = type;

	//Add message to log
	if(flog_add_msg(p,&msg)) {
		free(text);
		return(1);
	}
	free(text);
	return(0);
}


#ifdef DEBUG
//! Test various flog features
void flog_test(flog_t *p)
{
	flog_printf(p,__func__,FLOG_NONE,0,"This is a test message with FLOG_NONE (0x%02x) as type - This message should NEVER be visible",FLOG_NONE);
	flog_printf(p,__func__,FLOG_CRIT,0,"This is a test message with FLOG_CRIT (0x%02x) as type",FLOG_CRIT);
	flog_printf(p,__func__,FLOG_ERR,0,"This is a test message with FLOG_ERR (0x%02x) as type",FLOG_ERR);
	flog_printf(p,__func__,FLOG_WARN,0,"This is a test message with FLOG_WARN (0x%02x) as type",FLOG_WARN);
	flog_printf(p,__func__,FLOG_NOTE,0,"This is a test message with FLOG_NOTE (0x%02x) as type",FLOG_NOTE);
	flog_printf(p,__func__,FLOG_INFO,0,"This is a test message with FLOG_INFO (0x%02x) as type",FLOG_INFO);
	flog_printf(p,__func__,FLOG_VINFO,0,"This is a test message with FLOG_VINFO (0x%02x) as type",FLOG_VINFO);
	flog_printf(p,__func__,FLOG_DEBUG,0,"This is a test message with FLOG_DEBUG (0x%02x) as type",FLOG_DEBUG);
	flog_printf(p,__func__,FLOG_DEEP_DEBUG,0,"This is a test message with FLOG_DEEP_DEBUG (0x%02x) as type - This message should only be visible when implicitly switching on deep debugging",FLOG_DEEP_DEBUG);
	flog_dprintf(p,__func__,FLOG_CRITICAL,0,"This is a test message using flog_dprintf() macro with FLOG_CRITICAL (0x%02x) as type",FLOG_CRITICAL);
}
#endif
