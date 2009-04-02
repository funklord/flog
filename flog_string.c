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

#ifdef FLOG_TIMESTAMP
#include <time.h>


//! Create a string with timestamp in ISO-format

//! @param[out] **strp string to set (NULL on error)
//! @param[in] ts timestamp
//! @retval 0 success
int flog_get_str_iso_timestamp(char **strp, const FLOG_TIMESTAMP_T ts)
{
#ifdef FLOG_TIMESTAMP_USEC
	//! @todo implement FLOG_TIMESTAMP_USEC support
#else //FLOG_TIMESTAMP_USEC
	struct tm ts_tm;
	ts_tm = *localtime(&ts);
	if(asprintf(strp,"%04d-%02d-%02d %02d:%02d:%02d", ts_tm.tm_year+1900, ts_tm.tm_mon+1, ts_tm.tm_mday, ts_tm.tm_hour, ts_tm.tm_min, ts_tm.tm_sec)==-1) {
		*strp=NULL;
		return(-1);
	}
#endif //FLOG_TIMESTAMP_USEC
	return(0);
}
#endif //FLOG_TIMESTAMP


//! Create a string or NULL according to message type

//! @param[out] **strp string to set (NULL on error)
//! @param[in] type type of message
//! @retval 0 success
int flog_get_str_msg_type(char **strp, const FLOG_MSG_TYPE_T type)
{
	*strp=NULL;
	switch(type)
	{
		case FLOG_CRITICAL:
			if(!(*strp=strdup("Critical")))
				return(-1);
			break;
		case FLOG_ERROR:
			if(!(*strp=strdup("Error")))
				return(-1);
			break;
		case FLOG_WARNING:
			if(!(*strp=strdup("Warning")))
				return(-1);
			break;
		case FLOG_NOTIFY:
			if(!(*strp=strdup("!")))
				return(-1);
			break;
		case FLOG_INFO:
			break;
		case FLOG_VERBOSE:
			break;
		case FLOG_DEBUG:
			if(!(*strp=strdup("Debug")))
				return(-1);
			break;
		case FLOG_DEEP_DEBUG:
			if(!(*strp=strdup("Deep debug")))
				return(-1);
			break;
		default:
			break;
	}
	return(0);
}


#ifdef FLOG_MSG_ID_STRINGS
extern const char *flog_msg_id_str[];
#endif //FLOG_MSG_ID_STRINGS


//! Create a string from FLOG_MSG_ID

