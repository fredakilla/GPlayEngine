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
  
  void
  BiCubicSpline::makeSpline() {
    DX.resize(Z.size()) ;
    DY.resize(Z.size()) ;
    DXY.resize(Z.size()) ;
    // calcolo derivate
    sizeType nx = sizeType(X.size()) ;
    sizeType ny = sizeType(Y.size()) ;
    PchipSpline sp ;
    for ( sizeType j = 0 ; j < ny ; ++j ) {
      sp.build( &X.front(), 1, &Z[ipos_C(0,j)], ny, nx ) ;
      for ( sizeType i = 0 ; i < nx ; ++i ) DX[ipos_C(i,j)] = sp.ypNode(i) ;
    }
    for ( sizeType i = 0 ; i < nx ; ++i ) {
      sp.build( &Y.front(), 1, &Z[ipos_C(i,0)], 1, ny ) ;
      for ( sizeType j = 0 ; j < ny ; ++j ) DY[ipos_C(i,j)] = sp.ypNode(j) ;
    }
    std::fill( DXY.begin(), DXY.end(), 0 ) ;
  }

  void
  BiCubicSpline::writeToStream( ostream & s ) const {
    sizeType ny = sizeType(Y.size()) ;
    s << "Nx = " << X.size() << " Ny = " << Y.size() << '\n' ;
    for ( sizeType i = 1 ; i < sizeType(X.size()) ; ++i ) {
      for ( sizeType j = 1 ; j < sizeType(Y.size()) ; ++j ) {
        sizeType i00 = ipos_C(i-1,j-1,ny) ;
        sizeType i10 = ipos_C(i,j-1,ny) ;
        sizeType i01 = ipos_C(i-1,j,ny) ;
        sizeType i11 = ipos_C(i,j,ny) ;
        s << "patch (" << i << "," << j
          << ")\n DX = " << setw(10) << left << X[i]-X[i-1]
          <<    " DY = " << setw(10) << left << Y[j]-Y[j-1]
          << "\n Z00  = " << setw(10) << left << Z[i00]
          <<   " Z01  = " << setw(10) << left << Z[i01]
          <<   " Z10  = " << setw(10) << left << Z[i10]
          <<   " Z11  = " << setw(10) << left << Z[i11]
          << "\n Dx00 = " << setw(10) << left << DX[i00]
          <<   " Dx01 = " << setw(10) << left << DX[i01]
          <<   " Dx10 = " << setw(10) << left << DX[i10]
          <<   " Dx11 = " << setw(10) << left << DX[i11]
          << "\n Dy00 = " << setw(10) << left << DY[i00]
          <<   " Dy01 = " << setw(10) << left << DY[i01]
          <<   " Dy10 = " << setw(10) << left << DY[i10]
          <<   " Dy11 = " << setw(10) << left << DY[i11]
          << '\n' ;
      }
    }
  }

  char const *
  BiCubicSpline::type_name() const
  { return "BiCubic" ; }

}
