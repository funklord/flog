/*!
	@file flog_file.h
	@brief file output for Flog
	@author Nabeel Sowan (nabeel.sowan@vibes.se)
	
	When you want flog to write to a file
*/

#ifndef FLOG_FILE_H
#define FLOG_FILE_H

#include <flog.h>

int flog_output_file(const FLOG_MSG_T *p,void *data);

#endif
