/*!
	@file flog_stdio.h
	@brief stdio output for Flog
	@author Nabeel Sowan (nabeel.sowan@vibes.se)
	
	When you want flog to write to a stdout or stderr
*/

#ifndef FLOG_STDIO_H
#define FLOG_STDIO_H

#include "flog.h"

int flog_output_stdout(const FLOG_MSG_T *p,void *data);
int flog_output_stderr(const FLOG_MSG_T *p,void *data);

#endif
