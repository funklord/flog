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
int flog_output_file(flog_t *log,const flog_msg_t *msg)
{
	if(log->output_func_data==NULL) {
		log->output_error=-1;
		flog_print(log->error_log,"flog_output_file",FLOG_ERROR,FLOG_MSG_SET_OUTPUT_FILE,NULL);
		return(log->output_error);
	}
	char str[FLOG_CONFIG_STR_MAX];
	if(flog_get_str_message(str,sizeof(str),msg)<0)
		return(-1);

	FILE *f;
	if((f = fopen(log->output_func_data,"a+t"))==NULL) {
		log->output_error=errno;
		flog_printf(log->error_log,"fopen",FLOG_ERROR,FLOG_MSG_CANNOT_OPEN_FILE,"%s (%s)", log->output_func_data, strerror(log->output_error));
		return(log->output_error);
	}
	if(fputs(str,f)==EOF) {
		log->output_error=errno;
		fclose(f); //close to avoid multiple fp recursion
		flog_printf(log->error_log,"fprintf",FLOG_ERROR,FLOG_MSG_CANNOT_WRITE_FILE,"%s (%s)", log->output_func_data, strerror(log->output_error));
		return(log->output_error);
	}
	if(fclose(f)==EOF) {
		log->output_error=errno;
		flog_printf(log->error_log,"fclose",FLOG_ERROR,FLOG_MSG_CANNOT_WRITE_FILE,"%s (%s)", log->output_func_data, strerror(log->output_error));
		return(log->output_error);
	}
	return(0);
}


//! create and return a log that writes to file

//! @retval NULL error
//! @brief Point a caller-owned flog_t at a file
//!
//! Name and filename are BORROWED rather than copied and must outlive the
//! log. There is nothing to destroy: destroy_flog_output_file() exists to
//! free the copy create_flog_output_file() made, and there is no copy here.
#ifndef FLOG_CONFIG_ALLOCATION
static char flog_file_name[FLOG_CONFIG_MAX_LOGS][FLOG_CONFIG_FILENAME_MAX];
static uint_fast8_t flog_file_name_used[FLOG_CONFIG_MAX_LOGS];
#endif //FLOG_CONFIG_ALLOCATION


void init_flog_output_file(flog_t *p, const char *name, flog_msg_type_t accepted_msg_type, const char *filename)
{
	init_flog_t(p);
	p->name=(char *)name;
	p->accepted_msg_type=accepted_msg_type;
	p->output_func=flog_output_file;
	p->output_func_data=(void *)filename;
}

flog_t * create_flog_output_file(const char *name, flog_msg_type_t accepted_msg_type, const char *filename)
{
	flog_t *p;
	if((p=create_flog_t(name,accepted_msg_type))==NULL)
		return(NULL);
	p->output_func=flog_output_file;
	if(filename && filename[0]) {
#ifdef FLOG_CONFIG_ALLOCATION
		if((p->output_func_data=strdup(filename))==NULL) {
			destroy_flog_t(p);
			return(NULL);
		}
#else
		//! A slot from the same fixed pool the log itself came from, so the
		//! filename is copied and outlives the caller's string exactly as
		//! strdup made it. Keyed on the log's own slot index, so it is
		//! released by destroy_flog_output_file() along with the log.
		uint_fast8_t i;
		for(i=0;i<FLOG_CONFIG_MAX_LOGS;i++) {
			if(!flog_file_name_used[i]) {
				flog_file_name_used[i]=1;
				snprintf(flog_file_name[i],FLOG_CONFIG_FILENAME_MAX,"%s",filename);
				p->output_func_data=flog_file_name[i];
				break;
			}
		}
		if(p->output_func_data==NULL) {
			destroy_flog_t(p);
			return(NULL);
		}
#endif //FLOG_CONFIG_ALLOCATION
	}
	return(p);
}


//! free an output_file flog_t
void destroy_flog_output_file(flog_t *p)
{
	if(p!=NULL) {
#ifdef FLOG_CONFIG_ALLOCATION
		free(p->output_func_data);
#else
		uint_fast8_t i;
		for(i=0;i<FLOG_CONFIG_MAX_LOGS;i++) {
			if(p->output_func_data==flog_file_name[i]) {
				flog_file_name_used[i]=0;
				break;
			}
		}
#endif //FLOG_CONFIG_ALLOCATION
		p->output_func_data=NULL;
		destroy_flog_t(p);
	}
}


#endif //FLOG_CONFIG_OUTPUT_FILE

