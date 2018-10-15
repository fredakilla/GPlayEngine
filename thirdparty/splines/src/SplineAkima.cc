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
/*
//     #
//    # #   #    # # #    #   ##
//   #   #  #   #  # ##  ##  #  #
//  #     # ####   # # ## # #    #
//  ####### #  #   # #    # ######
//  #     # #   #  # #    # #    #
//  #     # #    # # #    # #    #
*/

namespace Splines {

  using namespace std ; // load standard namspace

  static
  valueType
  akima_one( valueType epsi,
             valueType di_m2,
             valueType di_m1,
             valueType di,
             valueType di_p1 ) {
    valueType wl  = std::abs(di_p1 - di) ;
    valueType wr  = std::abs(di_m1 - di_m2);
    valueType den = wl + wr ;
    if ( den <= epsi ) { wl = wr = 0.5 ; den = 1 ; } // if epsi == 0
    valueType num = wl * di_m1 + wr * di;
    return num / den ;
  }

  static
  void
  Akima_build( valueType const X[],
               valueType const Y[],
               valueType       Yp[],
               sizeType        npts ) {

    if ( npts == 2 ) { // solo 2 punti, niente da fare
      Yp[0] = Yp[1] = (Y[1]-Y[0])/(X[1]-X[0]) ;
    } else {
      std::vector<valueType> m(npts+3) ;

      // calcolo slopes (npts-1) intervals + 4
      for ( sizeType i = 1 ; i < npts ; ++i )
        m[i+1] = (Y[i]-Y[i-1])/(X[i]-X[i-1]) ;

      // extra slope at the boundary
      m[1]      = 2*m[2]-m[3] ;
      m[0]      = 2*m[1]-m[2] ;
      m[npts+1] = 2*m[npts]-m[npts-1] ;
      m[npts+2] = 2*m[npts+1]-m[npts] ;

      // minimum delta slope
      valueType epsi = 0 ;
      for ( sizeType i = 0 ; i < npts+2 ; ++i ) {
        valueType dm = std::abs(m[i+1]-m[i]) ;
        if ( dm > epsi ) epsi = dm ; 
      }
      epsi *= 1E-8 ;

      // 0  1  2  3  4---- n-1 n n+1 n+2
      //       +  +  +      +  +
      for ( sizeType i = 0 ; i < npts ; ++i )
        Yp[i] = akima_one( epsi, m[i], m[i+1], m[i+2], m[i+3] ) ;
    }
  }

  void
  AkimaSpline::build() {
    SPLINE_ASSERT( npts > 1,"AkimaSpline::build(): npts = " << npts << " not enought points" );
    sizeType ibegin = 0 ;
    sizeType iend   = 0 ;
    do {
      // cerca intervallo monotono strettamente crescente
      while ( ++iend < npts && X[iend-1] < X[iend] ) {} ;
      Akima_build( X+ibegin, Y+ibegin, Yp+ibegin, iend-ibegin ) ;
      ibegin = iend ;
    } while ( iend < npts ) ;
    
    SPLINE_CHECK_NAN(Yp,"AkimaSpline::build(): Yp",npts);
  }

}
