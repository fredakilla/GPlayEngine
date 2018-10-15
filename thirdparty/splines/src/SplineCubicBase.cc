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

  using namespace std ; // load standard namespace

  // build spline without computation

  void
  CubicSplineBase::build ( valueType const x[],  sizeType incx,
                           valueType const y[],  sizeType incy,
                           valueType const yp[], sizeType incyp,
                           sizeType n ) {
    reserve( n ) ;
    for ( sizeType i = 0 ; i < n ; ++i ) {
      X[i]  = x[i*incx] ;
      Y[i]  = y[i*incy] ;
      Yp[i] = yp[i*incyp] ;
    }
    npts = n ;
  }
  
  void
  CubicSplineBase::clear(void) {
    if ( !_external_alloc ) baseValue.free() ;
    npts = npts_reserved = 0 ;
    _external_alloc = false ;
    X = Y = Yp = nullptr ;
  }

  void
  CubicSplineBase::reserve( sizeType n ) {
    if ( _external_alloc && n <= npts_reserved ) {
      // nothing to do!, already allocated
    } else {
      npts_reserved = n ;
      baseValue.allocate(3*n) ;
      X  = baseValue(n) ;
      Y  = baseValue(n) ;
      Yp = baseValue(n) ;
      _external_alloc = false ;
    }
    npts = lastInterval = 0 ;
  }

  void
  CubicSplineBase::reserve_external( sizeType     n,
                                     valueType *& p_x,
                                     valueType *& p_y,
                                     valueType *& p_dy ) {
    npts_reserved = n ;
    X    = p_x ;
    Y    = p_y ;
    Yp   = p_dy ;
    npts = lastInterval = 0 ;
    _external_alloc = true ;
  }

  valueType
  CubicSplineBase::operator () ( valueType x ) const { 
    sizeType i = Spline::search( x ) ;
    Hermite3( x-X[i], X[i+1]-X[i], base ) ;
    return base[0] * Y[i]   +
           base[1] * Y[i+1] +
           base[2] * Yp[i]  +
           base[3] * Yp[i+1] ;
  }

  valueType
  CubicSplineBase::D( valueType x ) const {
    sizeType i = Spline::search( x ) ;
    Hermite3_D( x-X[i], X[i+1]-X[i], base_D ) ;
    return base_D[0] * Y[i]   +
           base_D[1] * Y[i+1] +
           base_D[2] * Yp[i]  +
           base_D[3] * Yp[i+1] ;
  }

  valueType
  CubicSplineBase::DD( valueType x ) const {
    sizeType i = Spline::search( x ) ;
    Hermite3_DD( x-X[i], X[i+1]-X[i], base_DD ) ;
    return base_DD[0] * Y[i]   +
           base_DD[1] * Y[i+1] +
           base_DD[2] * Yp[i]  +
           base_DD[3] * Yp[i+1] ;
  }

  valueType
  CubicSplineBase::DDD( valueType x ) const { 
    sizeType i = Spline::search( x ) ;
    Hermite3_DDD( x-X[i], X[i+1]-X[i], base_DDD ) ;
    return base_DDD[0] * Y[i]   +
           base_DDD[1] * Y[i+1] +
           base_DDD[2] * Yp[i]  +
           base_DDD[3] * Yp[i+1] ;
  }

  sizeType // order
  CubicSplineBase::coeffs( valueType cfs[], valueType nodes[], bool transpose ) const {
    sizeType n = npts > 0 ? npts-1 : 0 ;
    for ( sizeType i = 0 ; i < n ; ++i ) {
      nodes[i] = X[i] ;
      valueType H  = X[i+1]-X[i] ;
      valueType DY = (Y[i+1]-Y[i])/H ;
      valueType a = Y[i] ;
      valueType b = Yp[i] ;
      valueType c = (3*DY-2*Yp[i]-Yp[i+1])/H;
      valueType d = (Yp[i+1]+Yp[i]-2*DY)/(H*H) ;
      if ( transpose ) {
        cfs[4*i+3] = a ;
        cfs[4*i+2] = b ;
        cfs[4*i+1] = c ;
        cfs[4*i+0] = d ;
      } else {
        cfs[i+3*n] = a ;
        cfs[i+2*n] = b ;
        cfs[i+1*n] = c ;
        cfs[i+0*n] = d ;
      }
    }
    return 4 ;
  }
  
  sizeType
  CubicSplineBase::order() const { return 4 ; }

  // Implementation
  void
  CubicSplineBase::copySpline( CubicSplineBase const & S ) {
    CubicSplineBase::reserve(S.npts) ;
    npts = S.npts ;
    std::copy( S.X, S.X+npts, X ) ;
    std::copy( S.Y, S.Y+npts, Y ) ;
    std::copy( S.Yp, S.Yp+npts, Yp ) ;
  }

  //! change X-range of the spline
  void
  CubicSplineBase::setRange( valueType xmin, valueType xmax ) {
    Spline::setRange( xmin, xmax ) ;
    valueType recS = ( X[npts-1] - X[0] ) / (xmax - xmin) ;
    valueType * iy = Y ;
    while ( iy < Y + npts ) *iy++ *= recS ;
  }

  void
  CubicSplineBase::writeToStream( std::basic_ostream<char> & s ) const {
    sizeType nseg = npts > 0 ? npts - 1 : 0 ;
    for ( sizeType i = 0 ; i < nseg ; ++i )
      s << "segment N." << setw(4) << i
        << " X:[" << X[i] << ", " << X[i+1]
        << "] Y:[" << Y[i] << ", " << Y[i+1] 
        << "] Yp:[" << Yp[i] << ", " << Yp[i+1] 
        << "] slope: " << (Y[i+1]-Y[i])/(X[i+1]-X[i])
        << '\n' ; 
  }

}
