/**
 * @file Omu_Dependents.h 
 *   Dependent variables (single and vector, including Jacobians).
 *
 * rf, 7/31/01
 */

/*
    Copyright (C) 1997--2017  Ruediger Franke

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

#ifndef Omu_Dependents_H
#define Omu_Dependents_H

#include <Meschach.h>
#include "Omu_Vec.h"

/** Vector extended with structural information for a gradient. */
class OMU_API Omu_Gradient: public Mesch::VECP {
  friend class Omu_Dep;
public:
  /** Create an empty matrix. */
  Omu_Gradient();

  /** Free memory. */
  ~Omu_Gradient();

  // access methods for properties
  bool is_zero() {return _is_zero;}
  bool is_constant() {return _is_constant;}

protected:
  /** Allocate and initialize Gradient. */
  void size(int dim);

  /** Obtain properties for current matrix. */
  void analyze_struct(bool is_constant);

  bool _is_zero;
  bool _is_constant;
};


/** Single dependent variable. */
class OMU_API Omu_Dependent {
public:
  /** @name flags for indicating dependencies from independent variables */
  //@{
  static const int WRT_x; 	///< dependent with respect to x
  static const int WRT_u; 	///< dependent with respect to u
  static const int WRT_dx; 	///< dependent with respect to dx
  static const int WRT_xf; 	///< dependent with respect to xf
  static const int WRT_q; 	///< dependent with respect to sens pars q
  static const int WRT_ALL; 	///< dependent with respect to all variables
  //@}

  Omu_Gradient gx;	///< gradient wrt x (ie initial states of period)
  Omu_Gradient gu;	///< gradient wrt u (ie control parameters of stage)
  Omu_Gradient gxf;	///< gradient wrt xf (ie final states of period)

  Omu_Dependent();
  virtual ~Omu_Dependent();

  /** Assign a new value */
  const Omu_Dependent &operator = (const HQPReal &value) {
    _value = value;
    return *this;
  }

  /** Convert to ordinary Real */
  operator HQPReal&() {
    return _value;
  }

  /** Mark a linear dependency */
  virtual void set_linear(int wrt = Omu_Dependent::WRT_ALL,
			  bool value = true) = 0;
  /** Query if a dependency is linear */
  virtual bool is_linear(int wrt = Omu_Dependent::WRT_ALL) const = 0;

  /** Set flag indicating that gradients are required */
  void set_required_g(bool value = true) {
    _required_g = value;
  }
  /** Query if gradients are required when evaluating the dependent */
  bool is_required_g() const {
    return _required_g;
  }

protected:
  HQPReal _value;
  bool _required_g;
  // dismiss copy constructor and assignment operator
  Omu_Dependent(const Omu_Dependent &) {}
  Omu_Dependent &operator=(const Omu_Dependent &) {return *this;}
};


/** Matrix extended with structural information for a Jacobian. */
class OMU_API Omu_Jacobian: public Mesch::MATP {
  friend class Omu_DepVec;
public:
  /** Create an empty matrix. */
  Omu_Jacobian();

  /** Free memory. */
  ~Omu_Jacobian();

  /** @name Query properties of Jacobian matrix.
      The properties are checked with analyze_struct(). */
  //@{
  /** zero matrix */
  bool is_zero() const {return _is_zero;}

  /** diagonal matrix */
  bool is_diagonal() const {
    return _sbw_lower < 1 && _sbw_upper < 1;
  }

  /** identity matrix */
  bool is_ident() const {return _is_ident;}

  /** constant scalar matrix (is_constant and is_diagonal and
      J[i][i]==J[0][0], i = 1,...,min(nrows,ncols)-1) */
  bool is_scalar_constant() const {return _is_scalar_constant;}

  /** constant matrix */
  bool is_constant() const {return _is_constant;}

  /** semi-bandwidth, i.e. number of side diagonals */
  int sbw() const {return hqp_max(_sbw_lower, _sbw_upper);}

  /** lower semi-bandwidth */
  int sbw_lower() const {return _sbw_lower;}

  /** upper semi-bandwidth */
  int sbw_upper() const {return _sbw_upper;}

  /** row i is zero */
  bool is_zero_row(int i) const {return _zero_rows[i] != 0;}

  /** column j is zero */
  bool is_zero_column(int j) const {return _zero_cols[j] != 0;}
  //@}

protected:
  /** Allocate and initialize Jacobian. */
  void size(int nrows, int ncols);

  /** Resize dimension without reinitializing memory.
      Argument nrows must not be larger than allocated nrows. */
  void adapt_size(int nrows);

  /** Obtain properties for current matrix. */
  void analyze_struct(bool is_constant);

  bool _is_zero;
  bool _is_ident;
  bool _is_scalar_constant;
  bool _is_constant;
  int _sbw_lower;
  int _sbw_upper;
  IVECP _zero_rows;
  IVECP _zero_cols;
};


/** Vector of dependent variables. */
class OMU_API Omu_DependentVec: public Omu_Vec {
public:
  Omu_Jacobian Jx;	///< Jacobian wrt x (initial states of sample period)
  Omu_Jacobian Ju;	///< Jacobian wrt u (control parameters of stage)
  Omu_Jacobian Jdx;	///< Jacobian wrt dx (time derivative of x)
  Omu_Jacobian Jxf;	///< Jacobian wrt xf (final states of sample period)
  Omu_Jacobian Jq;	///< Jacobian wrt q (sensitivity parameters)

  VECP lambda;		///< Lagrange multiplier of associated constraint

  Omu_DependentVec();

  /** Mark linear dependency */
  virtual void set_linear(int wrt = Omu_Dependent::WRT_ALL,
			  bool value = true) = 0;
  /** Query if a dependency is linear */
  virtual bool is_linear(int wrt = Omu_Dependent::WRT_ALL) const = 0;

  /** Mark linear element of dependent vector */
  virtual void set_linear_element(int i, int wrt = Omu_Dependent::WRT_ALL,
				  bool value = true) = 0;
  /** Query if an element of the dependent vector is linear */
  virtual bool is_linear_element(int i,
				 int wrt = Omu_Dependent::WRT_ALL) const = 0;

  /** Mark a variable that appears linear in all elements */
  virtual void set_linear_variable(int wrt, int j, bool value = true) = 0;
  /** Query if a variable is linear in all elements */
  virtual bool is_linear_variable(int wrt, int j) const = 0;

  /** Set flag indicating that Jacobians are required */
  void set_required_J(bool value = true) {
    _required_J = value;
  }
  /** Query if Jacobians are required when dependent vector is evaluated */
  bool is_required_J() const {
    return _required_J;
  }

protected:
  bool _required_J;

  // dismiss copy constructor and assignment operator
  Omu_DependentVec(const Omu_DependentVec &): Omu_Vec() {}
  Omu_DependentVec &operator=(const Omu_DependentVec &) {return *this;}
};


#endif
