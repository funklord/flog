//! Output string routines for Flog

//! @file flog_string.h
//! @author Nabeel Sowan (nabeel.sowan@vibes.se)
//!
//! To convert flog messages to strings

#ifndef FLOG_STRING_H
#define FLOG_STRING_H

#include "flog.h"

#ifdef FLOG_CONFIG_STRING_OUTPUT

#ifdef FLOG_CONFIG_TIMESTAMP
int flog_get_str_iso_timestamp(char **strp, const FLOG_TIMESTAMP_T ts);
#endif //FLOG_CONFIG_TIMESTAMP
int flog_get_str_msg_type(char **strp, const FLOG_MSG_TYPE_T type);
int flog_get_str_msg_id(char **strp, const FLOG_MSG_ID_T msg_id);
#ifdef FLOG_CONFIG_SRC_INFO
int flog_get_str_src_info(char **strp, const char *src_file, const uint_fast16_t src_line, const char *src_func);
#endif //FLOG_CONFIG_SRC_INFO
int flog_get_str_message_header(char **strp, const FLOG_MSG_T *p);
int flog_get_str_message_content(char **strp, const FLOG_MSG_TYPE_T type, const FLOG_MSG_ID_T msg_id, const char *text);
int flog_get_str_message(char **strp, const FLOG_MSG_T *p);

#endif //FLOG_CONFIG_STRING_OUTPUT

#endif //FLOG_STRING_H
