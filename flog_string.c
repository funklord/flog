//! Output string routines for Flog

//! @file flog_string.c
//! @author Nabeel Sowan (nabeel.sowan@vibes.se)
//!
//! To convert flog messages to strings
//! internal use only, or when creating flog output function

#include "flog_string.h"

#ifdef FLOG_CONFIG_STRING_OUTPUT

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef FLOG_CONFIG_TIMESTAMP
#include <time.h>
#endif


//! @brief Append formatted text to a bounded buffer
//!
//! The whole module is built on this. Every string flog renders is a join of
//! a few optional parts, and the version that allocated expressed each join
//! as a nest of asprintf calls -- one branch per combination of parts, each
//! with its own failure path to unwind the parts already allocated. That is
//! where the 31 allocations lived, and most of the file was the unwinding
//! rather than the formatting.
//!
//! Appending into one buffer removes the combinations as well as the
//! allocations: an absent part appends nothing and needs no branch of its
//! own. Truncation is silent and bounded, which is the right failure for a
//! log line: losing the tail of one line beats losing the line.
//!
//! @param[out] *buf buffer to append into
//! @param[in] cap total size of buf, including the terminator
//! @param[in,out] *len length already in buf, updated on return
static void sb_add(char *buf, size_t cap, size_t *len, const char *fmt, ...)
{
	if(!buf || cap==0 || *len>=cap-1)
		return;
	va_list ap;
	va_start(ap,fmt);
	int n=vsnprintf(buf+*len,cap-*len,fmt,ap);
	va_end(ap);
	if(n<0)
		return;
	*len = ((size_t)n >= cap-*len) ? cap-1 : *len+(size_t)n;
}


#ifdef FLOG_CONFIG_TIMESTAMP
//! Write an ISO timestamp

//! @param[out] *buf buffer to write into
//! @param[in] cap size of buf including terminator
//! @param[in] ts timestamp
//! @retval -1 no buffer
int flog_get_str_iso_timestamp(char *buf, size_t cap, const flog_timestamp_t ts)
{
	if(!buf || cap==0)
		return(-1);
	struct tm ts_tm;
	size_t len=0;
	buf[0]='\0';
#ifdef FLOG_CONFIG_TIMESTAMP_USEC
	ts_tm = *localtime(&ts.tv_sec);
	sb_add(buf,cap,&len,"%04d-%02d-%02d %02d:%02d:%02d.%06d", ts_tm.tm_year+1900, ts_tm.tm_mon+1, ts_tm.tm_mday, ts_tm.tm_hour, ts_tm.tm_min, ts_tm.tm_sec, (int)ts.tv_usec);
#else //FLOG_CONFIG_TIMESTAMP_USEC
	ts_tm = *localtime(&ts);
	sb_add(buf,cap,&len,"%04d-%02d-%02d %02d:%02d:%02d", ts_tm.tm_year+1900, ts_tm.tm_mon+1, ts_tm.tm_mday, ts_tm.tm_hour, ts_tm.tm_min, ts_tm.tm_sec);
#endif //FLOG_CONFIG_TIMESTAMP_USEC
	return((int)len);
}
#endif //FLOG_CONFIG_TIMESTAMP


//! Write a string describing the message type, or nothing

//! @param[out] *buf buffer to write into
//! @param[in] cap size of buf including terminator
//! @param[in] type type of message
//! @retval -1 no buffer
int flog_get_str_msg_type(char *buf, size_t cap, const flog_msg_type_t type)
{
	if(!buf || cap==0)
		return(-1);
	const char *s=NULL;
	buf[0]='\0';
	switch(type)
	{
		case FLOG_CRITICAL:  s="Critical";   break;
		case FLOG_ERROR:     s="Error";      break;
		case FLOG_WARNING:   s="Warning";    break;
		case FLOG_NOTIFY:    s="!";          break;
		//! INFO and VERBOSE deliberately render nothing: the message speaks
		//! for itself and a prefix would be noise on the commonest lines.
		case FLOG_INFO:      break;
		case FLOG_VERBOSE:   break;
		case FLOG_DEBUG:     s="Debug";      break;
		case FLOG_DEEP_DEBUG:s="Deep debug"; break;
		default: break;
	}
	if(!s)
		return(0);
	size_t len=0;
	sb_add(buf,cap,&len,"%s",s);
	return((int)len);
}


#ifdef FLOG_CONFIG_MSG_ID_STRINGS
extern const char *flog_msg_id_str[];
#endif //FLOG_CONFIG_MSG_ID_STRINGS


//! Write a string for a message id, or nothing

