//! Output string routines for Flog

//! @file flog_string.h
//! @author Nabeel Sowan (nabeel.sowan@vibes.se)
//!
//! To convert flog messages to strings

#ifndef FLOG_STRING_H
#define FLOG_STRING_H

#include "flog.h"
#include <stddef.h>

#ifdef FLOG_CONFIG_STRING_OUTPUT

//! These write into a buffer the CALLER owns and return the number of
//! characters written, excluding the terminator, or -1 if given no buffer.
//! Nothing to render is 0 and an empty string, not NULL.
//!
//! They took a `char **` and handed back an allocation the caller had to
//! free. That signature is an allocation contract: there is no way to honour
//! it without allocating, so it was the one part of flog that could not be
//! made to work the same way on a target with no allocator. A buffer the
//! caller already has is the same information with no such requirement, and
//! it is usually a buffer the caller was going to copy into anyway.
#ifdef FLOG_CONFIG_TIMESTAMP
int flog_get_str_iso_timestamp(char *buf, size_t cap, const flog_timestamp_t ts);
#endif //FLOG_CONFIG_TIMESTAMP
int flog_get_str_msg_type(char *buf, size_t cap, const flog_msg_type_t type);
int flog_get_str_msg_id(char *buf, size_t cap, const flog_msg_id_t msg_id);
#ifdef FLOG_CONFIG_SRC_INFO
int flog_get_str_src_info(char *buf, size_t cap, const char *src_file, const uint_fast16_t src_line, const char *src_func);
#endif //FLOG_CONFIG_SRC_INFO
int flog_get_str_message_header(char *buf, size_t cap, const flog_msg_t *p);
int flog_get_str_message_content(char *buf, size_t cap, const flog_msg_type_t type, const flog_msg_id_t msg_id, const char *text);
int flog_get_str_message(char *buf, size_t cap, const flog_msg_t *p);

#endif //FLOG_CONFIG_STRING_OUTPUT

#endif //FLOG_STRING_H
