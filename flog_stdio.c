/*!
	@file flog_stdio.c
	@brief stdio output for Flog
	@author Nabeel Sowan (nabeel.sowan@vibes.se)
	
	When you want flog to write to a stdout or stderr
*/

#include "flog_stdio.h"
#include <stdlib.h>
#include <stdio.h>

int flog_output_stdout(FLOG_T *log __attribute__((__unused__)),const FLOG_MSG_T *msg)
{
	char *str;
	if((str=flog_msg_t_to_str(msg))==NULL)
		return(1);
	if(fprintf(stdout,str)<0) {
		free(str);
		return(1);
	}
	free(str);
	return(0);
}

int flog_output_stderr(FLOG_T *log __attribute__((__unused__)),const FLOG_MSG_T *msg)
{
	char *str;
	if((str=flog_msg_t_to_str(msg))==NULL)
		return(1);
	if(fprintf(stderr,str)<0) {
		free(str);
		return(1);
	}
	free(str);
	return(0);
}
