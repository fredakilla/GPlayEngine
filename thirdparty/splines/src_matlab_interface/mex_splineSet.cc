/****************************************************************************\
Copyright (c) 2015, Enrico Bertolazzi
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.
\****************************************************************************/

#include "Splines.hh"
#include "mex.h"
#include <map>
#include <string>

using namespace std ;
using Splines::valueType ;
using Splines::sizeType ;

#define arg_spline_name prhs[0]
#define arg_spline_type prhs[1]

#define out_P           plhs[0]
#define out_DP          plhs[1]
#define out_DDP         plhs[2]

#define ASSERT(COND,MSG)                  \
  if ( !(COND) ) {                        \
    std::ostringstream ost ;              \
    ost << "splineSet: " << MSG << '\n' ; \
    mexErrMsgTxt(ost.str().c_str()) ;     \
  }

static
void
mexErrorMessage() {
  // Check for proper number of arguments, etc
  mexErrMsgTxt(
"%======================================================================%\n"
"% splineSet:  Compute spline curve set                                 %\n"
"%                                                                      %\n"
"% USAGE: splineSet( 'name', kinds, X, Y ) ;                            %\n"
"%        P          = splineSet( 'name', X ) ;                         %\n"
"%        [P,DP]     = splineSet( 'name', X ) ;                         %\n"
"%        [P,DP,DDP] = splineSet( 'name', X ) ;                         %\n"
"%                                                                      %\n"
"% On input:                                                            %\n"
"%                                                                      %\n"
"%  kinds = cell of strings with the kind of spline, any of:            %\n"
"%         'linear', 'cubic', 'akima', 'bessel', 'pchip', 'quintic'     %\n"
"%  X = vector of X coordinates                                         %\n"
"%  Y = matrix of dimension length(X) x NSPL with Y coordinates         %\n"
"%                                                                      %\n"
"% On output:                                                           %\n"
"%                                                                      %\n"
"%  P   = matrix of dimension size(X) x NSPL with Y values              %\n"
"%  DP  = matrix of dimension size(X) x NSPL with derivative            %\n"
"%  DDP = matrix of dimension size(X) x NSPL with second derivative     %\n"
"%                                                                      %\n"
"%======================================================================%\n"
"%                                                                      %\n"
"%  Autor: Enrico Bertolazzi                                            %\n"
"%         Department of Industrial Engineering                         %\n"
"%         University of Trento                                         %\n"
"%         enrico.bertolazzi@unitn.it                                   %\n"
"%                                                                      %\n"
"%======================================================================%\n" ) ;
}

