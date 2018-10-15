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
#include <iomanip>

#ifdef __GCC__
#pragma GCC diagnostic ignored "-Wc++98-compat"
#endif
#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++98-compat"
#endif

/**
 * 
 */

namespace Splines {

  using namespace std ; // load standard namspace

  void
  ConstantSpline::reserve_external( sizeType n, valueType *& p_x, valueType *& p_y ) {
    if ( !_external_alloc ) baseValue.free() ;
    npts            = 0 ;
    npts_reserved   = n ;
    _external_alloc = true ;
    X = p_x ;
    Y = p_y ;
  }

  void
  ConstantSpline::reserve( sizeType n ) {
    if ( _external_alloc && n <= npts_reserved ) {
      // nothing to do!, already allocated
    } else {
      baseValue.allocate( 2*n ) ;
      npts_reserved   = n ;
      _external_alloc = false ;
      X = baseValue(n) ;
      Y = baseValue(n) ;
    }
    npts = lastInterval = 0 ;
  }

  //! Evalute spline value at `x`
  valueType
  ConstantSpline::operator () ( valueType x ) const {
    if ( x < X[0] ) return Y[0] ;
    if ( npts > 0 && x > X[npts-1] ) return Y[npts-1] ;
    return Y[search(x)] ;
  }

  void
  ConstantSpline::build( valueType const x[], sizeType incx,
                         valueType const y[], sizeType incy,
                         sizeType n ) {
    reserve( n ) ;
    for ( sizeType i = 0 ; i   < n ; ++i ) X[i] = x[i*incx] ;
    for ( sizeType i = 0 ; i+1 < n ; ++i ) Y[i] = y[i*incy] ;
    npts = n ;
    build() ;
  }

  void
  ConstantSpline::clear() {
    if ( !_external_alloc ) baseValue.free() ;
    npts = npts_reserved = 0 ;
    _external_alloc = false ;
    X = Y = nullptr ;
  }

  void
  ConstantSpline::writeToStream( std::basic_ostream<char> & s ) const {
    sizeType nseg = npts > 0 ? npts - 1 : 0 ;
    for ( sizeType i = 0 ; i < nseg ; ++i )
      s << "segment N." << setw(4) << i
        << " X:[ " << X[i] << ", " << X[i+1] << " ] Y:" << Y[i]
        << '\n' ; 
  }

  sizeType // order
  ConstantSpline::coeffs( valueType cfs[], valueType nodes[], bool ) const {
    sizeType nseg = npts > 0 ? npts - 1 : 0 ;
    for ( sizeType i = 0 ; i < nseg ; ++i ) {
      nodes[i] = X[i] ;
      cfs[i]   = Y[i] ;
    }
    return 1 ;
  }

  sizeType // order
  ConstantSpline::order() const { return 1 ; }

}
