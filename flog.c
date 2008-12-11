/*!
	@file flog.c
	@brief Flog logging library
	@author Nabeel Sowan (nabeel.sowan@vibes.se)
	
	Useful as the main logger of a program
*/

#include "flog.h"
#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

//#include <time.h>

//! initialise a FLOG_MSG_T to defaults
void init_flog_msg_t(FLOG_MSG_T *p)
{
#ifdef FLOG_TIMESTAMP
	p->time=0;
#endif
	p->type=FLOG_NONE;
	p->subsystem=NULL;
	p->text=NULL;
}

//! create and return a FLOG_MSG_T type
/*!
	@return NULL = error
*/
FLOG_MSG_T * create_flog_msg_t(FLOG_MSG_TYPE_T type,const char *subsystem,const char *text)
{
	if(text==NULL)
		return(NULL);
	FLOG_MSG_T *p;
	if((p=malloc(sizeof(FLOG_MSG_T)))!=NULL) {
		init_flog_msg_t(p);
#ifdef FLOG_TIMESTAMP
		p->time=0; //TODO
#endif
		p->type=type;
		if((subsystem != NULL) && (strlen(subsystem)>0)) {
			if(asprintf(&p->subsystem,subsystem)==-1) {
				destroy_flog_msg_t(p);
				return(NULL);
			}
		}
		if(asprintf(&p->text,text)==-1) {
			destroy_flog_msg_t(p);
			return(NULL);
		}
	}
	return(p);
}

//! free a FLOG_MSG_T
void destroy_flog_msg_t(FLOG_MSG_T *p)
{
	if(p!=NULL) {
		free(p->subsystem);
		free(p->text);
		free(p);
	}
}

//! initialise a FLOG_T to defaults
void init_flog_t(FLOG_T *p)
{
	p->name=NULL;
	p->accepted_msg_type=FLOG_SHOW_ALL;
	p->output_func=NULL;
	p->output_error=0;
	p->output_stop_on_error=1;
	p->error_log=NULL;
	p->msg=NULL;
	p->msg_amount=0;
	p->msg_max=0;
	p->sublog=NULL;
	p->sublog_amount=0;
}

