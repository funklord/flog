//! stdio output for Flog

//! @file flog_output_stdio.h
//! @author Nabeel Sowan (nabeel.sowan@vibes.se)
//!
//! When you want flog to write to stdout or stderr


#ifndef FLOG_OUTPUT_STDIO_H
#define FLOG_OUTPUT_STDIO_H

#include "flog.h"

#ifdef FLOG_OUTPUT_STDIO

int flog_output_stdout(FLOG_T *log,const FLOG_MSG_T *msg);
int flog_output_stderr(FLOG_T *log,const FLOG_MSG_T *msg);

FLOG_T * create_flog_output_stdout(const char *name, FLOG_MSG_TYPE_T accepted_msg_type);
FLOG_T * create_flog_output_stderr(const char *name, FLOG_MSG_TYPE_T accepted_msg_type);

#endif //FLOG_OUTPUT_STDIO

#endif //FLOG_OUTPUT_STDIO_H
