//! @file flog.c

#include "flog.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "libintl.h"

//! @todo static buffer
#define FLOG_FORMAT_BUFFERSIZE 1024

int write_to_logfile(const char *text/*const wchar_t *text*/);

int flog_init(const char *log_name,int am_stderr,int am_stdout,int am_file,const char *filename_log)
{
	//flog_opt.subsystem_prepend[0] = '\0';
	strcpy(flog_opt.subsystem_prepend,log_name);
	flog_opt.logbuffer_lines=0;
	if(am_stderr==-1)
		flog_opt.accepted_messages_stderr = FLOG_SHOW_INFO;
	else
		flog_opt.accepted_messages_stderr = am_stderr;
	if(am_stdout==-1)
		flog_opt.accepted_messages_stdout = FLOG_NONE;
	else
		flog_opt.accepted_messages_stdout = am_stderr;
	if(am_file==-1)
		flog_opt.accepted_messages_file = FLOG_SHOW_VERBOSE_INFO;
	else
		flog_opt.accepted_messages_file = am_file;
	strcpy(flog_opt.logfile,filename_log);
	flog_opt.buffered_write = 0;
	flog_opt.write_error = 0;
	return(flog_dprint(FLOG_FLOG_DEBUG,"flog_init",gettext("flog started")));
}

int flog_print(int type,const char *subsystem,const char *text)
{
	char outbuf[FLOG_FORMAT_BUFFERSIZE]; //! @todo static buffer (how to detect size of result from snprintf()?)
	int j,my_strlen;
	snprintf(outbuf,FLOG_FORMAT_BUFFERSIZE,"[%s %s/%s] %s%s\n",get_timestamp(),flog_opt.subsystem_prepend,subsystem,get_msg_type(type),text);
	my_strlen=strlen(outbuf);
	for(j=0;j<my_strlen-1;j++)
	{
		if(outbuf[j]<33)
			outbuf[j]=' ';
	}
	if(type & flog_opt.accepted_messages_stderr)
		fprintf(stderr,"%s",outbuf);
	if(type & flog_opt.accepted_messages_stdout)
		printf("%s",outbuf);
	if(type & flog_opt.accepted_messages_file)
	{
		if(!flog_opt.write_error)
		{
			if(write_to_logfile(outbuf))
			{
				flog_opt.write_error++;
				flog_printf(FLOG_CRITICAL,"flog_print","Cannot open logfile %s - ALL further messages to logfile will be suppressed",flog_opt.logfile);
				return(-1);
			}
		}
	}
	return(0);
}

int flog_printf(int type,const char *subsystem,const char *text_format, ...)
{
	char outbuf[FLOG_FORMAT_BUFFERSIZE]; //! @todo static buffer (how to detect size of result from vsnprintf()?)
	va_list ap;
	va_start(ap,text_format);
	vsnprintf(outbuf,FLOG_FORMAT_BUFFERSIZE,text_format,ap);
	va_end(ap);
	return(flog_print(type,subsystem,outbuf));
}

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

//! Return a string according to message type
/*!
	@todo this function is probably a memory leak by design :(
*/
const char * get_msg_type(int type)
{
	static char str[15];
	str[0]=0;
	switch(type)
	{
		case FLOG_CRITICAL:
			strcpy(str,gettext("Critical"));
			break;
		case FLOG_ERROR:
			strcpy(str,gettext("Error"));
			break;
		case FLOG_WARNING:
			strcpy(str,gettext("Warning"));
			break;
		case FLOG_NOTIFY:
			strcpy(str,"!");
			break;
		case FLOG_INFO:
			break;
		case FLOG_VERBOSE:
			break;
		case FLOG_DEBUG:
			strcpy(str,gettext("Debug"));
			break;
		case FLOG_FLOG_DEBUG:
			strcpy(str,gettext("Flog debug"));
			break;
		default:
			break;
	}
	if(strcmp(str,""))
		strcat(str,": ");
	return(str);
}

int write_to_logfile(const char *text/*const wchar_t *text*/)
{
	FILE *f;
	if((f = fopen(flog_opt.logfile,"a+t"))==NULL)
		return(-1);
	fprintf(f,"%s",text);
	fclose(f);
	return(0);
}

void flog_test(void)
{
	flog_printf(FLOG_NONE,__func__,"This is a test message with FLOG_NONE (0x%02x) as type - This message should NEVER be visible",FLOG_NONE);
	flog_printf(FLOG_CRIT,__func__,"This is a test message with FLOG_CRIT (0x%02x) as type",FLOG_CRIT);
	flog_printf(FLOG_ERR,__func__,"This is a test message with FLOG_ERR (0x%02x) as type",FLOG_ERR);
	flog_printf(FLOG_WARN,__func__,"This is a test message with FLOG_WARN (0x%02x) as type",FLOG_WARN);
	flog_printf(FLOG_NOTE,__func__,"This is a test message with FLOG_NOTE (0x%02x) as type",FLOG_NOTE);
	flog_printf(FLOG_INFO,__func__,"This is a test message with FLOG_INFO (0x%02x) as type",FLOG_INFO);
	flog_printf(FLOG_VINFO,__func__,"This is a test message with FLOG_VINFO (0x%02x) as type",FLOG_VINFO);
	flog_printf(FLOG_DEBUG,__func__,"This is a test message with FLOG_DEBUG (0x%02x) as type",FLOG_DEBUG);
	flog_printf(FLOG_FLOG_DEBUG,__func__,"This is a test message with FLOG_FLOG_DEBUG (0x%02x) as type - This message should only be visible when debugging the flog library itself",FLOG_FLOG_DEBUG);
	flog_dprintf(FLOG_FLOG_DEBUG,__func__,"This is a test message using flog_dprintf() macro with FLOG_CRITICAL (0x%02x) as type",FLOG_CRITICAL);
}