//! create and return a FLOG_T type
/*!
	@return NULL = error
*/
FLOG_T * create_flog_t(const char *name, FLOG_MSG_TYPE_T accepted_msg_type)
{
	FLOG_T *p;
	if((p=malloc(sizeof(FLOG_T)))!=NULL) {
		init_flog_t(p);
		p->accepted_msg_type=accepted_msg_type;
		if((name != NULL) && (strlen(name)>0)) {
			if(asprintf(&p->name,name)==-1) {
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
	if(p!=NULL) {
		free(p->name);
		if(p->msg!=NULL) {
			int i;
			for(i=0;i<p->msg_amount;i++)
				destroy_flog_msg_t(p->msg[i]);
			free(p->msg);
		}
		free(p->sublog); //!< Note that the sublogs themselves are not freed 
		free(p);
	}
}

//! add a FLOG_MSG_T to FLOG_T and do all required logic (used by flog_print[f] functions)
/*!
	@param p target log
	@param msg message to add
	@return 0 = success
*/
int flog_add_msg(FLOG_T *p,FLOG_MSG_T *msg)
{
	int e=0;
	
	//create temporary msg to allow reformatting the message
	FLOG_MSG_T *tmpmsg;
	if((tmpmsg=create_flog_msg_t(msg->type,msg->subsystem,msg->text))==NULL)
		return(1);
	
	//append name to subsystem
	if((p->name != NULL) && (strlen(p->name)==0))
		free(p->name);
	if((tmpmsg->subsystem != NULL) && (strlen(tmpmsg->subsystem)==0))
		free(tmpmsg->subsystem);
	if((p->name != NULL) && (tmpmsg->subsystem != NULL)) {
		char *tmpstr;
		if(asprintf(&tmpstr,"%s/%s",p->name,tmpmsg->subsystem)!=-1) {
			free(tmpmsg->subsystem);
			tmpmsg->subsystem=tmpstr;
		}	
	}
	else if((p->name != NULL) && (tmpmsg->subsystem == NULL))
		asprintf(&tmpmsg->subsystem,p->name);
	
	if(tmpmsg->type & p->accepted_msg_type) {
		//add message to buffer	
		if(p->msg_amount<p->msg_max) {
			FLOG_MSG_T **new_msg;
			if((new_msg=realloc(p->msg,(p->msg_amount+1)*sizeof(FLOG_MSG_T *)))!=NULL) {
				p->msg=new_msg;
				p->msg[p->msg_amount]=msg;
				p->msg_amount++;
			}
		}
		
		//run output function
		if(p->output_func != NULL) {
			if(p->output_stop_on_error ? !p->output_error : 1) {
				if((e=p->output_func(tmpmsg,NULL)))
					p->output_error=e;
			}
		}
	}
	
	//add message to sublogs
	int i;
	for(i=0;i<p->sublog_amount;i++) {
		if(flog_add_msg(p->sublog[i],tmpmsg))
			e=1;
	}
	destroy_flog_msg_t(tmpmsg);
	return(e);
}

//! clear all messages stored in log
void flog_clear_msg_buffer(FLOG_T *p)
{
	if((p!=NULL) && (p->msg!=NULL)) {
		int i;
		for(i=0;i<p->msg_amount;i++)
			destroy_flog_msg_t(p->msg[i]);
		free(p->msg);
		p->msg_amount=0;
	}
}

//! add a sublog to a log
/*!
	@param p target log
	@param sublog log to add
	@return 0 = success
*/
int flog_append_sublog(FLOG_T *p,FLOG_T *sublog)
{
	if(p==NULL)
		return(1);
	if(p==sublog) {
		flog_print(p->error_log,FLOG_ERROR,"flog_append_sublog","cannot append log to itself (causes circular dependency)");
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

//! output an flog message
/*!
	@param type use one of the FLOG_* defines
	@param subsystem which part of the program is outputing this message (use __func__ typically)
	@param text message text
	@return 0 = success
*/
int flog_print(FLOG_T *p,FLOG_MSG_TYPE_T type,const char *subsystem,const char *text)
{
	if(p==NULL)
		return(1);
	FLOG_MSG_T *msg;
	if((msg=create_flog_msg_t(type,subsystem,text))==NULL)
		return(1);
	if(flog_add_msg(p,msg)) {
		destroy_flog_msg_t(msg);
		return(1);
	}
	destroy_flog_msg_t(msg);
	return(0);
}

//! output a formatted flog message (calls flog_print())
/*!
	@param type use one of the FLOG_* defines
	@param subsystem which part of the program is outputing this message (use __func__ typically)
	@param textf formatted message text
	@return 0 = success
*/
int flog_printf(FLOG_T *p,FLOG_MSG_TYPE_T type,const char *subsystem,const char *textf, ...)
{
	if(p==NULL)
		return(1);
	char *text;
	va_list ap;
	va_start(ap,textf);
	if(vasprintf(&text,textf,ap)==-1)
		return(1);
	va_end(ap);
	
	if(flog_print(p,type,subsystem,text)) {
		free(text);
		return(1);
	}
	free(text);
	return(0);
}

//! create and return a string from FLOG_MSG_T type
/*!
	@return NULL = error
*/
char * flog_msg_t_to_str(const FLOG_MSG_T *p)
{
	char *str,*typestr;
	typestr=flog_get_msg_type_str(p->type);
#ifdef FLOG_TIMESTAMP
	//! @todo add timestamp support
#else
	if((p->subsystem != NULL) && (typestr != NULL))
		asprintf(&str,"[%s] %s%s\n",p->subsystem,typestr,p->text);
	else if((p->subsystem != NULL) && (typestr == NULL))
		asprintf(&str,"[%s] %s\n",p->subsystem,p->text);
	else if((p->subsystem == NULL) && (typestr != NULL))
		asprintf(&str,"%s%s\n",typestr,p->text);
	else
		asprintf(&str,"%s\n",p->text);
#endif
	free(typestr);
	return(str);
}

//! Return a string or NULL according to message type
char * flog_get_msg_type_str(FLOG_MSG_TYPE_T type)
{
	int e;
	char *str=NULL;
	switch(type)
	{
		case FLOG_CRITICAL:
			e=asprintf(&str,"Critical: ");
			break;
		case FLOG_ERROR:
			e=asprintf(&str,"Error: ");
			break;
		case FLOG_WARNING:
			e=asprintf(&str,"Warning: ");
			break;
		case FLOG_NOTIFY:
			e=asprintf(&str,"!: ");
			break;
		case FLOG_INFO:
			break;
		case FLOG_VERBOSE:
			break;
		case FLOG_DEBUG:
			e=asprintf(&str,"Debug: ");
			break;
		case FLOG_FLOG_DEBUG:
			e=asprintf(&str,"Flog debug: ");
			break;
		default:
			break;
	}
	if(e==-1)
		str=NULL;
	return(str);
}

void flog_test(FLOG_T *p)
{
	flog_printf(p,FLOG_NONE,__func__,"This is a test message with FLOG_NONE (0x%02x) as type - This message should NEVER be visible",FLOG_NONE);
	flog_printf(p,FLOG_CRIT,__func__,"This is a test message with FLOG_CRIT (0x%02x) as type",FLOG_CRIT);
	flog_printf(p,FLOG_ERR,__func__,"This is a test message with FLOG_ERR (0x%02x) as type",FLOG_ERR);
	flog_printf(p,FLOG_WARN,__func__,"This is a test message with FLOG_WARN (0x%02x) as type",FLOG_WARN);
	flog_printf(p,FLOG_NOTE,__func__,"This is a test message with FLOG_NOTE (0x%02x) as type",FLOG_NOTE);
	flog_printf(p,FLOG_INFO,__func__,"This is a test message with FLOG_INFO (0x%02x) as type",FLOG_INFO);
	flog_printf(p,FLOG_VINFO,__func__,"This is a test message with FLOG_VINFO (0x%02x) as type",FLOG_VINFO);
	flog_printf(p,FLOG_DEBUG,__func__,"This is a test message with FLOG_DEBUG (0x%02x) as type",FLOG_DEBUG);
	flog_printf(p,FLOG_FLOG_DEBUG,__func__,"This is a test message with FLOG_FLOG_DEBUG (0x%02x) as type - This message should only be visible when debugging the flog library itself",FLOG_FLOG_DEBUG);
	flog_dprintf(p,FLOG_CRITICAL,__func__,"This is a test message using flog_dprintf() macro with FLOG_CRITICAL (0x%02x) as type",FLOG_CRITICAL);
}

#ifdef FLOG_TIMESTAMP
//! Return a string with current timestamp in ISO-format
/*!
	@todo this function is probably a memory leak by design :(
*/
const char * get_timestamp(void)
{
	static char str[30];
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	sprintf(str,"%04d-%02d-%02d %02d:%02d:%02d",timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
	return(str);
}
#endif
