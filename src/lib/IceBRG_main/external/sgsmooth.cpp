/**********************************************************************\
 @file sgsmooth.cpp
 ------------------

 TODO <Insert file description here>

 **********************************************************************

 Copyright (C) 2014  Bryan R. Gillis

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

\**********************************************************************/

//!
// Sliding window signal processing (and linear algebra toolkit).
//
// supported operations:
// <ul>
// <li> Savitzky-Golay smoothing.
// <li> computing a numerical derivative based of Savitzky-Golay smoothing.
// <li> required linear algebra support for SG smoothing using STL based
//      vector/matrix classes
// </ul>
//
// \brief Linear Algebra "Toolkit".
//

// system headers
#include <stdexcept>
#include <stdio.h>
#include <stddef.h>             // for size_t
#include <math.h>               // for fabs
#include <vector>

#include "IceBRG_main/common.h"

#include "IceBRG_main/math/misc_math.hpp"

#include "sgsmooth.h"


namespace IceBRG{ namespace sgsmooth {

//! default convergence
static const flt_type & TINY_FLOAT = 1.0e-300;

//! comfortable array of ints;
typedef std::vector<size_t> uint_vect;

// savitzky golay smoothing.

/*! matrix class.
 *
 * This is a matrix class derived from a vector of float_vects.  Note that
 * the matrix elements indexed [row][column] with indices starting at 0 (c
 * style). Also note that because of its design looping through rows should
 * be faster than looping through columns.
 *
 * \brief two dimensional floating point array
 */
class float_mat : public std::vector<flt_vector_type> {
private:
    //! disable the default constructor
    explicit float_mat() {};
    //! disable assignment operator until it is implemented.
    float_mat &operator =(const float_mat &) { return *this; };
public:
    //! constructor with sizes
    float_mat(const size_t rows, const size_t cols, const flt_type & def=0.0);
    //! copy constructor for matrix
    float_mat(const float_mat &m);
    //! copy constructor for vector
    float_mat(const flt_vector_type &v);

    //! use default destructor
    // ~float_mat() {};

