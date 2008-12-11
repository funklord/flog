#include "flog.h"
#include "flog_stdio.h"
#include <stdio.h>
#include <stdlib.h>

//simple example program

int main(void)
{
	FLOG_T *log_main,*log_subfunc,*log_stdout,*log_stderr;
	printf("-[flog test start]-\n");
	
	//create logs
	log_main = create_flog_t(__func__,FLOG_SHOW_ALL);
	log_subfunc = create_flog_t("subfunc",FLOG_SHOW_ALL);
	log_stdout = create_flog_t("stdout",FLOG_SHOW_ALL);
	log_stderr = create_flog_t("stderr",FLOG_SHOW_ALL);
	
	//set output functions
	log_stdout->output_func=flog_output_stdout;
	log_stderr->output_func=flog_output_stderr;
	
	//append them to eachother
	flog_append_sublog(log_subfunc,log_main);
	flog_append_sublog(log_main,log_stdout);
	flog_append_sublog(log_main,log_stderr);
	
	flog_print(log_subfunc,FLOG_ERROR,"print_test","testing...");
	flog_printf(log_subfunc,FLOG_ERROR,"printf_test","testing... %d %d %d",1,2,3);
	
	printf("-[flog test suite]-\n");
	flog_test(log_main);
	
	//clean up
	destroy_flog_t(log_subfunc);
	destroy_flog_t(log_main);
	destroy_flog_t(log_stdout);
	destroy_flog_t(log_stderr);
	return(0);
}
