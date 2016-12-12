#include "parameters.h"

char data_suffix[512]   =   "";   //Whether to save the histogram of sampling in sectors of different n and order
char scan_filename[512] =   "";
char scan_label[512]    =   "";

static struct option long_options[] =
{
 {   "data-suffix", required_argument, NULL,   'a'},
 { "scan-filename", required_argument, NULL,   'b'},
 {    "scan-label", required_argument, NULL,   'c'},
 {               0,                 0, NULL,     0}
};

static char my_short_option_list[] = "a:b:c:";

int parse_command_line_options(int argc, char **argv)
{
 int gc, option_index;
 
 while(1)
 {
  gc = getopt_long(argc, argv, my_short_option_list, long_options, &option_index);
  if(gc==-1)
   break;
  switch(gc)
  {
   case 'a':
    strcpy(suffix, optarg);
   break;
   case 'b':
    strcpy(scan_filename, optarg);
   break;
   case 'c':
    strcpy(scan_label, optarg);
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
  
 return 0;
}

void init_parameters()
{
  
}

void print_parameters()
{
 logs_Write(0, "\n");
 logs_WriteParameter(0,   "Data suffix",  "%s", suffix);
 logs_WriteParameter(0, "Scan filename",  "%s", scan_filename);
 logs_WriteParameter(0,   "Scan suffix",  "%s", scan_label);
 logs_Write(0, "\n"); 
}
