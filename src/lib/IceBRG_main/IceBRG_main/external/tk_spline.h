/**
 * @file tk_spline.h
 *
 * simple cubic spline interpolation library without external
 * dependencies
 *
 * Split into .h and .cpp by Bryan Gillis July 16, 2014
 *
 * ---------------------------------------------------------------------
 * Copyright (C) 2011, 2014 Tino Kluge (ttk448 at gmail.com)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------
 *
 */

// body file: brg/math/interpolator/tk_spline.cpp

#ifndef _tk_spline_h
#define _tk_spline_h

#include <vector>

#include "IceBRG_main/utility.hpp"

namespace tk {

// band matrix solver
class band_matrix {
private:
   std::vector< std::vector<flt_type> > m_upper;  // upper band
   std::vector< std::vector<flt_type> > m_lower;  // lower band
public:
   band_matrix() {};                             // constructor
   band_matrix(int_type dim, int_type n_u, int_type n_l);       // constructor
   ~band_matrix() {};                            // destructor
   void resize(int_type dim, int_type n_u, int_type n_l);      // init with dim,n_u,n_l
   int_type dim() const;                             // matrix dimension
   int_type num_upper() const {
      return m_upper.size()-1;
   }
   int_type num_lower() const {
      return m_lower.size()-1;
   }
   // access operator
   flt_type & operator () (int_type i, int_type j);            // write
   flt_type   operator () (int_type i, int_type j) const;      // read
   // we can store an additional diogonal (in m_lower)
   flt_type& saved_diag(int_type i);
   flt_type  saved_diag(int_type i) const;
   void lu_decompose();
   std::vector<flt_type> r_solve(const std::vector<flt_type>& b) const;
   std::vector<flt_type> l_solve(const std::vector<flt_type>& b) const;
   std::vector<flt_type> lu_solve(const std::vector<flt_type>& b,
                                bool is_lu_decomposed=false);

};


// spline interpolation
class spline {
private:
   std::vector<flt_type> m_x,m_y;           // x,y coordinates of points
   // interpolation parameters
   // f(x) = a*(x-x_i)^3 + b*(x-x_i)^2 + c*(x-x_i) + y_i
   std::vector<flt_type> m_a,m_b,m_c,m_d;
public:
   void set_points(const std::vector<flt_type>& x,
                   const std::vector<flt_type>& y, bool cubic_spline=true);
   flt_type operator() (flt_type x) const;
};

} // namespace tk

#endif /* _tk_spline_h */
