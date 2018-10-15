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
#include <iomanip>
/**
 * 
 */

namespace Splines {

  using namespace std ; // load standard namspace

  valueType
  BilinearSpline::operator () ( valueType x, valueType y ) const {
    sizeType  i   = search_x( x ) ;
    sizeType  j   = search_y( y ) ;
    valueType DX  = X[i+1] - X[i] ;
    valueType DY  = Y[j+1] - Y[j] ;
    valueType u   = (x-X[i])/DX ;
    valueType v   = (y-Y[j])/DY ;
    valueType u1  = 1-u ;
    valueType v1  = 1-v ;
    valueType Z00 = Z[ipos_C(i,j)] ;
    valueType Z01 = Z[ipos_C(i,j+1)] ;
    valueType Z10 = Z[ipos_C(i+1,j)] ;
    valueType Z11 = Z[ipos_C(i+1,j+1)] ;
    return u1 * ( Z00 * v1 + Z01 * v ) +
           u  * ( Z10 * v1 + Z11 * v ) ;
  }

  valueType
  BilinearSpline::Dx( valueType x, valueType y ) const {
    sizeType  i   = search_x( x ) ;
    sizeType  j   = search_y( y ) ;
    valueType DX  = X[i+1] - X[i] ;
    valueType DY  = Y[j+1] - Y[j] ;
    valueType v   = (y-Y[j])/DY ;
    valueType Z00 = Z[ipos_C(i,j)] ;
    valueType Z01 = Z[ipos_C(i,j+1)] ;
    valueType Z10 = Z[ipos_C(i+1,j)] ;
    valueType Z11 = Z[ipos_C(i+1,j+1)] ;
    return ( (Z10-Z00) * (1-v) + (Z11-Z01) * v ) / DX ;
  }
  
  valueType
  BilinearSpline::Dy( valueType x, valueType y ) const {
    sizeType  i   = search_x( x ) ;
    sizeType  j   = search_y( y ) ;
    valueType DX  = X[i+1] - X[i] ;
    valueType DY  = Y[j+1] - Y[j] ;
    valueType u   = (x-X[i])/DX ;
    valueType Z00 = Z[ipos_C(i,j)] ;
    valueType Z01 = Z[ipos_C(i,j+1)] ;
    valueType Z10 = Z[ipos_C(i+1,j)] ;
    valueType Z11 = Z[ipos_C(i+1,j+1)] ;
    return ( ( Z01-Z00 ) * (1-u) + ( Z11-Z10 ) * u ) / DY ;

  }

  void
  BilinearSpline::D( valueType x, valueType y, valueType d[3] ) const {
    sizeType  i   = search_x( x ) ;
    sizeType  j   = search_y( y ) ;
    valueType DX  = X[i+1] - X[i] ;
    valueType DY  = Y[j+1] - Y[j] ;
    valueType u   = (x-X[i])/DX ;
    valueType v   = (y-Y[j])/DY ;
    valueType u1  = 1-u ;
    valueType v1  = 1-v ;
    valueType Z00 = Z[ipos_C(i,j)] ;
    valueType Z01 = Z[ipos_C(i,j+1)] ;
    valueType Z10 = Z[ipos_C(i+1,j)] ;
    valueType Z11 = Z[ipos_C(i+1,j+1)] ;
    d[0] = u1 * ( Z00 * v1 + Z01 * v ) + u * ( Z10 * v1 + Z11 * v ) ;
    d[1] = v1 * (Z10-Z00) + v * (Z11-Z01) ; d[1] /= DX ;
    d[2] = u1 * (Z01-Z00) + u * (Z11-Z10) ; d[2] /= DY ;
  }

  void
  BilinearSpline::writeToStream( ostream & s ) const {
    sizeType ny = sizeType(Y.size()) ;
    s << "Nx = " << X.size() << " Ny = " << Y.size() << '\n' ;
    for ( sizeType i = 1 ; i < sizeType(X.size()) ; ++i ) {
      for ( sizeType j = 1 ; j < sizeType(Y.size()) ; ++j ) {
        sizeType i00 = ipos_C(i-1,j-1,ny) ;
        sizeType i10 = ipos_C(i,j-1,ny) ;
        sizeType i01 = ipos_C(i-1,j,ny) ;
        sizeType i11 = ipos_C(i,j,ny) ;
        s << "patch (" << i << "," << j << ")\n"
          <<  "DX = " << setw(10) << left << X[i]-X[i-1]
          << " DY = " << setw(10) << left << Y[j]-Y[j-1]
          << " Z00 = " << setw(10) << left << Z[i00]
          << " Z01 = " << setw(10) << left << Z[i01]
          << " Z10 = " << setw(10) << left << Z[i10]
          << " Z11 = " << setw(10) << left << Z[i11]
          << '\n' ;
      }
    }
  }

  char const *
  BilinearSpline::type_name() const
  { return "bilinear" ; }

}
