/*!
	@file flog_file.c
	@brief file output for Flog
	@author Nabeel Sowan (nabeel.sowan@vibes.se)
	
	When you want flog to write to a file
*/

#include "flog_file.h"
#include <stdlib.h>
#include <stdio.h>

int flog_output_file(const FLOG_MSG_T *p,void *data __attribute__((__unused__)))
{
	char *str;
	if((str=flog_msg_t_to_str(p))==NULL)
		return(1);
	
	FILE *f;
	if((f = fopen("logfile","a+t"))==NULL) //! @todo implement this
		return(1);
	if(fprintf(f,str)<0) {
		free(str);
		return(1);
	}
	fclose(f);
	
	free(str);
	return(0);
}
