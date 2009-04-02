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


//! initialise a FLOG_MSG_T to defaults

//! internal use only, or when extending flog
void init_flog_msg_t(FLOG_MSG_T *p)
{
	memset(p,0,sizeof(FLOG_MSG_T));
#ifdef FLOG_TIMESTAMP
	//p->time=0;
#endif
#ifdef FLOG_SRC_INFO
	//p->src_file=NULL;
	p->src_line=-1;
	//p->src_func=NULL;
#endif
	//p->subsystem=NULL;
	//p->type=FLOG_NONE;
	//p->msg_id=0;
	//p->text=NULL;
}


//! create and return a FLOG_MSG_T type

//! internal use only, or when creating flog output function
//! @retval NULL error
FLOG_MSG_T * create_flog_msg_t(const char *subsystem,
#ifdef FLOG_TIMESTAMP
                               FLOG_TIMESTAMP_T timestamp,
#endif
#ifdef FLOG_SRC_INFO
                               const char *src_file,int src_line,const char *src_func,
#endif
                               FLOG_MSG_TYPE_T type,FLOG_MSG_ID_T msg_id,const char *text)
{
	FLOG_MSG_T *p;
	if((p=malloc(sizeof(FLOG_MSG_T)))!=NULL) {
		init_flog_msg_t(p);
		p->type=type;
		if(subsystem && subsystem[0]) {
			if((p->subsystem=strdup(subsystem))==NULL) {
				destroy_flog_msg_t(p);
				return(NULL);
			}
		}
#ifdef FLOG_TIMESTAMP
		p->timestamp=timestamp;
#endif
#ifdef FLOG_SRC_INFO
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


//! free a FLOG_MSG_T

//! internal use only, or when creating flog output function
void destroy_flog_msg_t(FLOG_MSG_T *p)
{
	if(p) {
		free(p->subsystem);
#ifdef FLOG_SRC_INFO
		free(p->src_file);
		free(p->src_func);
#endif
		free(p->text);
		free(p);
		p=NULL;
	}
}


//! initialise a FLOG_T to defaults

//! mainly internal use, or when extending flog
void init_flog_t(FLOG_T *p)
{
	memset(p,0,sizeof(FLOG_T));
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


//! create and return a FLOG_T type

//! @retval NULL error
FLOG_T * create_flog_t(const char *name, FLOG_MSG_TYPE_T accepted_msg_type)
{
	FLOG_T *p;
	if((p=malloc(sizeof(FLOG_T)))!=NULL) {
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


//! free a FLOG_T
void destroy_flog_t(FLOG_T *p)
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


//! add a FLOG_MSG_T to FLOG_T and do all required logic (used by flog_print[f] functions)

//! internal use only, or when extending flog
//! @param[in,out] *p target log
//! @param[in] *msg message to add
//! @retval 0 success
int flog_add_msg(FLOG_T *p,FLOG_MSG_T *msg)
{
	//! @todo since this function is recursive, a max stack usage limit would be in order

	//compare if accepted message type
	if(!(msg->type & p->accepted_msg_type))
		return(0);

	//copy the input msg into a FLOG_MSG_T struct
	FLOG_MSG_T outmsg;
	outmsg=*msg;

	//append name to subsystem
	int free_subsystem=0;
	if(p->name) {
		if(outmsg.subsystem) {
			char *tmpstr;
			if(asprintf(&tmpstr,"%s/%s",p->name,outmsg.subsystem)!=-1) { //We don't care if we can't allocate memory
				outmsg.subsystem = tmpstr;
				free_subsystem=1;
			}
		} else {
			outmsg.subsystem=p->name;
		}
	}

	//! @todo add message to buffer
	/*
	if(p->msg_amount<p->msg_max) {
		FLOG_MSG_T **new_msg;
		if((new_msg=realloc(p->msg,(p->msg_amount+1)*sizeof(FLOG_MSG_T *)))!=NULL) {
			p->msg=new_msg;
			p->msg[p->msg_amount]=msg;
			p->msg_amount++;
		}
	}*/

	int e;
	//! @todo invent a suitable error output strategy
	//run output function
	if(p->output_func) {
		if(p->output_stop_on_error ? !p->output_error : 1) {
			if((e=p->output_func(p,&outmsg)))
				p->output_error=e;
		}
	}

	//add message to sublogs
	uint_fast8_t i;
	for(i=0;i<p->sublog_amount;i++)
		e+=flog_add_msg(p->sublog[i],&outmsg);

	//if we allocated a string, free it
	if(free_subsystem)
		free(outmsg.subsystem);

	return(e);
}


//! clear all messages stored in log
void flog_clear_msg_buffer(FLOG_T *p)
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
int flog_append_sublog(FLOG_T *p,FLOG_T *sublog)
{
	if(!p)
		return(1);
	if(p==sublog) {
		flog_print(p->error_log,NULL,FLOG_ERROR,0,"cannot append log to itself (causes circular dependency)");
		return(1);
	}
	FLOG_T **new_sublog;
	if((new_sublog=realloc(p->sublog,(p->sublog_amount+1)*sizeof(FLOG_T *)))==NULL)
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
int flog_is_message_used(FLOG_T *p,FLOG_MSG_TYPE_T type)
{
	if(type & p->accepted_msg_type) {
		if(p->msg_amount<p->msg_max)
			return(1);
		if(p->output_func) {
			if(p->output_stop_on_error ? !p->output_error : 1)
				return(1);
		}
		uint_fast8_t i;
		for(i=0;i<p->sublog_amount;i++) {
			if(flog_is_message_used(p->sublog[i],type))
				return(1);
		}
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
//! @see flog_print()
int _flog_print(FLOG_T *p,const char *subsystem,
#ifdef FLOG_SRC_INFO
                const char *src_file,int src_line,const char *src_func,
#endif
                FLOG_MSG_TYPE_T type,FLOG_MSG_ID_T msg_id,const char *text)
{
	if(!p)
		return(1);
	//Only add message if it will be used
	if(!flog_is_message_used(p,type))
		return(0);

	//Convert the input into a FLOG_MSG_T struct
	FLOG_MSG_T msg;
	init_flog_msg_t(&msg);
	msg.msg_id = msg_id;
#ifdef FLOG_CONFIG_EMPTY_STRING_TO_NULL
	if(text && text[0])
#endif //FLOG_CONFIG_EMPTY_STRING_TO_NULL
		msg.text = text;
#ifndef FLOG_CONFIG_ALLOW_NULL_MESSAGES
	if(!msg.msg_id && !msg.text)
		return(1);
#endif //FLOG_CONFIG_ALLOW_NULL_MESSAGES
#ifdef FLOG_CONFIG_EMPTY_STRING_TO_NULL
	if(subsystem && subsystem[0])
#endif //FLOG_CONFIG_EMPTY_STRING_TO_NULL
		msg.subsystem = subsystem;
#ifdef FLOG_TIMESTAMP
#ifdef FLOG_TIMESTAMP_USEC
//! @todo implement FLOG_TIMESTAMP_USEC support
#else //FLOG_TIMESTAMP_USEC
	msg.timestamp = time(NULL);
#endif //FLOG_TIMESTAMP_USEC
#endif //FLOG_TIMESTAMP
#ifdef FLOG_SRC_INFO
#ifdef FLOG_CONFIG_EMPTY_STRING_TO_NULL
	if(src_file && src_file[0])
#endif //FLOG_CONFIG_EMPTY_STRING_TO_NULL
		msg.src_file = src_file;
	msg.src_line = src_line;
#ifdef FLOG_CONFIG_EMPTY_STRING_TO_NULL
	if(src_func && src_func[0])
#endif //FLOG_CONFIG_EMPTY_STRING_TO_NULL
		msg.src_func = src_func;
#endif //FLOG_SRC_INFO
	msg.type = type;
	msg.msg_id = msg_id;
#ifdef FLOG_CONFIG_EMPTY_STRING_TO_NULL
	if(text && text[0])
#endif //FLOG_CONFIG_EMPTY_STRING_TO_NULL
		msg.text = text;

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
//! @see flog_printf()
int _flog_printf(FLOG_T *p,const char *subsystem,
#ifdef FLOG_SRC_INFO
                 const char *src_file,int src_line,const char *src_func,
#endif
                 FLOG_MSG_TYPE_T type,FLOG_MSG_ID_T msg_id,const char *textf, ...)
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

	//Convert the input into a FLOG_MSG_T struct
	FLOG_MSG_T msg;
	init_flog_msg_t(&msg);
	msg.msg_id = msg_id;
#ifdef FLOG_CONFIG_EMPTY_STRING_TO_NULL
	if(text && text[0])
#endif //FLOG_CONFIG_EMPTY_STRING_TO_NULL
		msg.text = text;
#ifndef FLOG_CONFIG_ALLOW_NULL_MESSAGES
	if(!msg.msg_id && !msg.text)
		return(1);
#endif //FLOG_CONFIG_ALLOW_NULL_MESSAGES
#ifdef FLOG_CONFIG_EMPTY_STRING_TO_NULL
	if(subsystem && subsystem[0])
#endif //FLOG_CONFIG_EMPTY_STRING_TO_NULL
		msg.subsystem = subsystem;
#ifdef FLOG_TIMESTAMP
#ifdef FLOG_TIMESTAMP_USEC
//! @todo implement FLOG_TIMESTAMP_USEC support
#else //FLOG_TIMESTAMP_USEC
	msg.timestamp = time(NULL);
#endif //FLOG_TIMESTAMP_USEC
#endif //FLOG_TIMESTAMP
#ifdef FLOG_SRC_INFO
#ifdef FLOG_CONFIG_EMPTY_STRING_TO_NULL
	if(src_file && src_file[0])
#endif //FLOG_CONFIG_EMPTY_STRING_TO_NULL
		msg.src_file = src_file;
	msg.src_line = src_line;
#ifdef FLOG_CONFIG_EMPTY_STRING_TO_NULL
	if(src_func && src_func[0])
#endif //FLOG_CONFIG_EMPTY_STRING_TO_NULL
		msg.src_func = src_func;
#endif //FLOG_SRC_INFO
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
void flog_test(FLOG_T *p)
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
