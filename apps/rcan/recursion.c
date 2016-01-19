#include "recursion.h"

static double*  G[MAX_M][MAX_M];    //Storage for the expansion coefficients

static double   D0[MAXLS];          //LS-sized bare kinetic term
static double   G0[MAXLS];          //LS-sized bare propagator Values of the free propagator
static double   G0pq[MAXLS2];       //LS*LS-sized array, for the contact terms in the action

#ifdef FIX2
#define ils    (0.5)
#define LS2    (4)
#define twoLS  (4)
#define mLS    (2)
static const uint     LSn[MAX_MX2] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 1073741824, 0};
static const uint     LS2n[MAX_M]  = {1, 4, 16, 64, 256, 1024, 4096, 16384, 65536, 262144, 1048576, 4194304, 16777216, 67108864, 268435456, 1073741824};
static const uint     LS2n1[MAX_M] = {0, 2, 8, 32, 128, 512, 2048, 8192, 32768, 131072, 524288, 2097152, 8388608, 33554432, 134217728, 536870912};
static const double   iLSn[MAX_M]  = {1.0, 0.5, 0.25, 0.125, 0.0625, 0.03125, 0.015625, 0.0078125, 0.00390625, 0.001953125, 0.0009765625, 0.00048828125, 0.000244140625, 0.0001220703125, 0.00006103515625, 0.000030517578125};
#else
static double   ils          = 0.0;       //Inverse LS
static uint     LS2          = 0;         //LS squared
static uint     twoLS        = 0;         //two times LS
static uint     mLS          = 0;         //Local static copy of LS
static uint     LSn[MAX_MX2];
static uint     LS2n[MAX_M];
static uint     LS2n1[MAX_M];
static double   iLSn[MAX_M];
#endif

void init_lattice_constants()
{
 if(mmax>=MAX_M)
 { 
  mmax = MAX_M;
  logs_WriteWarning("The input value of mmax is too large, reset to %i", MAX_M);
 };       
 
#ifndef FIX2
 mLS   = LS;
 twoLS = 2*LS;   
 ils   = 1.0/(double)LS;
 LS2   = LS*LS;
 
 LSn[0] = 1;
 for(int i=1; i<MAX_MX2; i++)
  LSn[i] = LSn[i-1]*LS;
    
 LS2n[0] = 1;
 iLSn[0] = 1.0;
 for(int i=1; i<MAX_M; i++)
 {
   LS2n[i] = LS2n[i-1]*LS2; 
  iLSn[i]  = iLSn[i-1]/(double)LS;
 };
 LS2n1[0] = 0;
 LS2n1[1] = LS;
 for(int i=2; i<MAX_M; i++)
  LS2n1[i] = LS2n1[i-1]*LS2;
#endif
}

void init_kinematics(double bare_mass)
{
 //Initializing the bare propagator and the bare momentum term
 for(int p=0; p<LS; p++)
 {
  D0[p] = meff_sq + 4.0*sin(M_PI*(double)p/(double)LS);
  G0[p] = 1.0/(bare_mass + D0[p]);
  for(int q=0; q<LS; q++)
   G0pq[LS*q + p] = ((p+q)%LS==0? G0[p]/(double)LS : 0.0);
 };
}

void init_recursion_unpacked()
{
 init_lattice_constants();
 //For cleanness, setting all address elements to NULL
 for(int n=0; n<MAX_M; n++)
  for(int m=0; m<MAX_M; m++)
   G[n][m] = NULL;

 //Allocating dynamic memory to contain the pre-calculated propagators    
 uint memory_allocated = 0;
 for(int m=mmin_prc; m<=mmax; m++)
 {
  uint psp_dim = LS2;
  for(int n=1; n<=mmax-m+1; n++) 
  {
   G[m][n] = (double *)malloc(psp_dim*sizeof(double));
   if(G[m][n]!=NULL)
   {
    memory_allocated += sizeof(double)*psp_dim;
    psp_dim *= LS2; 
   }
   else
    logs_WriteErrorAndTerminate("\nUnable to allocate %u bytes of memory at n = %i, m = %i. Exiting.", psp_dim*sizeof(double), n, m); 
  };
 };
 
 logs_WriteParameter(0, "Allocated buffer memory", "%2.4lf Mb", (double)memory_allocated/(double)(1024*1024));
}

void test_recursion()
{
 double max_err_momentum = 0.0;
 double max_err_cyclic   = 0.0;
 
 for(int m=0; m<=mmax; m++)
  for(int n=1; n<=mmax-m+1; n++)
  {
   logs_Write(0, "Checking the m = %02i, n = %02i terms...", m, n);
   for(uint P=0; P<LS2n[n]; P++)
    if(total_momentum(P, n)!=0)
    {
     max_err_momentum = MAX(max_err_momentum, fabs(G[m][n][P]));
     if(fabs(G[m][n][P])>1.0E-8)
     {
      char* mstr = NULL;
      get_momentum_str(P, n, &mstr, ", ");
      logs_WriteError("Momentum non-conservation at m = %02i, n = %02i, P = %u > [%s]", m, n, P, mstr);
     };
    }
    else
    {
     //Testing the full cyclic symmetry
     uint sP = P;
     for(int i=0; i<2*n; i++)
     {
      sP = sP/LS + LSn[2*n-1]*(sP%LS);
      max_err_cyclic = MAX(max_err_cyclic, fabs(G[m][n][P]-G[m][n][sP]));
      if(fabs(G[m][n][P]-G[m][n][sP])>1.0E-8)
      {
       char* mstr1 = NULL; char* mstr2 = NULL;
       get_momentum_str( P, n, &mstr1, ", ");
       get_momentum_str(sP, n, &mstr2, ", ");
       logs_WriteError("Cyclic symmetry violated at m = %02i, n = %02i, P=[%s], sP=[%s]", m, n, mstr1, mstr2);
       system("PAUSE");
      };
     };
    };  
  };
 logs_Write(0, "");
 logs_WriteParameter(0, "Max. err for non-conserved momenta",     "%2.4E", max_err_momentum);
 logs_WriteParameter(0, "Max. err for the full cyclic symmetry",  "%2.4E", max_err_cyclic);
}

