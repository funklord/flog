//! Simple example program

#include "flog.h"
#include "flog_output_stdio.h"
#include "flog_output_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


int main(void)
{
	FLOG_T *log_main,*log_subfunc;
	printf("-[flog test start]-\n");

	//create logs
	log_main = create_flog_t(__func__,FLOG_ACCEPT_DEEP_DEBUG);
	log_main->error_log=log_main;
	log_subfunc = create_flog_t("subfunc",FLOG_ACCEPT_DEEP_DEBUG);
	log_subfunc->error_log=log_main;
	flog_append_sublog(log_subfunc,log_main);
#ifdef FLOG_CONFIG_OUTPUT_STDIO
	FLOG_T *log_stdout,*log_stderr;
	log_stdout = create_flog_output_stdout("stdout",FLOG_ACCEPT_ONLY_ERROR);
	log_stdout->error_log=log_main;
	flog_append_sublog(log_main,log_stdout);
	log_stderr = create_flog_output_stderr("stderr",FLOG_ACCEPT_ALL);
	log_stderr->error_log=log_main;
	flog_append_sublog(log_main,log_stderr);
#endif
#ifdef FLOG_CONFIG_OUTPUT_FILE
	FLOG_T *log_file;
	log_file = create_flog_output_file("file",FLOG_ACCEPT_ALL,"test.log");
	log_file->error_log=log_main;
	flog_append_sublog(log_main,log_file);
#endif

	flog_print(log_subfunc,"print_test",FLOG_ERROR,0,"testing...");
	flog_printf(log_subfunc,"printf_test",FLOG_INFO,0,"testing... %d %d %d",1,2,3);

	flog_print(log_subfunc,NULL,FLOG_ERROR,FLOG_MSG_MARK,NULL);
	flog_print(log_subfunc,NULL,FLOG_ERROR,ENOMEM,NULL);

	flog_assert(log_subfunc,1+1);
	flog_assert(log_subfunc,1-1);

#ifdef DEBUG
	printf("-[flog test suite]-\n");
	flog_test(log_main);
#endif

	//int i;
	//for(i=0;i<10000000;i++)
		//flog_print(log_subfunc,NULL,FLOG_DEEP_DEBUG,FLOG_MSG_MARK,NULL);
	//clean up
	destroy_flog_t(log_subfunc);
	destroy_flog_t(log_main);
#ifdef FLOG_CONFIG_OUTPUT_STDIO
	destroy_flog_t(log_stdout);
	destroy_flog_t(log_stderr);
#endif
#ifdef FLOG_CONFIG_OUTPUT_FILE
	destroy_flog_output_file(log_file);
#endif
	return(0);
}
