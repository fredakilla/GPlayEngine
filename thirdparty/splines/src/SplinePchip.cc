/*--------------------------------------------------------------------------*\
 |                                                                          |
 |  Copyright (C) 2016                                                      |
 |                                                                          |
 |         , __                 , __                                        |
 |        /|/  \               /|/  \                                       |
 |         | __/ _   ,_         | __/ _   ,_                                | 
 |         |   \|/  /  |  |   | |   \|/  /  |  |   |                        |
 |         |(__/|__/   |_/ \_/|/|(__/|__/   |_/ \_/|/                       |
 |                           /|                   /|                        |
 |                           \|                   \|                        |
 |                                                                          |
 |      Enrico Bertolazzi                                                   |
 |      Dipartimento di Ingegneria Industriale                              |
 |      Universita` degli Studi di Trento                                   |
 |      email: enrico.bertolazzi@unitn.it                                   |
 |                                                                          |
\*--------------------------------------------------------------------------*/

#include "Splines.hh"

#include <cmath>

/**
 * 
 */

namespace Splines {

  using namespace std ; // load standard namspace
  
  static
  inline
  valueType
  max_abs( valueType a, valueType b ) {
    valueType res = std::abs(a) ;
    if ( res < std::abs(b) ) res = std::abs(b) ;
    return res ;
  }
  
  static
  inline
  valueType
  min_abs( valueType a, valueType b ) {
    valueType res = std::abs(a) ;
    if ( res > std::abs(b) ) res = std::abs(b) ;
    return res ;
  }

  /*
  //   ____      _     _      ____        _ _            
  //  |  _ \ ___| |__ (_)_ __/ ___| _ __ | (_)_ __   ___ 
  //  | |_) / __| '_ \| | '_ \___ \| '_ \| | | '_ \ / _ \
  //  |  __/ (__| | | | | |_) |__) | |_) | | | | | |  __/
  //  |_|   \___|_| |_|_| .__/____/| .__/|_|_|_| |_|\___|
  //                    |_|        |_|                   
  */
  static
  inline
  int
  signTest( valueType const a, valueType const b ) {
    int sa = 0 ;
    int sb = 0 ;
    if      ( a > 0 ) sa =  1 ;
    else if ( a < 0 ) sa = -1 ;
    if      ( b > 0 ) sb =  1 ;
    else if ( b < 0 ) sb = -1 ;    
    return sa*sb ;
  }

  /*!
  // Reference:
  // ==========
  //
  //    F.N. Fritsch, R.E. Carlson:
  //    Monotone Piecewise Cubic Interpolation,
  //    SIAM J. Numer. Anal. Vol 17, No. 2, April 1980
  //
  //    F.N. Fritsch and J. Butland:
  //    A method for constructing local monotone piecewise cubic interpolants,
  //    SIAM Journal on Scientific and Statistical Computing 5, 2 (June 1984), pp. 300-304.
  */
  void
  pchip( valueType const X[],
         valueType const Y[],
         valueType       Yp[],
         sizeType        n ) {

    indexType ierr = 0 ;

    // function definition is ok, go on.
    valueType h1    = X[1] - X[0] ;
    valueType del1  = (Y[1]-Y[0])/h1 ;
    valueType dsave = del1 ;

    // special case n=2 -- use linear interpolation.
    if ( n == 1 ) { Yp[0] = Yp[1] = del1 ; return ; }

    valueType h2   = X[2] - X[1] ;
    valueType del2 = (Y[2]-Y[1])/h2 ;

    // Set Yp[0] via non-centered three-point formula, adjusted to be shape-preserving.
    valueType hsum = h1 + h2 ;
    valueType w1   = (h1 + hsum)/hsum ;
    valueType w2   = -h1/hsum ;
    Yp[0] = w1*del1 + w2*del2 ;
    valueType dmin, dmax ;
    if ( signTest(Yp[0],del1) <= 0 ) {
      Yp[0] = 0 ;
    } else if ( signTest(del1,del2) < 0 ) {
      // NEED DO THIS CHECK ONLY IF MONOTONICITY SWITCHES.
      dmax = 3*del1 ;
      if ( std::abs(Yp[0]) > std::abs(dmax) ) Yp[0] = dmax ;
    }

    // loop through interior points.
    for ( sizeType i = 1 ; i < n ; ++i ) {
      if ( i > 1 ) {
        h1   = h2 ;
        h2   = X[i+1] - X[i] ;
        hsum = h1 + h2 ;
        del1 = del2 ;
        del2 = (Y[i+1] - Y[i])/h2 ;
      }
      // set Yp[i]=0 unless data are strictly monotonic.
      Yp[i] = 0 ;
      // count number of changes in direction of monotonicity.
      switch ( signTest(del1,del2) ) {
      case -1:
        if ( del2 == 0 ) break ;
        if ( signTest(dsave,del2) < 0 ) ++ierr ;
        dsave = del2 ;
        break ;
      case 0:
        ++ierr ;
        dsave = del2 ;
        break ;
      case 1: // use brodlie modification of butland formula.
        w1    = (1+h1/hsum)/3 ;
        w2    = (1+h2/hsum)/3 ;
        dmax  = max_abs( del1, del2 ) ;
        dmin  = min_abs( del1, del2 ) ;
        valueType drat1 = del1/dmax ;
        valueType drat2 = del2/dmax ;
        Yp[i] = dmin/(w1*drat1 + w2*drat2) ;
        break ;
      }
    }
    // set Yp[n] via non-centered three-point formula, adjusted to be shape-preserving.
    w1 = -h2/hsum ;
    w2 = (h2 + hsum)/hsum ;
    Yp[n] = w1*del1 + w2*del2 ;
    if ( signTest(Yp[n],del2) <= 0 ) {
      Yp[n] = 0 ;
    } else if ( signTest(del1,del2) < 0 ) {
      // need do this check only if monotonicity switches.
      dmax = 3*del2 ;
      if ( abs(Yp[n]) > abs(dmax) ) Yp[n] = dmax ;
    }
    // cout << "ierr = " << ierr << '\n' ;
  }

  void
  PchipSpline::build() {
    SPLINE_ASSERT( npts > 1,
                   "PchipSpline::build(): npts = " << npts <<
                   " not enought points" );
    sizeType ibegin = 0 ;
    sizeType iend   = 0 ;
    do {
      // cerca intervallo monotono strettamente crescente
      while ( ++iend < npts && X[iend-1] < X[iend] ) {} ;
      pchip( X+ibegin, Y+ibegin, Yp+ibegin, (iend-ibegin)-1 ) ;
      ibegin = iend ;
    } while ( iend < npts ) ;
    
    SPLINE_CHECK_NAN(Yp,"PchipSpline::build(): Yp",npts);
    //pchip( X, Y, Yp, npts -1 ) ;
  }

}
