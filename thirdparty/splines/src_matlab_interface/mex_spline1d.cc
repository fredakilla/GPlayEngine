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

#define ASSERT(COND,MSG)                 \
  if ( !(COND) ) {                       \
    std::ostringstream ost ;             \
    ost << "spline2d: " << MSG << '\n' ; \
    mexErrMsgTxt(ost.str().c_str()) ;    \
  }

static
void
mexErrorMessage() {
  // Check for proper number of arguments, etc
  mexErrMsgTxt(
"%======================================================================%\n"
"% spline1d:  Compute spline curve                                      %\n"
"%                                                                      %\n"
"% USAGE: spline1d( 'name', kind, X, Y ) ;                              %\n"
"%        pp         = spline1d( kind, X, Y ) ;                         %\n"
"%        P          = spline1d( 'name', X ) ;                          %\n"
"%        [P,DP]     = spline1d( 'name', X ) ;                          %\n"
"%        [P,DP,DDP] = spline1d( 'name', X ) ;                          %\n"
"%                                                                      %\n"
"% On input:                                                            %\n"
"%                                                                      %\n"
"%  kind = string with the kind of spline, any of:                      %\n"
"%         'linear', 'cubic', 'akima', 'bessel', 'pchip', 'quintic'     %\n"
"%  X = vector of X coordinates                                         %\n"
"%  Y = vector of Y coordinates                                         %\n"
"%                                                                      %\n"
"% On output:                                                           %\n"
"%                                                                      %\n"
"%  P   = vector of Y values                                            %\n"
"%  DP  = vector of dimension size(X) with derivative                   %\n"
"%  DDP = vector of dimension size(X) with second derivative            %\n"
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

  static std::map<string,Splines::Spline*> pSplines ;

  // the first argument must be a string

  ASSERT( mxIsChar(arg_spline_name), "First argument must be a string" ) ;
  string sname = mxArrayToString(arg_spline_name) ;

  if ( nrhs == 4 ) { // setup

    ASSERT( mxIsChar(arg_spline_type), "Second argument expected to be a string" ) ;

    string tname = mxArrayToString(arg_spline_type) ;
    
    Splines::Spline* p_spline = nullptr ;

    if ( tname == "linear" ) {
      p_spline = new Splines::LinearSpline() ;
    } else if ( tname == "cubic" ) {
      p_spline = new Splines::CubicSpline() ;
    } else if ( tname == "akima" ) {
      p_spline = new Splines::AkimaSpline() ;
    } else if ( tname == "bessel" ) {
      p_spline = new Splines::BesselSpline() ;
    } else if ( tname == "pchip" ) {
      p_spline = new Splines::PchipSpline() ;
    } else if ( tname == "quintic" ) {
      p_spline = new Splines::QuinticSpline() ;
    } else {
      ASSERT(false,"Second argument must be one of the strings:\n" <<
             "'linear', 'cubic', 'akima', 'bessel', 'pchip', 'quintic'" ) ;
    }
    pSplines[sname] = p_spline ;

    mxArray const * arg_x = prhs[2] ;
    mxArray const * arg_y = prhs[3] ;

    mwSize number_of_dimensions = mxGetNumberOfDimensions(arg_x) ;
    ASSERT( number_of_dimensions == 2, "Expect vector as third argument" ) ;
    mwSize const * dims_x = mxGetDimensions(arg_x) ;
    ASSERT( dims_x[0] == 1 || dims_x[1] == 1, "Expect (1 x n or n x 1) matrix as third argument, found " << dims_x[0] << " x " << dims_x[1] ) ;
    double const * x = mxGetPr(arg_x) ;
    sizeType  nx = dims_x[0]*dims_x[1] ;

    number_of_dimensions = mxGetNumberOfDimensions(arg_y) ;
    ASSERT( number_of_dimensions == 2, "Expect vector as 4th argument" ) ;
    mwSize const * dims_y = mxGetDimensions(arg_y) ;
    ASSERT( dims_y[0] == 1 || dims_y[1] == 1, "Expect (1 x n or n x 1) matrix as 4th argument, found " << dims_y[0] << " x " << dims_y[1] ) ;
    double const * y = mxGetPr(arg_y) ;
    sizeType  ny = dims_y[0]*dims_y[1] ;

    ASSERT( nx == ny, "lenght of third and fourth argument mut be the same" ) ;

    p_spline -> build( x, y, nx ) ;

  } else if ( nrhs == 3 ) { // eval

    Splines::Spline* p_spline = nullptr ;

    if ( sname == "linear" ) {
      p_spline = new Splines::LinearSpline() ;
    } else if ( sname == "cubic" ) {
      p_spline = new Splines::CubicSpline() ;
    } else if ( sname == "akima" ) {
      p_spline = new Splines::AkimaSpline() ;
    } else if ( sname == "bessel" ) {
      p_spline = new Splines::BesselSpline() ;
    } else if ( sname == "pchip" ) {
      p_spline = new Splines::PchipSpline() ;
    } else if ( sname == "quintic" ) {
      p_spline = new Splines::QuinticSpline() ;
    } else {
      ASSERT(false,"Second argument must be one of the strings:\n" <<
             "'linear', 'cubic', 'akima', 'bessel', 'pchip', 'quintic'" ) ;
    }
    pSplines[sname] = p_spline ;

    mxArray const * arg_x = prhs[1] ;
    mxArray const * arg_y = prhs[2] ;

    mwSize number_of_dimensions = mxGetNumberOfDimensions(arg_x) ;
    ASSERT( number_of_dimensions == 2, "Expect vector as second argument" ) ;
    mwSize const * dims_x = mxGetDimensions(arg_x) ;
    ASSERT( dims_x[0] == 1 || dims_x[1] == 1, "Expect (1 x n or n x 1) matrix as third argument, found " << dims_x[0] << " x " << dims_x[1] ) ;
    double const * x = mxGetPr(arg_x) ;
    sizeType nx = dims_x[0]*dims_x[1] ;

    number_of_dimensions = mxGetNumberOfDimensions(arg_y) ;
    ASSERT( number_of_dimensions == 2, "Expect vector as third argument" ) ;
    mwSize const * dims_y = mxGetDimensions(arg_y) ;
    ASSERT( dims_y[0] == 1 || dims_y[1] == 1, "Expect (1 x n or n x 1) matrix as 4th argument, found " << dims_y[0] << " x " << dims_y[1] ) ;
    double const * y = mxGetPr(arg_y) ;
    sizeType ny = dims_y[0]*dims_y[1] ;

    ASSERT( nx == ny, "lenght of third and fourth argument must be the same" ) ;

    p_spline -> build( x, y, nx ) ;

    static char const *fieldnames[] = {
      "form", "breaks", "coefs", "pieces", "order", "dim"
    } ;
    int nfield = 6 ;
    plhs[0] = mxCreateStructMatrix(1,1,nfield,fieldnames);
 
    mxArray * mx_nodes = mxCreateNumericMatrix(1, nx, mxDOUBLE_CLASS, mxREAL);
    valueType  * nodes = mxGetPr(mx_nodes) ;

    mxArray * mx_coeffs = mxCreateNumericMatrix(nx-1, p_spline -> order(), mxDOUBLE_CLASS, mxREAL);
    valueType     * cfs = mxGetPr(mx_coeffs) ;

    p_spline -> coeffs( cfs, nodes, false ) ;

    mxSetFieldByNumber( plhs[0], 0, 0, mxCreateString( "pp" ) ) ;
    mxSetFieldByNumber( plhs[0], 0, 1, mx_nodes ) ;
    mxSetFieldByNumber( plhs[0], 0, 2, mx_coeffs ) ;
    mxSetFieldByNumber( plhs[0], 0, 3, mxCreateDoubleScalar( nx-1 ) ) ;
    mxSetFieldByNumber( plhs[0], 0, 4, mxCreateDoubleScalar( p_spline -> order() ) ) ;
    mxSetFieldByNumber( plhs[0], 0, 5, mxCreateDoubleScalar( 1 ) ) ;
    
    delete p_spline ;

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
