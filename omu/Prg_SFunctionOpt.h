/**
 * @file Prg_SFunctionOpt.h
 *    Optimal control problem for a model given as MEX S-function.
 *
 * rf, 7/25/00
 *
 */

/*
    Copyright (C) 1997--2002  Ruediger Franke

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; 
    version 2 of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library (file COPYING.LIB);
    if not, write to the Free Software Foundation, Inc.,
    59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef Prg_SFunctionOpt_H
#define Prg_SFunctionOpt_H

#include "Prg_SFunction.h"
#include "Omu_Variables.h"

/** Extend Omu_VariableVec with attributes for optimization criterion. */
class Omu_OptVarVec: public Omu_VariableVec {
public:
  VECP 	weight1;	///< weight for linear objective term (default: 0.0)
  VECP 	weight2;	///< weight for quadratic objective term (default: 0.0)
  VECP 	ref; 		///< reference value for quadratic term (default: 0.0)
  IVECP active; 	///< indicate used variables (default: 0 -- not used)

  Omu_OptVarVec(); 		///< allocate empty vectors
  virtual ~Omu_OptVarVec(); 	///< destroy vectors
  virtual void resize(int n); 	///< resize and initialize vectors
};

/**
   Optimal control problem for a model given as MEX S-function.
   The optimization time horizon @f$[t_0,t_f]@f$ is split into @f$k=0,...,K@f$ 
   stages with time points @f$t_0=t^0<t^1<\ldots<t^K=t_f@f$. Each stage may 
   be further subdivided. This leads to @f$KK=sps\,K@f$ sample periods, 
   where sps is the number of samples periods per stage, and with the sample
   time points @f$t^{kk}, kk=0,...,KK@f$. Additional sample time points within
   a stage are for instance useful for better treating path constraints.

   In the following all vector operations are defined element wise.
   The treated optimization problem reads
   @f[
   \begin{array}{l}
    \displaystyle{}
    J\ =\ \sum_{kk=0}^{KK} \Delta t^{kk} \sum_{i=1}^{n_u} \left\{
      u_{weight1}\,u(t^{kk})
      \ +\ u_{weight2}\left[u(t^{kk})-\frac{u_{ref}}{u_{nominal}}\right]^2
      \right\}_i
    \\[4ex] \displaystyle \qquad
    \ + \ \sum_{kk=0}^{KK-1} \Delta t^{kk} \sum_{i=1}^{n_u} \left\{
      {der\_u}_{weight1}\,\dot{u}(t^{kk})
      \ +\ {der\_u}_{weight2}\left[\dot{u}(t^{kk})
                                -\frac{{der\_u}_{ref}}{u_{nominal}}\right]^2
      \right\}_i
    \\[4ex] \displaystyle \qquad
    \ + \ \sum_{kk=0}^{KK} \Delta t^{kk} \sum_{i=1}^{n_y} \left\{
      y_{weight1}\,y(t^{kk})
      \ +\ y_{weight2}\left[y(t^{kk})-\frac{y_{ref}}{y_{nominal}}\right]^2
    \right\}_i
    \\[4ex] \displaystyle \qquad
    \ + \ \sum_{kk=0}^{KK} \Delta t^{kk} \sum_{i=1}^{n_y} \left\{
         y_{soft\_weight1}\,s^{kk} + y_{soft\_weight2}\,s^{kk}s^{kk}
    \right\}_i
    \\[4ex] \displaystyle \qquad
    \ + \ \sum_{i=1}^{n_y} \left\{
      y_{f\_weight1}\,y(t_f)
      \ +\ y_{f\_weight2}\left[y(t_f)-\frac{y_{ref}}{y_{nominal}}\right]^2
    \right\}_i
    \quad\to\quad \min
   \end{array}
   @f]
   with
   @f[
    \Delta t^{kk} = \frac{1}{2}\left\{\begin{array}{ll}
      t^{kk+1} - t^{kk}, & kk=0, \\
      t^{kk} - t^{kk-1}, & kk=KK, \\
      t^{kk+1} - t^{kk-1}, & \mbox{else},
    \end{array}\right.
   @f]
   subject to the model given with the S-function methods
   mdlDerivatives @f$f@f$ and mdlOutputs @f$g,\ t\in[t_0,t_f]@f$
   @f[
   \begin{array}{l}
    \displaystyle \dot{x}(t) = 
     \frac{f[x_{nominal}\,x(t),\ u_{nominal}\,u(t)]}{x_{nominal}}, \\[3ex]
    \displaystyle y(t) = 
     \frac{g[x_{nominal}\,x(t),\ u_{nominal}\,u(t)]}{y_{nominal}}
      \ +\ \frac{y_{bias}}{y_{nominal}},
   \end{array}
   @f]
   with piecewise linear approximation of @f$u(t)@f$ 
   either using optimized control parameters @f$du^k@f$ or given inputs 
   @f$us@f$ 
   @f[
   \begin{array}{ll}
    \left\{ \dot{u}(t) = du^{k}
    \right\}_i, & i \in \mbox{find}(u_{active}), \\[1ex]
    & t\in[t^{k},t^{k+1}),\ k=0,\ldots,K-1, \\[3ex]
    \left\{ u(t) = \displaystyle 
        \frac{t^{kk+1}-t}{t^{kk+1}-t^{kk}}\ \frac{us^{kk}}{u_{nominal}}
      + \frac{t-t^{kk}}{t^{kk+1}-t^{kk}} \ \frac{us^{kk+1}}{u_{nominal}}
    \right\}_i, & i \notin \mbox{find}(u_{active}), \\[1ex]
    & t\in[t^{kk},t^{kk+1}),\ kk=0,\ldots,KK-1,
   \end{array}
   @f]
   and subject to the constraints
   @f[
   \begin{array}{rcccll}
    \displaystyle &&x(t^0) &=& \displaystyle \frac{x^0}{x_{nominal}}, \\[3ex]
    \displaystyle &&u(t^0) &=& \displaystyle \frac{us^0}{u_{nominal}}, \quad &
        \mbox{if}\ \ nus_{fixed}>0, \\[3ex]
    \displaystyle \frac{u_{min}}{u_{nominal}} &<& u(t^{k})
        &<& \displaystyle \frac{u_{max}}{u_{nominal}}, \quad &
        k=0,\ldots,K \ \ \mbox{and}\ \ sps\,k \ge nus_{fixed}, \\[3ex]
    \displaystyle &&du^k &=& \displaystyle du^k_{initial}, \quad &
        k = 0,\ldots,K-1 \ \ \mbox{and}\ \ sps\,k < nus_{fixed}-1, \\[3ex]
    \displaystyle \frac{{der\_u}_{min}}{u_{nominal}} &<& du^{k}
        &<& \displaystyle \frac{{der\_u}_{max}}{u_{nominal}}, \quad &
        k=0,\ldots,K-1 \ \ \mbox{and}\ \ sps\,k \ge nus_{fixed}-1, \\[3ex]
    \displaystyle \frac{y_{min}}{y_{nominal}} &<& y(t^{kk})
        &<& \displaystyle \displaystyle \frac{y_{max}}{y_{nominal}}, \quad &
        kk=0,\ldots,KK, \\[3ex]
    \displaystyle \frac{y_{soft\_min}}{y_{nominal}} - s^{kk} &<& y(t^{kk})
        &<& \displaystyle \frac{y_{soft\_max}}{y_{nominal}} + s^{kk}, \\[3ex]
    \displaystyle && s^{kk} &>& 0, \quad & kk=0,\ldots,KK, \\[3ex]
    \displaystyle \frac{y_{f\_min}}{y_{nominal}} &<& y(t_f)
        &<& \displaystyle \displaystyle \frac{y_{f\_max}}{y_{nominal}}.
   \end{array}
   @f]
   The initial guess is taken from given initial states and model inputs
   @f[
   \begin{array}{rcll}
    x_{initial}(t^0) &=& \displaystyle \frac{x^0}{x_{nominal}}, \\[3ex]
    u_{initial}(t^0) &=& \displaystyle \frac{us^0}{u_{nominal}}, \\[3ex]
    du^k_{initial} &=& \displaystyle 
     \left\{\frac{us^{sps\,(k+1)} - us^{sps\,k}}
                 {(t^{sps\,(k+1)}-t^{sps\,k})\,u_{nominal}}
     \right\}_i, & i \in \mbox{find}(u_{active}), \\[1ex]
               &&& k=0,\ldots,K-1.
   \end{array}
   @f]

   The problem is treated as multistage problem with K stages per default. 
   Consequently additional K junction conditions (equality constraints)
   are introduced for the state variables x and the piecewise linear
   approximated control trajectories u. Alternatively the problem can
   be treated without stages applying pure control vector parameterization
   and hiding model states from the optimizer.

   Model inputs and outputs can be accessed through
   @f[
   \begin{array}{l}
    us^{kk} = u_{nominal}\,u(t^{kk}), \quad kk=1,\ldots,KK, \\[1ex]
    ys^{kk} = y_{nominal}\,y(t^{kk}), \quad kk=0,\ldots,KK.
   \end{array}
   @f]
 */