void free_recursion()
{
 for(int m=0; m<=mmax; m++)
  for(int n=1; n<=mmax-m+1; n++) 
   SAFE_FREE(G[n][m]);
}

void printf_momentum_str(uint P, int n, char* prefix, char* separator, char* suffix)
{
 uint mP = P;
 fprintf(stdout, "%s", prefix);
 for(int i=0; i<2*n-1; i++)
 {
  fprintf(stdout, "%1u%s", mP%LS, separator);
  mP /= LS;
 };
 fprintf(stdout, "%1u%s\n", mP%LS, suffix);
 fflush(stdout);
}

void get_momentum_str(uint P, int n, char** s, char* separator)
{
 uint mP = P;
 for(int i=0; i<2*n-1; i++)
 {
  sprintf_append(s, "%1u%s", mP%LS, separator);
  mP /= LS;
 };
 sprintf_append(s, "%1u", mP%LS);
}

uint total_momentum(uint P, int n)
{
 uint mP = P, res = 0;
 for(int i=0; i<2*n; i++)
 {
  res += mP%LS;
  mP /= LS;
 };
 return (res%LS);
}

void unpack_momenta(uint P, int n, uint* ps)
{
 uint mP = P;
 for(int i=0; i<2*n; i++)
 {
  ps[i] = mP%LS;
  mP /= LS;
 };
}

uint pack_momenta(int n, uint* ps)
{
 uint P = 0, F = 1;
 for(int i=0; i<2*n; i++)
 {
  P += ps[i]*F;
  F *= LS;
 };
 return P;
}

//TODO: packed/unpacked as a flag, probably at compilation time?

void generalized_sc()
{
 for(int m=0; m<=mmax; m++)
 {
  for(int n=1; n<=mmax-m+1; n++)
  {
   logs_Write(0, "Generating the m = %02i, n = %02i terms...", m, n);
   for(uint P=0; P<LS2n[n]; P++)
   {
    if(P%65536==0) logs_WriteStatus(1, P, LS2n[n], 1024, "");
    
    uint p1 = P%mLS;
    //First contribution - contact terms
    double c1 = 0.0;
    if(n>1)
    {
     for(int A=2; A<n; A++)
     {
      uint qA = (P/LS2n1[A])%mLS;
      if((p1+qA)%mLS==0)
      {
       uint P1 = mLS*((P/mLS)%LS2n1[A-1]) + (P/LS2n[A-1])%mLS;
       uint P2 = P/LS2n[A];
       for(int m1=0; m1<=m; m1++)
        c1 += G[m1][A-1][P1]*G[m-m1][n-A][P2];
      }; 
     };
     c1 *= ils*G0[p1];
     c1 += G0pq[P%LS2]*G[m][n-1][P/LS2];
     uint P1 = mLS*((P/mLS)%LS2n1[n-1]) + (P/LS2n[n-1])%mLS;
     c1 += G0pq[(P/LS2n1[n])*mLS + p1]*G[m][n-1][P1];
    }
    else
    {
     if(m==0)
      c1 += G0pq[P];   
    }; //End of if(n>1)
    double c2 = 0.0; double c3 = 0.0;
    if(m>0)
    {
     //Second contribution - double-trace vertex
     for(int A=2; A<=n; A++)
     {
      uint P1 = mLS*((P/mLS)%LS2n1[A-1]); //This is now the sequence 0, q_1, ..., p_{A-1}, q_{A-1} 
      uint P2 = mLS*(P/LS2n1[A]);       //This is now the sequence 0, q_A, ..., p_n, q_n
      uint pA = (P/LS2n[A-1])%mLS;
      for(uint p1t=0; p1t<mLS; p1t++)
      {
       uint pAt = (p1 + pA + (mLS - p1t))%mLS;
       for(int m1=0; m1<m; m1++)
        c2 += G[m1][A-1][p1t + P1]*G[m-m1-1][n-A+1][pAt + P2];
      }; 
     };
     c2 *= ils*G0[p1];
     //Third contribution - scalar vertex
     uint P1 = (P/mLS)*LS2n1[2];
     for(uint p1t=0; p1t<mLS; p1t++)
      for(uint q1t=0; q1t<mLS; q1t++)
      {
       uint p2t = (p1 + (mLS - p1t) + (mLS - q1t))%mLS;
       c3 += D0[q1t]*G[m-1][n+1][p1t + mLS*q1t + LS2*p2t + P1];
      };
     c3 *= G0[p1]; 
    }; //End of if(m>0) 
    //Summing up all the contributions
    G[m][n][P] = c1 - c2 + c3; //-c2 is important!!! It is the origin of the sign problem!!!
   }; //End of loop over P      
  }; //End of loop over n    
 }; //End of loop over m    
}
