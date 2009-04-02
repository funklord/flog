//! file output for Flog

//! @file flog_output_file.h
//! @author Nabeel Sowan (nabeel.sowan@vibes.se)
//!
//! When you want flog to write to a file
//! Choose logfile name by setting data to string


#ifndef FLOG_OUTPUT_FILE_H
#define FLOG_OUTPUT_FILE_H

#include "flog.h"

#ifdef FLOG_CONFIG_OUTPUT_FILE

int flog_output_file(FLOG_T *log,const FLOG_MSG_T *msg);
FLOG_T * create_flog_output_file(const char *name, FLOG_MSG_TYPE_T accepted_msg_type, const char *filename);
void destroy_flog_output_file(FLOG_T *p);

#endif //FLOG_CONFIG_OUTPUT_FILE

#endif //FLOG_OUTPUT_FILE_H