class Prg_SFunctionOpt: public Prg_SFunction {

 protected:
  Omu_OptVarVec _mdl_u; 	///< model inputs
  Omu_OptVarVec _mdl_der_u; 	///< rates of change of inputs
  Omu_OptVarVec _mdl_y; 	///< model outputs
  Omu_OptVarVec _mdl_y_soft; 	///< attributes for relaxed output constraints
  Omu_OptVarVec _mdl_yf; 	///< model outputs at final time

  double 	_t_nominal; 	///< nominal time (used internally for scaling)
  VECP 		_mdl_u_nominal;	///< nominal inputs (for scaling)
  VECP 		_mdl_x_nominal;	///< nominal states (for scaling)
  VECP 		_mdl_y_nominal;	///< nominal outputs (for scaling)

  VECP 		_mdl_y_bias;	///< bias correction (offset) for outputs 

  int		_nx;	///< number of states for optimizer
  int		_nu;	///< number of optimized control inputs
  int		_nc;	///< number of constrained outputs
  int		_ncf;	///< number of constrained outputs at final time
  int		_ns;	///< number of slack variables for soft constraints
  int		_nsc;	///< number of soft constraints
  bool 		_multistage; 	///< treat as multistage problem

  /**
   * Number of sample periods per stage (default: 1).
   * The value can be increased to devide each control interval into
   * multiple sample periods, e.g. for evaluating constraints and
   * the objective within control intervals. Currently _sps>1 is only
   * supported for multistage problems.
   */
  int 		_sps;

