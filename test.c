#include "flog.h"
#include "flog_stdio.h"
#include "flog_file.h"
#include <stdio.h>
#include <stdlib.h>

//simple example program

int main(void)
{
	FLOG_T *log_main,*log_subfunc,*log_stdout,*log_stderr,*log_file;
	printf("-[flog test start]-\n");
	
	//create logs
	log_main = create_flog_t(__func__,FLOG_ACCEPT_ALL);
	log_subfunc = create_flog_t("subfunc",FLOG_ACCEPT_ALL);
	log_stdout = create_flog_t("stdout",FLOG_ACCEPT_ONLY_ERROR);
	log_stderr = create_flog_t("stderr",FLOG_ACCEPT_ALL);
	log_file = create_flog_t("file",FLOG_ACCEPT_ALL);
	
	//set error logs
	log_main->error_log=log_main;
	log_subfunc->error_log=log_main;
	log_stdout->error_log=log_main;
	log_stderr->error_log=log_main;
	log_file->error_log=log_main;
	
	//set output functions
	log_stdout->output_func=flog_output_stdout;
	log_stderr->output_func=flog_output_stderr;
	log_file->output_func=flog_output_file;
	
	//append them to eachother
	flog_append_sublog(log_subfunc,log_main);
	flog_append_sublog(log_main,log_stdout);
	flog_append_sublog(log_main,log_stderr);
	flog_append_sublog(log_main,log_file);
	
	flog_print(log_subfunc,FLOG_ERROR,"print_test","testing...");
	flog_printf(log_subfunc,FLOG_INFO,"printf_test","testing... %d %d %d",1,2,3);
	
	flog_assert(log_subfunc,1+1);
	flog_assert(log_subfunc,1-1);
	
#ifdef DEBUG
	printf("-[flog test suite]-\n");
	flog_test(log_main);
#endif
	
	//clean up
	destroy_flog_t(log_subfunc);
	destroy_flog_t(log_main);
	destroy_flog_t(log_stdout);
	destroy_flog_t(log_stderr);
	destroy_flog_t(log_file);
	return(0);
}
