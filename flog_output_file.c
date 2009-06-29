//! file output for Flog

//! @file flog_output_file.c
//! @author Nabeel Sowan (nabeel.sowan@vibes.se)
//!
//! When you want flog to write to a file
//! Choose logfile name by setting data to string


#include "flog_output_file.h"

#ifdef FLOG_CONFIG_OUTPUT_FILE

#include "flog_string.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


//! Output function for simple log output to a file
//! filename is stored in log.output_func_data as a string

//! @retval 0 success
int flog_output_file(FLOG_T *log,const FLOG_MSG_T *msg)
{
	if(log->output_func_data==NULL) {
		log->output_error=-1;
		flog_print(log->error_log,"flog_output_file",FLOG_ERROR,FLOG_MSG_SET_OUTPUT_FILE,NULL);
		return(log->output_error);
	}
	char *str;
	if(flog_get_str_message(&str,msg))
		return(-1);

	FILE *f;
	if((f = fopen(log->output_func_data,"a+t"))==NULL) {
		log->output_error=errno;
		free(str);
		flog_printf(log->error_log,"fopen",FLOG_ERROR,FLOG_MSG_CANNOT_OPEN_FILE,"%s (%s)", log->output_func_data, strerror(log->output_error));
		return(log->output_error);
	}
	if(fputs(str,f)==EOF) {
		log->output_error=errno;
		free(str);
		fclose(f); //close to avoid multiple fp recursion
		flog_printf(log->error_log,"fprintf",FLOG_ERROR,FLOG_MSG_CANNOT_WRITE_FILE,"%s (%s)", log->output_func_data, strerror(log->output_error));
		return(log->output_error);
	}
	free(str);
	if(fclose(f)==EOF) {
		log->output_error=errno;
		flog_printf(log->error_log,"fclose",FLOG_ERROR,FLOG_MSG_CANNOT_WRITE_FILE,"%s (%s)", log->output_func_data, strerror(log->output_error));
		return(log->output_error);
	}
	return(0);
}


//! create and return a log that writes to file

//! @retval NULL error
FLOG_T * create_flog_output_file(const char *name, FLOG_MSG_TYPE_T accepted_msg_type, const char *filename)
{
	FLOG_T *p;
	if((p=create_flog_t(name,accepted_msg_type))==NULL)
		return(NULL);
	p->output_func=flog_output_file;
	if(filename && filename[0]) {
		if((p->output_func_data=strdup(filename))==NULL) {
			destroy_flog_t(p);
			return(NULL);
		}
	}
	return(p);
}


//! free an output_file FLOG_T
void destroy_flog_output_file(FLOG_T *p)
{
	if(p!=NULL) {
		free(p->output_func_data);
		p->output_func_data=NULL;
		destroy_flog_t(p);
	}
}


#endif //FLOG_CONFIG_OUTPUT_FILE
