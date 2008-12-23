/*!
	@file flog_file.c
	@brief file output for Flog
	@author Nabeel Sowan (nabeel.sowan@vibes.se)
	
	When you want flog to write to a file
	Choose logfile name by setting data to string
*/

#include "flog_file.h"
#include <stdlib.h>
#include <stdio.h>

int flog_output_file(FLOG_T *log,const FLOG_MSG_T *msg)
{
	char *str;
	int e=0;
	if((str=flog_msg_t_to_str(msg))==NULL)
		return(1);
	if(log->output_func_data==NULL) {
		log->output_error=1;
		flog_print(log->error_log,FLOG_ERROR,"flog_file_output","please set log output filename");
		e=1;
	} else {
		FILE *f;
		if((f = fopen(log->output_func_data,"a+t"))==NULL) {
			log->output_error=1;
			flog_printf(log->error_log,FLOG_ERROR,NULL,"cannot open file: %s",log->output_func_data);
			e=1;
		} else {
			if(fprintf(f,str)<0) {
				log->output_error=1;
				flog_printf(log->error_log,FLOG_ERROR,NULL,"cannot write to file: %s",log->output_func_data);
				e=1;
			}
			fclose(f);
		}
	}
	free(str);
	return(e);
}
