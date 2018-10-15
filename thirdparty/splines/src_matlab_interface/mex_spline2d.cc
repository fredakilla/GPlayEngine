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

#define arg_spline_name   prhs[0]
#define arg_spline_type   prhs[1]

#define out_z             plhs[0]
#define out_Dx            plhs[1]
#define out_Dy            plhs[2]
#define out_Dxx           plhs[3]
#define out_Dxy           plhs[4]
#define out_Dyy           plhs[5]

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
"% spline2d:  Compute spline surface                                    %\n"
"%                                                                      %\n"
"% USAGE: spline2d( 'name', 'type', X, Y, Z ) ;                         %\n"
"%        P = spline2d( 'name', X, Y ) ;                                %\n"
"%        P = spline2d( 'name', XY ) ;                                  %\n"
"%        [P,DX,DY] = spline2d( 'name', X, Y ) ;                        %\n"
"%        [P,DX,DY] = spline2d( 'name', XY ) ;                          %\n"
"%        [P,DX,DY,DXX,DXY,DYY] = spline2d( 'name', X, Y ) ;            %\n"
"%        [P,DX,DY,DXX,DXY,DYY] = spline2d( 'name', XY ) ;              %\n"
"%                                                                      %\n"
"% On input:                                                            %\n"
"%                                                                      %\n"
"%  X  = vector of X coordinates                                        %\n"
"%  Y  = vector of Y coordinates                                        %\n"
"%  Z  = matrix of Z coordinates Z(i,j) correspond to X(i),Y(j)         %\n"
"%  XY = matrix 2 x n of point where spline is computed                 %\n"
"%                                                                      %\n"
"% On output:                                                           %\n"
"%                                                                      %\n"
"%  P   = matrix nx x ny of Z values if input are X, Y and              %\n"
"%        length(X) = nx, length(Y) = ny or a vector 1 x n              %\n"
"%        if input is XY                                                %\n"
"%  DX  = matrix of dimension size(P) with X-derivative                 %\n"
"%  DY  = matrix of dimension size(P) with Y-derivative                 %\n"
"%  DXX = matrix of dimension size(P) with XX-derivative                %\n"
"%  DXY = matrix of dimension size(P) with XY-derivative                %\n"
"%  DYY = matrix of dimension size(P) with YY-derivative                %\n"
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

  static std::map<string,Splines::SplineSurf*> pSplines ;

  // the first argument must be a string
  
  ASSERT( mxIsChar(arg_spline_name), "First argument must be a string" ) ;
  string sname = mxArrayToString(arg_spline_name) ;

  if ( mxIsChar(arg_spline_type) ) { // constructor
    string tname = mxArrayToString(arg_spline_type) ;
    
    Splines::SplineSurf * p_spline = nullptr ;

    if ( tname == "bilinear" ) {
      p_spline = new Splines::BilinearSpline() ;
    } else if ( tname == "bicubic" ) {
      p_spline = new Splines::BiCubicSpline() ;
    } else if ( tname == "akima" ) {
      p_spline = new Splines::Akima2Dspline() ;
    } else if ( tname == "biquintic" ) {
      p_spline = new Splines::BiQuinticSpline() ;
    } else {
      ASSERT(false,"Second argument must be one of the strings: 'bilinear', 'bicubic', 'akima', 'biquintic'" ) ;
    }
    pSplines[sname] = p_spline ;

    mxArray const * arg_x = prhs[2] ;
    mxArray const * arg_y = prhs[3] ;
    mxArray const * arg_z = prhs[4] ;

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

    number_of_dimensions = mxGetNumberOfDimensions(arg_z) ;
    ASSERT( number_of_dimensions == 2, "Expect matrix as 5th argument" ) ;
    mwSize const * dims_z = mxGetDimensions(arg_z) ;
    ASSERT( dims_z[0] == nx || dims_z[1] == ny, "Expect (" << nx << " x " << ny << " matrix as 5th argument, found " << dims_z[0] << " x " << dims_z[1] ) ;
    double const * z = mxGetPr(arg_z) ;
    sizeType ldZ = nx ;

    //for ( mwSize i = 0 ; i < nx ; ++i ) mexPrintf("x[%d] = %g\n", i, x[i]) ;
    //for ( mwSize j = 0 ; j < ny ; ++j ) mexPrintf("y[%d] = %g\n", j, y[j]) ;
    //for ( mwSize j = 0 ; j < nx*ny ; ++j ) mexPrintf("z[%d] = %g\n", j, z[j]) ;

    p_spline -> build ( x, 1, y, 1, z, ldZ, nx, ny, true, false ) ;

  } else { // eval
    // check if spline exists
    std::map<string,Splines::SplineSurf*>::iterator it = pSplines.find(sname) ;
    ASSERT( it != pSplines.end(), "Spline: ``" << sname << "'' not defined" );
    
    // evaluation
    if ( nrhs == 2 ) { // spline2d('nome',XY)
      //arg_xy
      mxArray const * arg_xy = prhs[1] ;

      mwSize number_of_dimensions = mxGetNumberOfDimensions(arg_xy) ;
      ASSERT( number_of_dimensions == 2, "Expect (2 x n) matrix as second argument" ) ;
      mwSize const * dims = mxGetDimensions(arg_xy) ;
      ASSERT( dims[0] == 2, "Expect (2 x n) matrix as second argument, found " << dims[0] << " x " << dims[1] ) ;

      valueType * x = mxGetPr(arg_xy) ;
      
      ASSERT( nlhs >= 1, "Expect at least one output argument" ) ;
      
      Splines::SplineSurf * p_spline = it->second ;

      if ( nlhs == 1 ) {
        out_z = mxCreateNumericMatrix(1, dims[1], mxDOUBLE_CLASS, mxREAL);
        valueType * z = mxGetPr(out_z) ;
        for ( mwSize i = 0 ; i < dims[1] ; ++i )
          z[i] = (*p_spline)(x[2*i],x[2*i+1]) ;
      } else if ( nlhs == 3 ) {
        out_z  = mxCreateNumericMatrix(1, dims[1], mxDOUBLE_CLASS, mxREAL);
        out_Dx = mxCreateNumericMatrix(1, dims[1], mxDOUBLE_CLASS, mxREAL);
        out_Dy = mxCreateNumericMatrix(1, dims[1], mxDOUBLE_CLASS, mxREAL);
        valueType * z  = mxGetPr(out_z) ;
        valueType * dx = mxGetPr(out_Dx) ;
        valueType * dy = mxGetPr(out_Dy) ;
        for ( mwSize i = 0 ; i < dims[1] ; ++i ) {
          valueType vals[3] ;
          p_spline->D(x[2*i],x[2*i+1],vals) ;
          z[i] = vals[0] ;
          dx[i] = vals[1] ;
          dy[i] = vals[2] ;
        }
      } else if ( nlhs == 6 ) {
        out_z   = mxCreateNumericMatrix(1, dims[1], mxDOUBLE_CLASS, mxREAL);
        out_Dx  = mxCreateNumericMatrix(1, dims[1], mxDOUBLE_CLASS, mxREAL);
        out_Dy  = mxCreateNumericMatrix(1, dims[1], mxDOUBLE_CLASS, mxREAL);
        out_Dxx = mxCreateNumericMatrix(1, dims[1], mxDOUBLE_CLASS, mxREAL);
        out_Dxy = mxCreateNumericMatrix(1, dims[1], mxDOUBLE_CLASS, mxREAL);
        out_Dyy = mxCreateNumericMatrix(1, dims[1], mxDOUBLE_CLASS, mxREAL);
        valueType * z   = mxGetPr(out_z) ;
        valueType * dx  = mxGetPr(out_Dx) ;
        valueType * dy  = mxGetPr(out_Dy) ;
        valueType * dxx = mxGetPr(out_Dxx) ;
        valueType * dxy = mxGetPr(out_Dxy) ;
        valueType * dyy = mxGetPr(out_Dyy) ;
        for ( mwSize i = 0 ; i < dims[1] ; ++i ) {
          valueType vals[6] ;
          p_spline->DD(x[2*i],x[2*i+1],vals) ;
          z[i]   = vals[0] ;
          dx[i]  = vals[1] ;
          dy[i]  = vals[2] ;
          dxx[i] = vals[3] ;
          dxy[i] = vals[4] ;
          dyy[i] = vals[5] ;
        }
      } else {
        ASSERT( false, "Expect 1, 3, or 6 output arguments" ) ;
      }

    } else if ( nrhs == 3 ) { // spline2d('nome',X,Y)

      mxArray const * arg_x = prhs[1] ;
      mwSize number_of_dimensions = mxGetNumberOfDimensions(arg_x) ;
      ASSERT( number_of_dimensions == 2, "Expect a vector as second argument" ) ;
      mwSize const * dims_x = mxGetDimensions(arg_x) ;
      ASSERT( dims_x[0] == 1 || dims_x[1] == 1,
              "Expect row or column vector as second argument, found matrix " <<
              dims_x[0] << " x " << dims_x[1] ) ;
      valueType * x = mxGetPr(arg_x) ;
      mwSize     nx = dims_x[0]*dims_x[1] ;

      mxArray const * arg_y = prhs[2] ;
      number_of_dimensions = mxGetNumberOfDimensions(arg_y) ;
      ASSERT( number_of_dimensions == 2, "Expect a vector as third argument" ) ;
      mwSize const * dims_y = mxGetDimensions(arg_y) ;
      ASSERT( dims_y[0] == 1 || dims_y[1] == 1,
              "Expect row or column vector as third argument, found matrix " <<
              dims_y[0] << " x " << dims_y[1] ) ;
      valueType * y = mxGetPr(arg_y) ;
      mwSize     ny = dims_y[0]*dims_y[1] ;

      ASSERT( nlhs >= 1, "Expect at least one output argument" ) ;
      
      Splines::SplineSurf * p_spline = it->second ;

      if ( nlhs == 1 ) {
        out_z = mxCreateNumericMatrix(nx, ny, mxDOUBLE_CLASS, mxREAL);
        valueType * z = mxGetPr(out_z) ;
        for ( mwSize j = 0 ; j < ny ; ++j )
          for ( mwSize i = 0 ; i < nx ; ++i )
            z[i+j*nx] = (*p_spline)(x[i],y[j]) ;
      } else if ( nlhs == 3 ) {
        out_z  = mxCreateNumericMatrix(nx, ny, mxDOUBLE_CLASS, mxREAL);
        out_Dx = mxCreateNumericMatrix(nx, ny, mxDOUBLE_CLASS, mxREAL);
        out_Dy = mxCreateNumericMatrix(nx, ny, mxDOUBLE_CLASS, mxREAL);
        valueType * z  = mxGetPr(out_z) ;
        valueType * dx = mxGetPr(out_Dx) ;
        valueType * dy = mxGetPr(out_Dy) ;
        for ( mwSize j = 0 ; j < ny ; ++j ) {
          for ( mwSize i = 0 ; i < nx ; ++i ) {
            valueType vals[3] ;
            p_spline->D(x[i],y[j],vals) ;
            mwSize ij = i+j*nx ;
            z[ij]  = vals[0] ;
            dx[ij] = vals[1] ;
            dy[ij] = vals[2] ;
          }
        }
      } else if ( nlhs == 6 ) {
        out_z   = mxCreateNumericMatrix(nx, ny, mxDOUBLE_CLASS, mxREAL);
        out_Dx  = mxCreateNumericMatrix(nx, ny, mxDOUBLE_CLASS, mxREAL);
        out_Dy  = mxCreateNumericMatrix(nx, ny, mxDOUBLE_CLASS, mxREAL);
        out_Dxx = mxCreateNumericMatrix(nx, ny, mxDOUBLE_CLASS, mxREAL);
        out_Dxy = mxCreateNumericMatrix(nx, ny, mxDOUBLE_CLASS, mxREAL);
        out_Dyy = mxCreateNumericMatrix(nx, ny, mxDOUBLE_CLASS, mxREAL);
        valueType * z   = mxGetPr(out_z) ;
        valueType * dx  = mxGetPr(out_Dx) ;
        valueType * dy  = mxGetPr(out_Dy) ;
        valueType * dxx = mxGetPr(out_Dxx) ;
        valueType * dxy = mxGetPr(out_Dxy) ;
        valueType * dyy = mxGetPr(out_Dyy) ;
        for ( mwSize j = 0 ; j < ny ; ++j ) {
          for ( mwSize i = 0 ; i < nx ; ++i ) {
            valueType vals[6] ;
            p_spline->DD(x[i],y[j],vals) ;
            mwSize ij = i+j*nx ;
            z[ij]   = vals[0] ;
            dx[ij]  = vals[1] ;
            dy[ij]  = vals[2] ;
            dxx[ij] = vals[3] ;
            dxy[ij] = vals[4] ;
            dyy[ij] = vals[5] ;
          }
        }
      } else {
        ASSERT( false, "Expect 1, 3, or 6 output arguments" ) ;
      }
    } else {
      mexErrorMessage() ;
    }
  }
}