//! @param[out] **strp string to set (NULL on error)
//! @param[in] msg_id message ID type
//! @retval 0 success
int flog_get_str_msg_id(char **strp, const FLOG_MSG_ID_T msg_id)
{
	*strp=NULL;
	if(msg_id==0)
		return(0);
	if(msg_id>=FLOG_MSG_ID_AMOUNT_RESERVED_FOR_ERRNO) {
#ifdef FLOG_MSG_ID_STRINGS
		//! @todo we need to run toupper() on the first char of the message (maybe another function?)
#ifdef FLOG_MSG_ID_OUTPUT_SHOW_ID
		if(asprintf(strp,"(%d) %s", msg_id, flog_msg_id_str[msg_id-FLOG_MSG_ID_AMOUNT_RESERVED_FOR_ERRNO])==-1) {
			*strp=NULL;
#else //FLOG_MSG_ID_OUTPUT_SHOW_ID
		if(!(*strp=strdup(flog_msg_id_str[msg_id-FLOG_MSG_ID_AMOUNT_RESERVED_FOR_ERRNO]))) {
#endif //FLOG_MSG_ID_OUTPUT_SHOW_ID
#else //FLOG_MSG_ID_STRINGS
		if(asprintf(strp,"%d", msg_id)==-1) {
			*strp=NULL;
#endif //FLOG_MSG_ID_STRINGS
			return(-1);
		}
	} else {
		//! @todo make thread safe with strerror_r()
#ifdef FLOG_ERRNO_STRINGS
#ifdef FLOG_MSG_ID_OUTPUT_SHOW_ID
		if(asprintf(strp,"(%d) %s", msg_id, strerror(msg_id))==-1) {
			*strp=NULL;
#else //FLOG_MSG_ID_OUTPUT_SHOW_ID
		if(!(*strp=strdup(strerror(msg_id)))) {
#endif //FLOG_MSG_ID_OUTPUT_SHOW_ID
#else //FLOG_ERRNO_STRINGS
		if(asprintf(strp,"(%d)", msg_id)==-1) {
			*strp=NULL;
#endif //FLOG_ERRNO_STRINGS
			return(-1);
		}
	}
	return(0);
}


#ifdef FLOG_SRC_INFO
//! Create a string from flog source info

//! @param[out] **strp string to set (NULL on error)
//! @param[in] *src_file source file
//! @param[in] src_line source line
//! @param[in] *src_func source function
//! @retval 0 success
int flog_get_str_src_info(char **strp, const char *src_file, const int src_line, const char *src_func)
{
	*strp=NULL;
	if(src_file) {
		if(src_line!=-1) {
			if(src_func) {
				if(asprintf(strp,"%s:%d|%s()",src_file,(int)src_line,src_func)==-1) {
					*strp=NULL;
					return(-1);
				}
			} else {
				if(asprintf(strp,"%s:%d",src_file,(int)src_line)==-1) {
					*strp=NULL;
					return(-1);
				}
			}
		} else {
			if(src_func) {
				if(asprintf(strp,"%s|%s()",src_file,src_func)==-1) {
					*strp=NULL;
					return(-1);
				}
			} else {
				if(!(*strp=strdup(src_file)))
					return(-1);
			}
		}
	} else {
		if(src_line!=-1) {
			if(src_func) {
				if(asprintf(strp,":%d|%s()",(int)src_line,src_func)==-1) {
					*strp=NULL;
					return(-1);
				}
			} else {
				if(asprintf(strp,":%d",(int)src_line)==-1) {
					*strp=NULL;
					return(-1);
				}
			}
		} else {
			if(src_func) {
				if(asprintf(strp,"%s()",src_func)==-1) {
					*strp=NULL;
					return(-1);
				}
			}	else {
				*strp=NULL;
			}
		}
	}
	return(0);
}
#endif


//! Create a string with message header

//! @param[out] **strp string to set (NULL on error)
//! @param[in] *p flog message type
//! @retval 0 success
int flog_get_str_message_header(char **strp, const FLOG_MSG_T *p)
{
	*strp=NULL;
#ifdef FLOG_TIMESTAMP
	char *str_timestamp;
	if(flog_get_str_iso_timestamp(&str_timestamp, p->timestamp))
		return(-1);
#endif
#ifdef FLOG_SRC_INFO
	char *str_src_info;
	if(flog_get_str_src_info(&str_src_info, p->src_file, p->src_line, p->src_func)) {
#ifdef FLOG_TIMESTAMP
		free(str_timestamp);
#endif
		return(-1);
	}
#endif

	if(p->subsystem) {
#ifdef FLOG_SRC_INFO
		if(str_src_info) {
#ifdef FLOG_TIMESTAMP
			if(str_timestamp) {
				if(asprintf(strp,"%s %s %s", str_timestamp, str_src_info, p->subsystem)==-1) {
					free(str_timestamp);
					free(str_src_info);
					*strp=NULL;
					return(-1);
				}
				free(str_timestamp);
			} else {
#endif
				if(asprintf(strp,"%s %s", str_src_info, p->subsystem)==-1) {
					free(str_src_info);
					*strp=NULL;
					return(-1);
				}
#ifdef FLOG_TIMESTAMP
			}
#endif
			free(str_src_info);
		} else {
#endif
#ifdef FLOG_TIMESTAMP
			if(str_timestamp) {
				if(asprintf(strp,"%s %s", str_timestamp, p->subsystem)==-1) {
					free(str_timestamp);
					*strp=NULL;
					return(-1);
				}
				free(str_timestamp);
			} else {
#endif
				if(!(*strp=strdup(p->subsystem)))
					return(-1);
#ifdef FLOG_TIMESTAMP
			}
#endif
#ifdef FLOG_SRC_INFO
		}
#endif
	} else {
#ifdef FLOG_SRC_INFO
		if(str_src_info) {
#ifdef FLOG_TIMESTAMP
			if(str_timestamp) {
				if(asprintf(strp,"%s %s", str_timestamp, str_src_info)==-1) {
					free(str_timestamp);
					free(str_src_info);
					*strp=NULL;
					return(-1);
				}
				free(str_timestamp);
			} else {
#endif
				if(!(*strp=strdup(str_src_info))) {
					free(str_src_info);
					return(-1);
				}
#ifdef FLOG_TIMESTAMP
			}
#endif
			free(str_src_info);
		} else {
#endif
#ifdef FLOG_TIMESTAMP
			if(str_timestamp) {
				if(!(*strp=strdup(str_timestamp))) {
					free(str_timestamp);
					return(-1);
				}
				free(str_timestamp);
			} else {
#endif
				*strp=NULL;
#ifdef FLOG_TIMESTAMP
			}
#endif
#ifdef FLOG_SRC_INFO
		}
#endif
	}
	return(0);
}


//! Create a string with message contents

//! @param[out] **strp string to set (NULL on error)
//! @param[in] type type of message
//! @param[in] msg_id msg ID
//! @param[in] *text custom message string
//! @retval 0 success
int flog_get_str_message_content(char **strp, const FLOG_MSG_TYPE_T type, const FLOG_MSG_ID_T msg_id, const char *text)
{
	char *str_type,*str_msg_id;
	*strp=NULL;
	if(flog_get_str_msg_type(&str_type,type))
		return(-1);
	if(flog_get_str_msg_id(&str_msg_id,msg_id)) {
		free(str_type);
		return(-1);
	}
	if(str_type) {
		if(str_msg_id) {
			if(text) {
				if(asprintf(strp,"%s: %s: %s",str_type,str_msg_id,text)==-1) {
					free(str_type);
					free(str_msg_id);
					*strp=NULL;
					return(-1);
				}
			} else {
				if(asprintf(strp,"%s: %s",str_type,str_msg_id)==-1) {
					free(str_type);
					free(str_msg_id);
					*strp=NULL;
					return(-1);
				}
			}
			free(str_msg_id);
		} else {
			if(text) {
				if(asprintf(strp,"%s: %s",str_type,text)==-1) {
					free(str_type);
					*strp=NULL;
					return(-1);
				}
			} else {
				if(!(*strp=strdup(str_type))) {
					free(str_type);
					return(-1);
				}
			}
		}
		free(str_type);
	} else {
		if(str_msg_id) {
			if(text) {
				if(asprintf(strp,"%s: %s",str_msg_id,text)==-1) {
					free(str_msg_id);
					*strp=NULL;
					return(-1);
				}
			} else {
				if(!(*strp=strdup(str_msg_id))) {
					free(str_msg_id);
					return(-1);
				}
			}
			free(str_msg_id);
		} else {
			if(text) {
				if(!(*strp=strdup(text)))
					return(-1);
			} else
				*strp=NULL;
		}
	}
	return(0);
}


//! Create and return a string from FLOG_MSG_T type

//! @param[out] **strp string to set (NULL on error)
//! @param[in] *p flog message struct
//! @retval 0 success
int flog_get_str_message(char **strp, const FLOG_MSG_T *p)
{
	char *str_msg_header, *str_msg_content;
	*strp=NULL;
	if(flog_get_str_message_header(&str_msg_header,p))
		return(-1);
	if(flog_get_str_message_content(&str_msg_content, p->type, p->msg_id, p->text)) {
		free(str_msg_header);
		return(-1);
	}
	if(str_msg_header) {
		if(str_msg_content) {
			if(asprintf(strp,"[%s] %s\n", str_msg_header, str_msg_content)==-1) {
				free(str_msg_content);
				free(str_msg_header);
				*strp=NULL;
				return(-1);
			}
			free(str_msg_content);
		} else {
			if(asprintf(strp,"[%s]\n", str_msg_header)==-1) {
				free(str_msg_header);
				*strp=NULL;
				return(-1);
			}
		}
		free(str_msg_header);
	} else {
		if(str_msg_content) {
			if(asprintf(strp,"%s\n", str_msg_content)==-1) {
			free(str_msg_content);
			*strp=NULL;
			return(-1);
			}
			free(str_msg_content);
		} else
			*strp=NULL;
	}
	return(0);
}


/*
char * flog_msg_t_to_str(const FLOG_MSG_T *p)
{
	char *str,*typestr;
	typestr=flog_get_msg_type_str(p->type);
#ifdef FLOG_TIMESTAMP
	//! @todo add timestamp support
#else
#ifdef FLOG_SRC_INFO
	if((p->subsystem != NULL) && (typestr != NULL))
		asprintf(&str,"[%s:%d|%s() %s] %s%s\n",p->src_file,(int)p->src_line,p->src_func,p->subsystem,typestr,p->text);
	else if((p->subsystem != NULL) && (typestr == NULL))
		asprintf(&str,"[%s:%d|%s() %s] %s\n",p->src_file,(int)p->src_line,p->src_func,p->subsystem,p->text);
	else if((p->subsystem == NULL) && (typestr != NULL))
		asprintf(&str,"[%s:%d|%s()] %s%s\n",p->src_file,(int)p->src_line,p->src_func,typestr,p->text);
	else
		asprintf(&str,"[%s:%d|%s()] %s\n",p->src_file,(int)p->src_line,p->src_func,p->text);
#else
	if((p->subsystem != NULL) && (typestr != NULL))
		asprintf(&str,"[%s] %s%s\n",p->subsystem,typestr,p->text);
	else if((p->subsystem != NULL) && (typestr == NULL))
		asprintf(&str,"[%s] %s\n",p->subsystem,p->text);
	else if((p->subsystem == NULL) && (typestr != NULL))
		asprintf(&str,"%s%s\n",typestr,p->text);
	else
		asprintf(&str,"%s\n",p->text);
#endif //FLOG_SRC_INFO
#endif //FLOG_TIMESTAMP
	free(typestr);
	return(str);
}
*/

#endif //FLOG_CONFIG_STRING_OUTPUT
