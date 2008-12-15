/*!
	@file flog_file.h
	@brief file output for Flog
	@author Nabeel Sowan (nabeel.sowan@vibes.se)
	
	When you want flog to write to a file
	Choose logfile name by setting data to string
*/

#ifndef FLOG_FILE_H
#define FLOG_FILE_H

#include "flog.h"

int flog_output_file(FLOG_T *log,const FLOG_MSG_T *msg);

#endif
