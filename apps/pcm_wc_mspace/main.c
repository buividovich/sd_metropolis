#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <clue_logs.h>
#include <rand_num_generators.h>

#include <sd_metropolis.h>
#include <lattice_propagator.h>

#include "parameters.h"
#include "actions.h"
#include "statistics.h"

#include <fcntl.h>
#include <unistd.h>
#include <limits.h>

#ifdef TESTS
 #include "test_stereo_vertex.h"
#endif

int main(int argc, char *argv[])
{
 ansi_colors = 1;
 print_errors_to_stderr = 0;
  
 logs_Write(0, "\n\nDIAGRAMMATIC MONTE-CARLO FOR SU(N) SIGMA MODEL IN THE PLANAR LIMIT - BASED ON THE STEREOGRAPHIC PROJECTION\n");
 
 init_rand(time(NULL));
 
 parse_command_line_options(argc, argv);
 init_actions();
 init_parameters();
 
 max_recursion_depth = 2*max_order;
 init_metropolis(&(X.top));
 
 t_observable_stat* obs_stat = init_observable_stat();
 
 print_parameters();
 
 logs_Write(0, "Running Metropolis for %i MC steps", number_mc_steps);
 for(int imc=0; imc<number_mc_steps; imc++)
 {
  metropolis_step();
  gather_observable_stat(obs_stat); 
 };
  
 process_mc_stat();
 save_mc_stat();
 
 process_observable_stat(obs_stat);
 
 free_observable_stat(obs_stat);
 SAFE_FREE(obs_stat);
 
 free_metropolis();
 free_actions();
 free_lat_propagator(&P);
 
 return 0;
}