  MATP	_mdl_us;	///< given model inputs (controls and disturbances)
  MATP	_mdl_ys;	///< calculated model outputs

  /**
   * Number of fixed control inputs at begin of time horizon (default: 0).
   * The initial value is fixed for _nus_fixed=1, the initial and the 
   * second value are fixed for _nus_fixed=2, and so on).
   */
  IVECP 	_nus_fixed;

  /**
   * @name Implementation of predefined methods.
   * @see Omu_Program
   */
  //@{
  void setup_stages(IVECP ks, VECP ts);

  void setup(int k,
	     Omu_VariableVec &x, Omu_VariableVec &u, Omu_VariableVec &c);

  void setup_struct(int k,
		    const Omu_VariableVec &x, const Omu_VariableVec &u,
		    Omu_DependentVec &xt, Omu_DependentVec &F,
		    Omu_DependentVec &f,
		    Omu_Dependent &f0, Omu_DependentVec &c);

  void init_simulation(int k,
		       Omu_VariableVec &x, Omu_VariableVec &u);

  void update(int kk,
	      const Omu_StateVec &x, const Omu_Vec &u,
	      const Omu_StateVec &xf,
	      Omu_DependentVec &f, Omu_Dependent &f0,
	      Omu_DependentVec &c);

  void consistic(int kk, double t,
		 const Omu_StateVec &x, const Omu_Vec &u,
		 Omu_DependentVec &xt);

  void continuous(int kk, double t,
		  const Omu_StateVec &x, const Omu_Vec &u,
		  const Omu_StateVec &xp, Omu_DependentVec &F);
  //@}

  /**
   * @name Overloaded gradient routines
   * These routines call the S-function method mdlJacobian if available;
   * otherwise they direct calls to the according methods by Omu_Program.
   */
  //@{

  /**
   * Overloaded update routine for obtaining gradients.
   */
  void update_grds(int kk, 
		   const Omu_StateVec &x, const Omu_Vec &u,
		   const Omu_StateVec &xf,
		   Omu_DependentVec &f, Omu_Dependent &f0,
		   Omu_DependentVec  &c);

  /**
   * Overloaded continuous routine for obtaining gradients.
   */
  void continuous_grds(int kk, double t,
		       const Omu_StateVec &x, const Omu_Vec &u,
		       const Omu_StateVec &xp, Omu_DependentVec &F);
  //@}

 public:

  Prg_SFunctionOpt();		///< constructor
  ~Prg_SFunctionOpt();		///< destructor

  char *name() {return "SFunctionOpt";} ///< name SFunctionOpt
};  

#endif

