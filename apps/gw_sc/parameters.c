#include "parameters.h"

char* gtotal_file       =   NULL;

static struct option long_options[] =
{
 METROPOLIS_LONG_OPTIONS,
 LARGEN_QFT_LONG_OPTIONS,
 {           "gtotal-file",  required_argument,                       NULL,  'a'},
 {                       0,                  0,                       NULL,    0}
};

static char my_short_option_list[] = "a:";

int parse_command_line_options(int argc, char **argv)
{
 int gc, option_index;
 //Compose the joint list of all short options
 int short_option_list_length = 3;
 short_option_list_length += strlen(   metropolis_short_option_list);
 short_option_list_length += strlen(   largeN_QFT_short_option_list);
 short_option_list_length += strlen(           my_short_option_list);
 char* short_option_list = NULL;
 SAFE_MALLOC(short_option_list, char, short_option_list_length);
 sprintf(short_option_list, "%s",     my_short_option_list);
 strcat(short_option_list,    metropolis_short_option_list);
 strcat(short_option_list,    largeN_QFT_short_option_list);

 while(1)
 {
  gc = getopt_long(argc, argv, short_option_list, long_options, &option_index);
  if(gc==-1)
   break;
  switch(gc)
  {
   PARSE_METROPOLIS_OPTIONS;
   PARSE_LARGEN_QFT_OPTIONS;
   case 'a':
    COPY_FILE_NAME(optarg, gtotal_file);
   break;
   case   0:
   break;
   case '?':
	printhelp();
   break;	
   default:
    printhelp();       
   break;
  }; 
 };
 
 SAFE_FREE(short_option_list);  
 return 0;
}

void init_parameters()
{
 if(param_auto_tuning)
 {
  cc = 1.0; NN = 1.0;
  find_cc_NN_minimum(param_tuning_accuracy, &max_ampl_sum);
  if(max_ampl_sum>0.0)
  {
   control_max_ampl_sum = 1;
   max_ampl_sum_tol     = 0.001*max_ampl_sum;                   
  };
 };
 check_cc_NN_minimum(0.05);    
}

void print_parameters()
{
 logs_Write(0, "\n");
 print_metropolis_parameters();
 print_largeN_QFT_parameters();
 logs_Write(0, "OTHER PARAMETERS\n");
 logs_WriteParameter(0, "gtotal_file", "%s", gtotal_file);
}


