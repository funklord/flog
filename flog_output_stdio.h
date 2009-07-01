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

int flog_output_stdout(FLOG_T *log,const FLOG_MSG_T *msg);
int flog_output_stderr(FLOG_T *log,const FLOG_MSG_T *msg);

FLOG_T * create_flog_output_stdout(const char *name, FLOG_MSG_TYPE_T accepted_msg_type);
FLOG_T * create_flog_output_stderr(const char *name, FLOG_MSG_TYPE_T accepted_msg_type);

#endif //FLOG_CONFIG_OUTPUT_STDIO

#endif //FLOG_OUTPUT_STDIO_H
