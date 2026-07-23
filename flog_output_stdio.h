//! stdio output for Flog

//! @file flog_output_stdio.h
//! @author Nabeel Sowan (nabeel.sowan@vibes.se)
//!
//! When you want flog to write to stdout or stderr


#ifndef FLOG_OUTPUT_STDIO_H
#define FLOG_OUTPUT_STDIO_H

#include "flog.h"

#ifdef FLOG_CONFIG_OUTPUT_STDIO

// Sanity checks
#ifndef FLOG_CONFIG_STRING_OUTPUT
#error FLOG_CONFIG_OUTPUT_STDIO requires FLOG_CONFIG_STRING_OUTPUT
#endif
#ifndef FLOG_CONFIG_MSG_ID_STRINGS
#error FLOG_CONFIG_OUTPUT_STDIO requires FLOG_CONFIG_MSG_ID_STRINGS
#endif
#ifndef FLOG_CONFIG_ERRNO_STRINGS
#error FLOG_CONFIG_OUTPUT_STDIO requires FLOG_CONFIG_ERRNO_STRINGS
#endif

int flog_output_stdout(flog_t *log,const flog_msg_t *msg);
int flog_output_stderr(flog_t *log,const flog_msg_t *msg);

flog_t * create_flog_output_stdout(const char *name, flog_msg_type_t accepted_msg_type);
flog_t * create_flog_output_stderr(const char *name, flog_msg_type_t accepted_msg_type);

#endif //FLOG_CONFIG_OUTPUT_STDIO

#endif //FLOG_OUTPUT_STDIO_H
