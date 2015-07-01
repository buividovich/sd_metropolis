#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <clue_logs.h>
#include <rand_num_generators.h>

#include <sd_metropolis.h>

#include "parameters.h"
#include "actions.h"
#include "statistics.h"

//#include "detailed_statistics.h"

int main(int argc, char *argv[])
{
 ansi_colors = 1;
 print_errors_to_stderr = 0;
 
 logs_Write(0, "\n\nDIAGRAMMATIC MONTE-CARLO FOR HERMITIAN PHI4 MATRIX MODEL IN THE PLANAR LIMIT\n\n");
 
 init_rand(time(NULL));
 
 parse_command_line_options(argc, argv);
 init_actions();
 init_parameters();
 print_parameters();
 
 init_metropolis();
 init_observable_stat();
 
 logs_Write(0, "Starting thermalization process for %i MC steps", therm_mc_steps);
 for(int imc=0; imc<therm_mc_steps; imc++)
  metropolis_step(imc);
 
 logs_Write(0, "Starting production run for %i MC steps", prod_mc_steps);
 for(int imc=0; imc<prod_mc_steps; imc++)
 {
  metropolis_step(imc);
  if(imc%mc_interval==0)
  {
   gather_observable_stat();
   //gather_detailed_statistics();
  }; 
 }; 
 
 char prefix[500];
 largeN_QFT_prefix(prefix);
 process_mc_stat(prefix, 1);
 process_observable_stat();
 //print_detailed_statistics();
 
 free_observable_stat();
 free_metropolis();
 free_actions();
 free_parameters();
 
 return EXIT_SUCCESS;
}
