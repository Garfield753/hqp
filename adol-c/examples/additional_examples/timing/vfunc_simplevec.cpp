#define _VFUNC_SIMPLEVEC_C_
/*
   --------------------------------------------------------------
   File vfunc_simplevec.C 
   of ADOL-C version 1.8.0                        as of Dec/01/98
   --------------------------------------------------------------

   Example: function module containing a simple vector example

   Each << function module >> contains:
          
     (1) const char* const controlFileName 
     (2) int indepDim; 
     (3) int depDim; 

     (4) void initProblemParameters( void )
     (5) void initIndependents( double* indeps )
     (6) void originalVectorFunction( double* indeps, double* deps )
     (7) void tapingVectorFunction( int tag, double* indeps, double* deps )   

   Last changes: 
     981201 olvo new headers
     980728 olvo first version

   --------------------------------------------------------------
*/


/****************************************************************************/
/*                                                                 INCLUDES */
#include "../../../adolc/adolc.h"

#include <time.h>
#include <stdlib.h>


/****************************************************************************/
/*                                                         GLOBAL VARIABLES */

/*--------------------------------------------------------------------------*/
/*                                                        Control file name */
const char* controlFileName = "simplevec.ctrl";

/*--------------------------------------------------------------------------*/
/*                                                               Dimensions */
int indepDim;
int depDim;

/*--------------------------------------------------------------------------*/
/*                                       Other problem dependent parameters */
static unsigned short int dx[3]; /* variable needed by erand48(.) */


/****************************************************************************/
/*                                                  INIT PROBLEM PARAMETERS */
void initProblemParameters( void )
{ fprintf(stdout,"A SIMPLE VECTOR FUNCTION (ADOL-C Example)\n\n");

  /* number of indeps & deps */ 
  if (indepDim < 1)
  { fprintf(stdout,"    # of independents = ? ");
    fscanf(stdin,"%d",&indepDim);
    fprintf(stdout,"\n");
  }
  if (depDim < 1)
  { fprintf(stdout,"    # of dependents = ? ");
    fscanf(stdin,"%d",&depDim);
    fprintf(stdout,"\n");
  }

  /* Init erand48(); */
  struct tm s;
  time_t t;
  time(&t); 
  s=*localtime(&t);
  srand(s.tm_sec*s.tm_min); 
  dx[0]=rand();
  dx[1]=rand();
  dx[2]=rand();
}


/****************************************************************************/
/*                                                        INITIALIZE INDEPs */
void initIndependents( double* indeps )
{ for (int i=0; i<indepDim; i++)
     indeps[i] = erand48(dx);
}


/****************************************************************************/
/*                                                 ORIGINAL SCALAR FUNCTION */

/*--------------------------------------------------------------------------*/
/*                                               The simple vector function */
void simplevec( int n, int m, double* indeps, double* deps )
{ int i, j;
  double temp = 1.0;
  for (j=0; j<m; j++)
  { deps[j] = temp;
    for (i=0; i<n; i++)
      deps[j] *= indeps[i];
    temp = deps[j];
  }
}

/*--------------------------------------------------------------------------*/
/*                                                   The interface function */
void originalVectorFunction( double* indeps, double* deps )
{ simplevec(indepDim,depDim,indeps,deps);
}


/****************************************************************************/
/*                                                   TAPING SCALAR FUNCTION */

/*--------------------------------------------------------------------------*/
/*                                       The simple adouble vector function */
void activeSimplevec( int n, int m, adouble* indeps, adouble* deps )
{ int i, j;
  adouble temp = 1.0;
  for (j=0; j<m; j++)
  { deps[j] = temp;
    for (i=0; i<n; i++)
      deps[j] *= indeps[i];
    temp = deps[j];
  }
}

/*--------------------------------------------------------------------------*/
/*                                                   The interface function */
void tapingVectorFunction( int tag, double* indeps, double* deps )
{ int i;
  trace_on(tag);
  adouble* activeIndeps = new adouble[indepDim];
  adouble* activeDeps   = new adouble[depDim];
  adouble* aIP = activeIndeps;
  double*  iP  = indeps; 
  for (i=0; i<indepDim; i++)
     *aIP++ <<= *iP++; 
  activeSimplevec(indepDim,depDim,activeIndeps,activeDeps);
  aIP = activeDeps;
  iP  = deps; 
  for (i=0; i<depDim; i++)
    *aIP++ >>= *iP++; 
  trace_off();
}


#undef _VFUNC_SIMPLEVEC_C_