extern "C"
void
mexFunction( int nlhs, mxArray       *plhs[],
             int nrhs, mxArray const *prhs[] ) {

  static std::map<string,Splines::SplineSet*> pSplineSet ;

  // the first argument must be a string

  ASSERT( mxIsChar(arg_spline_name), "First argument must be a string" ) ;
  string sname = mxArrayToString(arg_spline_name) ;

  if ( nrhs == 4 ) { // setup

    // check if spline exists
    std::map<string,Splines::SplineSet*>::iterator it = pSplineSet.find(sname) ;
    Splines::SplineSet * p_spline_set ;
    if ( it != pSplineSet.end() ) {
      p_spline_set = pSplineSet[sname] = new SplineSet() ;
    } else {
      p_spline_set = it->second ;
    };

    ASSERT( mxIsCell(arg_spline_type), "Second argument expected to be cell array" ) ;
    mwSize const *dims = mxGetDimensions(arg_spline_type) ;
    mwSize nspl = dims[0] ;
    std::vector<Splines::SplineType> types ;
    types.reserve(nspl) ;
    for ( mwSize i = 0 ; i < nspl ; ++i ) {
      mxArray const * cell = mxGetCell(arg_spline_type,i);
      ASSERT( mxIsChar(cell),
              "Second argument expected to be cell array of strings" ) ;
      string tname = mxArrayToString(cell) ;
      Splines::SplineType st ;
      if      ( tname == "linear"  ) st = Splines::LINEAR_TYPE ;
      else if ( tname == "cubic"   ) st = Splines::CUBIC_TYPE ;
      else if ( tname == "akima"   ) st = Splines::AKIMA_TYPE ;
      else if ( tname == "bessel"  ) st = Splines::BESSEL_TYPE ;
      else if ( tname == "pchip"   ) st = Splines::PCHIP_TYPE ;
      else if ( tname == "quintic" ) st = Splines::QUINTIC_TYPE ;
      else {
        ASSERT(false, "Cell array of strings must contains the strings:\n" <<
                      "'linear', 'cubic', 'akima', 'bessel', 'pchip', 'quintic'" ) ;
      }
      types.push_back(st) ;
    }

    mxArray const * arg_x = prhs[2] ;
    mxArray const * arg_y = prhs[3] ;

    mwSize number_of_dimensions = mxGetNumberOfDimensions(arg_x) ;
    ASSERT( number_of_dimensions == 2, "Expect vector as third argument" ) ;
    mwSize const * dims_x = mxGetDimensions(arg_x) ;
    ASSERT( dims_x[0] == 1 || dims_x[1] == 1, "Expect (1 x n or n x 1) matrix as third argument, found " << dims_x[0] << " x " << dims_x[1] ) ;
    double const * x = mxGetPr(arg_x) ;
    sizeType npts = dims_x[0]*dims_x[1] ;

    number_of_dimensions = mxGetNumberOfDimensions(arg_y) ;
    ASSERT( number_of_dimensions == 2, "Expect matrix as 4th argument" ) ;
    mwSize const * dims_y = mxGetDimensions(arg_y) ;
    ASSERT( dims_y[0] == npts || dims_y[1] == nspl,
            "Expect (" << nx << " x " << nspl << " matrix as 4th argument, found " <<
            dims_y[0] << " x " << dims_y[1] ) ;
    double const * y = mxGetPr(arg_y) ;
    sizeType ny = dims_y[0]*dims_y[1] ;

    ASSERT( nx == ny, "lenght of third and fourth argument mut be the same" ) ;
    
    std::vector<char const *>   headers ;
    std::vector<double const **> Y ;
    headers.reserve(nspl) ;
    pY.reserve(nspl) ;
    for ( mwSize i = 0 ; i < nspl ; ++i ) {
      headers.push_back() ;
      pY.push_back(y+i*npts) ;
    }
    p_spline_set -> build ( nspl, npts, &headers.front(), &types.front(),
                            x, &Y.front(), nullptr ) ;

  } else if ( nrhs == 2 ) { // eval

    // check if spline exists
    std::map<string,Splines::Spline*>::iterator it = pSplines.find(sname) ;
    ASSERT( it != pSplines.end(), "Spline: ``" << sname << "'' not defined" );

    mxArray const * arg_x = prhs[1] ;
    mwSize number_of_dimensions = mxGetNumberOfDimensions(arg_x) ;
    ASSERT( number_of_dimensions == 2, "Expect (2 x n) matrix as second argument" ) ;
    mwSize const * dims_x = mxGetDimensions(arg_x) ;
    ASSERT( dims_x[0] == 1 || dims_x[1] == 1, "Expect a vector as second argument, found " << dims_x[0] << " x " << dims_x[1] ) ;
    valueType * x = mxGetPr(arg_x) ;
    mwSize n = dims_x[0]*dims_x[1] ;

    ASSERT( nlhs >= 1, "Expect at least one output argument" ) ;

    Splines::Spline * p_spline = it->second ;

    if ( nlhs >= 1 ) {
      out_P = mxCreateNumericMatrix(1, n, mxDOUBLE_CLASS, mxREAL);
      valueType * P = mxGetPr(out_P) ;
      for ( mwSize i = 0 ; i < n ; ++i ) P[i] = (*p_spline)(x[i]) ;
    }
    if ( nlhs >= 2 ) {
      out_DP = mxCreateNumericMatrix(1, n, mxDOUBLE_CLASS, mxREAL);
      valueType * DP = mxGetPr(out_DP) ;
      for ( mwSize i = 0 ; i < n ; ++i ) DP[i] = p_spline->D(x[i]) ;
    }
    if ( nlhs >= 3 ) {
      out_DDP = mxCreateNumericMatrix(1, n, mxDOUBLE_CLASS, mxREAL);
      valueType * DDP = mxGetPr(out_DDP) ;
      for ( mwSize i = 0 ; i < n ; ++i ) DDP[i] = p_spline->DD(x[i]) ;
    }
    ASSERT( nlhs <= 3, "Expect 1, 2, or 3 output arguments" ) ;
  } else {
    mexErrorMessage() ;
  }
}