//! @param[out] *buf buffer to write into
//! @param[in] cap size of buf including terminator
//! @param[in] msg_id message ID type
//! @retval -1 no buffer
int flog_get_str_msg_id(char *buf, size_t cap, const flog_msg_id_t msg_id)
{
	if(!buf || cap==0)
		return(-1);
	size_t len=0;
	buf[0]='\0';
	if(msg_id==0)
		return(0);
	if(msg_id>=FLOG_MSG_ID_AMOUNT_RESERVED_FOR_ERRNO) {
#ifdef FLOG_CONFIG_MSG_ID_STRINGS
		//! @todo we need to run toupper() on the first char of the message (maybe another function?)
#ifdef FLOG_CONFIG_OUTPUT_SHOW_MSG_ID
		sb_add(buf,cap,&len,"(%d) %s", msg_id, flog_msg_id_str[msg_id-FLOG_MSG_ID_AMOUNT_RESERVED_FOR_ERRNO]);
#else //FLOG_CONFIG_OUTPUT_SHOW_MSG_ID
		sb_add(buf,cap,&len,"%s", flog_msg_id_str[msg_id-FLOG_MSG_ID_AMOUNT_RESERVED_FOR_ERRNO]);
#endif //FLOG_CONFIG_OUTPUT_SHOW_MSG_ID
#else //FLOG_CONFIG_MSG_ID_STRINGS
		sb_add(buf,cap,&len,"%d", msg_id);
#endif //FLOG_CONFIG_MSG_ID_STRINGS
	} else {
		//! @todo make thread safe with strerror_r()
#ifdef FLOG_CONFIG_ERRNO_STRINGS
#ifdef FLOG_CONFIG_OUTPUT_SHOW_MSG_ID
		sb_add(buf,cap,&len,"(%d) %s", msg_id, strerror(msg_id));
#else //FLOG_CONFIG_OUTPUT_SHOW_MSG_ID
		sb_add(buf,cap,&len,"%s", strerror(msg_id));
#endif //FLOG_CONFIG_OUTPUT_SHOW_MSG_ID
#else //FLOG_CONFIG_ERRNO_STRINGS
		sb_add(buf,cap,&len,"(%d)", msg_id);
#endif //FLOG_CONFIG_ERRNO_STRINGS
	}
	return((int)len);
}


#ifdef FLOG_CONFIG_SRC_INFO
//! Write a string from flog source info, or nothing

//! file, then ":line" if there is one, then "|func()" -- with the separator
//! omitted when nothing precedes it, so a lone function renders "func()"
//! rather than "|func()".
//!
//! @param[out] *buf buffer to write into
//! @param[in] cap size of buf including terminator
//! @param[in] *src_file source file
//! @param[in] src_line source line
//! @param[in] *src_func source function
//! @retval -1 no buffer
int flog_get_str_src_info(char *buf, size_t cap, const char *src_file, const uint_fast16_t src_line, const char *src_func)
{
	if(!buf || cap==0)
		return(-1);
	size_t len=0;
	buf[0]='\0';
	if(src_file)
		sb_add(buf,cap,&len,"%s",src_file);
	if(src_line)
		sb_add(buf,cap,&len,":%d",(int)src_line);
	if(src_func)
		sb_add(buf,cap,&len,"%s%s()",len?"|":"",src_func);
	return((int)len);
}
#endif


//! Write the message header, or nothing

//! Timestamp, source info and subsystem, whichever are present, separated by
//! single spaces.
//!
//! @param[out] *buf buffer to write into
//! @param[in] cap size of buf including terminator
//! @param[in] *p flog message
//! @retval -1 no buffer
int flog_get_str_message_header(char *buf, size_t cap, const flog_msg_t *p)
{
	if(!buf || cap==0)
		return(-1);
	size_t len=0;
	buf[0]='\0';
#ifdef FLOG_CONFIG_TIMESTAMP
	char str_timestamp[FLOG_CONFIG_STR_MAX];
	if(flog_get_str_iso_timestamp(str_timestamp,sizeof(str_timestamp),p->timestamp)>0)
		sb_add(buf,cap,&len,"%s",str_timestamp);
#endif
#ifdef FLOG_CONFIG_SRC_INFO
	char str_src_info[FLOG_CONFIG_STR_MAX];
	if(flog_get_str_src_info(str_src_info,sizeof(str_src_info),p->src_file,p->src_line,p->src_func)>0)
		sb_add(buf,cap,&len,"%s%s",len?" ":"",str_src_info);
#endif
	if(p->subsystem)
		sb_add(buf,cap,&len,"%s%s",len?" ":"",p->subsystem);
	return((int)len);
}


//! Write the message contents, or nothing

//! Type, message id and text, whichever are present, separated by ": ".
//!
//! @param[out] *buf buffer to write into
//! @param[in] cap size of buf including terminator
//! @param[in] type type of message
//! @param[in] msg_id msg ID
//! @param[in] *text custom message string
//! @retval -1 no buffer
int flog_get_str_message_content(char *buf, size_t cap, const flog_msg_type_t type, const flog_msg_id_t msg_id, const char *text)
{
	if(!buf || cap==0)
		return(-1);
	size_t len=0;
	buf[0]='\0';
	char part[FLOG_CONFIG_STR_MAX];
	if(flog_get_str_msg_type(part,sizeof(part),type)>0)
		sb_add(buf,cap,&len,"%s",part);
	if(flog_get_str_msg_id(part,sizeof(part),msg_id)>0)
		sb_add(buf,cap,&len,"%s%s",len?": ":"",part);
	if(text)
		sb_add(buf,cap,&len,"%s%s",len?": ":"",text);
	return((int)len);
}


//! Write a whole rendered line from a flog_msg_t

//! "[header] content\n", with either half omitted when empty, and nothing at
//! all when both are.
//!
//! @param[out] *buf buffer to write into
//! @param[in] cap size of buf including terminator
//! @param[in] *p flog message struct
//! @retval -1 no buffer
int flog_get_str_message(char *buf, size_t cap, const flog_msg_t *p)
{
	if(!buf || cap==0)
		return(-1);
	size_t len=0;
	buf[0]='\0';
	char header[FLOG_CONFIG_STR_MAX],content[FLOG_CONFIG_STR_MAX];
	int have_header = flog_get_str_message_header(header,sizeof(header),p)>0;
	int have_content = flog_get_str_message_content(content,sizeof(content),p->type,p->msg_id,p->text)>0;
	if(have_header) {
		if(have_content)
			sb_add(buf,cap,&len,"[%s] %s\n",header,content);
		else
			sb_add(buf,cap,&len,"[%s]\n",header);
	} else if(have_content) {
		sb_add(buf,cap,&len,"%s\n",content);
	}
	return((int)len);
}

#endif //FLOG_CONFIG_STRING_OUTPUT
