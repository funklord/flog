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
int flog_get_str_iso_timestamp(char **strp, const flog_timestamp_t ts);
#endif //FLOG_CONFIG_TIMESTAMP
int flog_get_str_msg_type(char **strp, const flog_msg_type_t type);
int flog_get_str_msg_id(char **strp, const flog_msg_id_t msg_id);
#ifdef FLOG_CONFIG_SRC_INFO
int flog_get_str_src_info(char **strp, const char *src_file, const uint_fast16_t src_line, const char *src_func);
#endif //FLOG_CONFIG_SRC_INFO
int flog_get_str_message_header(char **strp, const flog_msg_t *p);
int flog_get_str_message_content(char **strp, const flog_msg_type_t type, const flog_msg_id_t msg_id, const char *text);
int flog_get_str_message(char **strp, const flog_msg_t *p);

#endif //FLOG_CONFIG_STRING_OUTPUT

#endif //FLOG_STRING_H