    //! get size
    size_t nr_rows(void) const { return size(); };
    //! get size
    size_t nr_cols(void) const { return front().size(); };
};



// constructor with sizes
float_mat::float_mat(const size_t rows,const size_t cols,const flt_type & defval)
        : std::vector<flt_vector_type>(rows) {
    size_t i;
    for (i = 0; i < rows; ++i) {
        (*this)[i].resize(cols, defval);
    }
    if ((rows < 1) || (cols < 1)) {
        char buffer[1024];

        sprintf(buffer, "cannot build matrix with %u rows and %u columns\n",
                (int_type)rows, (int_type)cols);
        sgs_error(buffer);
    }
}

// copy constructor for matrix
float_mat::float_mat(const float_mat &m) : std::vector<flt_vector_type>(m.size()) {

    float_mat::iterator inew = begin();
    float_mat::const_iterator iold = m.begin();
    for (/* empty */; iold < m.end(); ++inew, ++iold) {
        const size_t oldsz = iold->size();
        inew->resize(oldsz);
        const flt_vector_type oldvec(*iold);
        *inew = oldvec;
    }
}

// copy constructor for vector
float_mat::float_mat(const flt_vector_type &v)
        : std::vector<flt_vector_type>(1) {

    const size_t oldsz = v.size();
    front().resize(oldsz);
    front() = v;
}

//////////////////////
// Helper functions //
//////////////////////

//! permute() orders the rows of A to match the integers in the index array.
void permute(float_mat &A, uint_vect &idx)
{
	uint_vect i(idx.size());
    size_t j,k;

    for (j = 0; j < A.nr_rows(); ++j) {
        i[j] = j;
    }

    // loop over permuted indices
    for (j = 0; j < A.nr_rows(); ++j) {
        if (i[j] != idx[j]) {

            // search only the remaining indices
            for (k = j+1; k < A.nr_rows(); ++k) {
                if (i[k] ==idx[j]) {
                    std::swap(A[j],A[k]); // swap the rows and
                    i[k] = i[j];     // the elements of
                    i[j] = idx[j];   // the ordered index.
                    break; // next j
                }
            }
        }
    }
}

/*! \brief Implicit partial pivoting.
 *
 * The function looks for pivot element only in rows below the current
 * element, A[idx[row]][column], then swaps that row with the current one in
 * the index map. The algorithm is for implicit pivoting (i.e., the pivot is
 * chosen as if the max coefficient in each row is set to 1) based on the
 * scaling information in the vector scale. The map of swapped indices is
 * recorded in swp. The return value is +1 or -1 depending on whether the
 * number of row swaps was even or odd respectively. */
static int_type partial_pivot(float_mat &A, const size_t row, const size_t col,
                         flt_vector_type &scale, uint_vect &idx, flt_type tol)
{
    if (tol <= 0.0)
        tol = TINY_FLOAT;

    int_type swapNum = 1;

    // default pivot is the current position, [row,col]
    size_t pivot = row;
    flt_type piv_elem = fabs(A[idx[row]][col]) * scale[idx[row]];

    // loop over possible pivots below current
    size_t j;
    for (j = row + 1; j < A.nr_rows(); ++j) {

        const flt_type & tmp = fabs(A[idx[j]][col]) * scale[idx[j]];

        // if this elem is larger, then it becomes the pivot
        if (tmp > piv_elem) {
            pivot = j;
            piv_elem = tmp;
        }
    }

#if 0
    if(piv_elem < tol) {
        sgs_error("partial_pivot(): Zero pivot encountered.\n")
#endif

    if(pivot > row) {           // bring the pivot to the diagonal
        j = idx[row];           // reorder swap array
        idx[row] = idx[pivot];
        idx[pivot] = j;
        swapNum = -swapNum;     // keeping track of odd or even swap
    }
    return swapNum;
}

/*! \brief Perform backward substitution.
 *
 * Solves the system of equations A*b=a, ASSUMING that A is upper
 * triangular. If diag==1, then the diagonal elements are additionally
 * assumed to be 1.  Note that the lower triangular elements are never
 * checked, so this function is valid to use after a LU-decomposition in
 * place.  A is not modified, and the solution, b, is returned in a. */
static void lu_backsubst(float_mat &A, float_mat &a, bool diag=false)
{
	size_t k;
	long_int_type c,r;

    for (r = (A.nr_rows() - 1); r >= 0; --r) {
        for (c = (A.nr_cols() - 1); c > r; --c) {
            for (k = 0; k < A.nr_cols(); ++k) {
                a[r][k] -= A[r][c] * a[c][k];
            }
        }
        if(!diag) {
            for (k = 0; k < A.nr_cols(); ++k) {
                a[r][k] /= A[r][r];
            }
        }
    }
}

/*! \brief Perform forward substitution.
 *
 * Solves the system of equations A*b=a, ASSUMING that A is lower
 * triangular. If diag==1, then the diagonal elements are additionally
 * assumed to be 1.  Note that the upper triangular elements are never
 * checked, so this function is valid to use after a LU-decomposition in
 * place.  A is not modified, and the solution, b, is returned in a. */
static void lu_forwsubst(float_mat &A, float_mat &a, bool diag=true)
{
	size_t r,k,c;
    for (r = 0;r < A.nr_rows(); ++r) {
        for(c = 0; c < r; ++c) {
            for (k = 0; k < A.nr_cols(); ++k) {
                a[r][k] -= A[r][c] * a[c][k];
            }
        }
        if(!diag) {
            for (k = 0; k < A.nr_cols(); ++k) {
                a[r][k] /= A[r][r];
            }
        }
    }
}

/*! \brief Performs LU factorization in place.
 *
 * This is Crout's algorithm (cf., Num. Rec. in C, Section 2.3).  The map of
 * swapped indeces is recorded in idx. The return value is +1 or -1
 * depending on whether the number of row swaps was even or odd
 * respectively.  idx must be preinitialized to a valid set of indices
 * (e.g., {1,2, ... ,A.nr_rows()}). */
static int_type lu_factorize(float_mat &A, uint_vect &idx, flt_type tol=TINY_FLOAT)
{
    if ( tol <= 0.0)
        tol = TINY_FLOAT;

    if ((A.nr_rows() == 0) || (A.nr_rows() != A.nr_cols())) {
        sgs_error("lu_factorize(): cannot handle empty "
                  "or nonsquare matrices.\n");

        return 0;
    }

    flt_vector_type scale(A.nr_rows());  // implicit pivot scaling
    size_t i,j;
    for (i = 0; i < A.nr_rows(); ++i) {
        flt_type maxval = 0.0;
        for (j = 0; j < A.nr_cols(); ++j) {
            if (fabs(A[i][j]) > maxval)
                maxval = fabs(A[i][j]);
        }
        if (maxval == 0.0) {
            sgs_error("lu_factorize(): zero pivot found.\n");
            return 0;
        }
        scale[i] = 1.0 / maxval;
    }

    int_type swapNum = 1;
    size_t c,r;
    for (c = 0; c < A.nr_cols() ; ++c) {            // loop over columns
        swapNum *= partial_pivot(A, c, c, scale, idx, tol); // bring pivot to diagonal
        for(r = 0; r < A.nr_rows(); ++r) {      //  loop over rows
            size_t lim = (r < c) ? r : c;
            for (j = 0; j < lim; ++j) {
                A[idx[r]][c] -= A[idx[r]][j] * A[idx[j]][c];
            }
            if (r > c)
                A[idx[r]][c] /= A[idx[c]][c];
        }
    }
    permute(A,idx);
    return swapNum;
}

/*! \brief Solve a system of linear equations.
 * Solves the inhomogeneous matrix problem with lu-decomposition. Note that
 * inversion may be accomplished by setting a to the identity_matrix. */
static float_mat lin_solve(const float_mat &A, const float_mat &a,
                           flt_type tol=TINY_FLOAT)
{
    float_mat B(A);
    float_mat b(a);
    uint_vect idx(B.nr_rows());
    size_t j;

    for (j = 0; j < B.nr_rows(); ++j) {
        idx[j] = j;  // init row swap label array
    }
    lu_factorize(B,idx,tol); // get the lu-decomp.
    permute(b,idx);          // sort the inhomogeneity to match the lu-decomp
    lu_forwsubst(B,b);       // solve the forward problem
    lu_backsubst(B,b);       // solve the backward problem
    return b;
}

///////////////////////
// related functions //
///////////////////////

//! Returns the inverse of a matrix using LU-decomposition.
static float_mat invert(const float_mat &A)
{
    const int_type n = A.size();
    float_mat E(n, n, 0.0);
    float_mat B(A);
    int_type i;

    for (i = 0; i < n; ++i) {
        E[i][i] = 1.0;
    }

    return lin_solve(B, E);
}

//! returns the transposed matrix.
static float_mat transpose(const float_mat &a)
{
    float_mat res(a.nr_cols(), a.nr_rows());
    size_t i,j;

    for (i = 0; i < a.nr_rows(); ++i) {
        for (j = 0; j < a.nr_cols(); ++j) {
            res[j][i] = a[i][j];
        }
    }
    return res;
}

//! matrix multiplication.
float_mat operator *(const float_mat &a, const float_mat &b)
{
    float_mat res(a.nr_rows(), b.nr_cols());
    if (a.nr_cols() != b.nr_rows()) {
        sgs_error("incompatible matrices in multiplication\n");
        return res;
    }

    size_t i,j,k;

    for (i = 0; i < a.nr_rows(); ++i) {
        for (j = 0; j < b.nr_cols(); ++j) {
            flt_type sum(0.0);
            for (k = 0; k < a.nr_cols(); ++k) {
                sum += a[i][k] * b[k][j];
            }
            res[i][j] = sum;
        }
    }
    return res;
}


//! calculate savitzky golay coefficients.
static flt_vector_type sg_coeff(const flt_vector_type &b, const size_t deg)
{
    const size_t rows(b.size());
    const size_t cols(deg + 1);
    float_mat A(rows, cols);
    flt_vector_type res(rows);

    // generate input matrix for least squares fit
    size_t i,j;
    for (i = 0; i < rows; ++i) {
        for (j = 0; j < cols; ++j) {
            A[i][j] = runtime_ipow(i, j);
        }
    }

    float_mat c(invert(transpose(A) * A) * (transpose(A) * transpose(b)));

    for (i = 0; i < b.size(); ++i) {
        res[i] = c[0][0];
        for (j = 1; j <= deg; ++j) {
            res[i] += c[j][0] * runtime_ipow(i, j);
        }
    }
    return res;
}

/*! \brief savitzky golay smoothing.
 *
 * This method means fitting a polynome of degree 'deg' to a sliding window
 * of width 2w+1 throughout the data.  The needed coefficients are
 * generated dynamically by doing a least squares fit on a "symmetric" unit
 * vector of size 2w+1, e.g. for w=2 b=(0,0,1,0,0). evaluating the polynome
 * yields the sg-coefficients.  at the border non symmectric vectors b are
 * used. */
flt_vector_type sg_smooth(const flt_vector_type &v, const size_t & width, const size_t & deg)
{
    flt_vector_type res(v.size(), 0.0);
    if ((width < 1) || (deg < 1) || (v.size() < (2 * width + 2))) {
        sgs_error("sgsmooth: parameter error.\n");
        return res;
    }

    const size_t window = 2 * width + 1;
    const size_t endidx = v.size() - 1;

    // handle border cases first because we need different coefficients
    size_t i,j;
#if defined(_OPENMP)
#pragma omp parallel for private(i,j) schedule(static)
#endif
    for (i = 0; i < width; ++i) {
        flt_vector_type b1(window, 0.0);
        b1[i] = 1.0;

        const flt_vector_type c1(sg_coeff(b1, deg));
        for (j = 0; j < window; ++j) {
            res[i]          += c1[j] * v[j];
            res[endidx - i] += c1[j] * v[endidx - j];
        }
    }

    // now loop over rest of data. reusing the "symmetric" coefficients.
    flt_vector_type b2(window, 0.0);
    b2[width] = 1.0;
    const flt_vector_type c2(sg_coeff(b2, deg));

#if defined(_OPENMP)
#pragma omp parallel for private(i,j) schedule(static)
#endif
    for (i = 0; i <= (v.size() - window); ++i) {
        for (j = 0; j < window; ++j) {
            res[i + width] += c2[j] * v[i + j];
        }
    }
    return res;
}

/*! least squares fit a polynome of degree 'deg' to data in 'b'.
 *  then calculate the first derivative and return it. */
static flt_vector_type lsqr_fprime(const flt_vector_type &b, const size_t deg)
{
    const size_t rows(b.size());
    const size_t cols(deg + 1);
    float_mat A(rows, cols);
    flt_vector_type res(rows);

    // generate input matrix for least squares fit
    size_t i,j;
    for (i = 0; i < rows; ++i) {
        for (j = 0; j < cols; ++j) {
            A[i][j] = IceBRG::runtime_ipow(i, j);
        }
    }

    float_mat c(invert(transpose(A) * A) * (transpose(A) * transpose(b)));

    for (i = 0; i < b.size(); ++i) {
        res[i] = c[1][0];
        for (j = 1; j < deg; ++j) {
            res[i] += c[j + 1][0] * flt_type(j+1)
                * IceBRG::runtime_ipow(i, j);
        }
    }
    return res;
}

/*! \brief savitzky golay smoothed numerical derivative.
 *
 * This method means fitting a polynome of degree 'deg' to a sliding window
 * of width 2w+1 throughout the data.
 *
 * In contrast to the sg_smooth function we do a brute force attempt by
 * always fitting the data to a polynome of degree 'deg' and using the
 * result. */
std::vector<flt_type> sg_derivative(const std::vector<flt_type> &v, const size_t & width,
                         const size_t & deg, const flt_type & h)
{
    flt_vector_type res(v.size(), 0.0);

    const size_t window = 2 * width + 1;

    if ((width < 1) || (deg < 1) || (v.size() < (window-1))) {
        sgs_error("sgsderiv: parameter error.\n");
        return res;
    }

    // handle border cases first because we do not repeat the fit
    // lower part
    flt_vector_type b(window, 0.0);
    size_t i,j;

    for (i = 0; i < window; ++i) {
        b[i] = v[i] / h;
    }
    const flt_vector_type c(lsqr_fprime(b, deg));
    for (j = 0; j <= width; ++j) {
        res[j] = c[j];
    }
    // upper part. direction of fit is reversed
    for (i = 0; i < window; ++i) {
        b[i] = v[v.size() - 1 - i] / h;
    }
    const flt_vector_type d(lsqr_fprime(b, deg));
    for (i = 0; i <= width; ++i) {
        res[v.size() - 1 - i] = -d[i];
    }

    // now loop over rest of data. wasting a lot of least squares calcs
    // since we only use the middle value.
//#if defined(_OPENMP)
//#pragma omp parallel for private(i,j) schedule(static)
//#endif
    for (i = 1; i < (v.size() - window); ++i) {
        for (j = 0; j < window; ++j) {
            b[j] = v[i + j] / h;
        }
        res[i + width] = lsqr_fprime(b, deg)[width];
    }
    return res;
}

void sgs_error(const char *errmsg)
{
	throw std::logic_error(errmsg);
}

}} // end all namespaces

// Local Variables:
// mode: c++
// c-basic-offset: 4
// fill-column: 76
// indent-tabs-mode: nil
// End:




