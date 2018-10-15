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
  
  // Statement Function definitions
  inline
  valueType
  Extrapolate2( valueType X1,
                valueType X2,
                valueType Z0,
                valueType Z1 ) {
    return (Z1-Z0)*X2/X1 + Z0 ;
  }
  
  inline
  valueType
  Extrapolate3( valueType X1,
                valueType X2,
                valueType X3,
                valueType Z0,
                valueType Z1,
                valueType Z2 ) {
    return ( (Z2-Z0) * (X3-X1)/X2 - (Z1-Z0) * (X3-X2)/X1 ) * (X3/(X2-X1)) + Z0 ;
  }

  static
  void
  estimate( valueType   z0,
            valueType   z1,
            valueType   z2,
            valueType   z3,
            valueType   x1,
            valueType   x2,
            valueType   x3,
            valueType & c1,
            valueType & c2,
            valueType & c3,
            valueType & SX,
            valueType & SXX,
            valueType & b0,
            valueType & b1,
            valueType   RF[2],
            valueType   RI[2] ) {

    // Primary estimate of partial derivative zx as the coefficient of the bicubic polynomial.
    c1 = x2*x3/((x1-x2)*(x1-x3)) ;
    c2 = x3*x1/((x2-x3)*(x2-x1)) ;
    c3 = x1*x2/((x3-x1)*(x3-x2)) ;
    valueType primary_estimate = c1*(z1-z0)/x1 + c2*(z2-z0)/x2 + c3*(z3-z0)/x3 ;

    // Volatility factor and distance factor in the x direction for the primary estimate of zx.
              SX  = x1 + x2 + x3 ;
    valueType SZ  = z0 + z1 + z2 + z3 ;
              SXX = x1*x1 + x2*x2 + x3*x3 ;
    valueType SXZ = x1*z1 + z2*z2 + x3*z3 ;
    valueType DNM = 4.0*SXX - SX*SX ;
              b0  = (SXX*SZ-SX*SXZ)/DNM ;
              b1  = (4.0*SXZ-SX*SZ)/DNM ;
    valueType dz0 = z0 - b0 ;
    valueType dz1 = z1 - (b0+b1*x1) ;
    valueType dz2 = z2 - (b0+b1*x2) ;
    valueType dz3 = z3 - (b0+b1*x3) ;
    valueType volatility_factor = dz0*dz0 + dz1*dz1 + dz2*dz2 + dz3*dz3 ;
    // epsi value used to decide whether or not the volatility factor is essentially zero.
    valueType epsi = (z0*z0+z1*z1+z2*z2+z3*z3)*1.0E-12 ;
    // Accumulates the weighted primary estimates and their weights.
    if ( volatility_factor > epsi ) { // finite weight.
      valueType WT = 1.0/ (volatility_factor*SXX) ;
      RF[1] += WT*primary_estimate ;
      RF[0] += WT ;
    } else { // infinite weight.
      RI[1] += primary_estimate ;
      RI[0] += 1.0 ;
    }
  }
  
  // 0 1 2 3 (4) 5 6 7 8
  static
  void
  AkimaSmooth( valueType const X[9], sizeType imin, sizeType imax,
               valueType const Y[9], sizeType jmin, sizeType jmax,
               valueType const Z[9][9],
               valueType & DX,
               valueType & DY,
               valueType & DXY ) {

    int stencil[3][4] = { { -3, -2, -1, 1 },
                          { -2, -1,  1, 2 },
                          { -1,  1,  2, 3 } } ;

    valueType by0[4], by1[4], CY1A[4], CY2A[4], CY3A[4], SYA[4], SYYA[4] ;

    valueType X0  = X[4] ;
    valueType Y0  = Y[4] ;
    valueType Z00 = Z[4][4] ;

    // Initial setting
    valueType DXF[2]  = {0,0} ;
    valueType DXI[2]  = {0,0} ;
    valueType DYF[2]  = {0,0} ;
    valueType DYI[2]  = {0,0} ;
    valueType DXYF[2] = {0,0} ;
    valueType DXYI[2] = {0,0} ;

    for ( int k = 0 ; k < 4 ; ++k ) {
      int j1 = 4 + stencil[0][k] ;
      int j2 = 4 + stencil[1][k] ;
      int j3 = 4 + stencil[2][k] ;
      if ( j1 >= int(jmin) && j3 <= int(jmax) )
        estimate( Z00, Z[4][j1], Z[4][j2], Z[4][j3],
                  Y[j1] - Y0, Y[j2] - Y0, Y[j3] - Y0,
                  CY1A[k], CY2A[k], CY3A[k], SYA[k], SYYA[k],
                  by0[k], by1[k], DYF, DYI ) ;
    }

    for ( int kx = 0 ; kx < 4 ; ++kx ) {
      int i1 = 4 + stencil[0][kx] ;
      int i2 = 4 + stencil[1][kx] ;
      int i3 = 4 + stencil[2][kx] ;

      if ( i1 < int(imin) || i3 > int(imax) ) continue ;

      valueType X1  = X[i1] - X0 ;
      valueType X2  = X[i2] - X0 ;
      valueType X3  = X[i3] - X0 ;
      valueType Z10 = Z[i1][4] ;
      valueType Z20 = Z[i2][4] ;
      valueType Z30 = Z[i3][4] ;
      valueType CX1, CX2, CX3, SX, SXX, B00X, B10 ;
      estimate( Z00, Z10, Z20, Z30, X1, X2, X3, CX1, CX2, CX3, SX, SXX, B00X, B10, DXF, DXI ) ;

      for ( int ky = 0 ; ky < 4 ; ++ky ) {
        int j1 = 4 + stencil[0][ky] ;
        int j2 = 4 + stencil[1][ky] ;
        int j3 = 4 + stencil[2][ky] ;
        if ( j1 < int(jmin) || j3 > int(jmax) ) continue ;

        valueType Y1   = Y[j1] - Y0 ;
        valueType Y2   = Y[j2] - Y0 ;
        valueType Y3   = Y[j3] - Y0 ;
        valueType CY1  = CY1A[ky] ;
        valueType CY2  = CY2A[ky] ;
        valueType CY3  = CY3A[ky] ;
        valueType SY   = SYA[ky] ;
        valueType SYY  = SYYA[ky] ;
        valueType B00Y = by0[ky] ;
        valueType B01  = by1[ky] ;

        valueType Z01 = Z[4][j1],  Z02 = Z[4][j2],  Z03 = Z[4][j3] ;
        valueType Z11 = Z[i1][j1], Z12 = Z[i1][j2], Z13 = Z[i1][j3] ;
        valueType Z21 = Z[i2][j1], Z22 = Z[i2][j2], Z23 = Z[i2][j3] ;
        valueType Z31 = Z[i3][j1], Z32 = Z[i3][j2], Z33 = Z[i3][j3] ;

        // Primary estimate of partial derivative zxy as the coefficient of the bicubic polynomial.
        valueType DZXY11 = (Z11-Z10-Z01+Z00)/(X1*Y1);
        valueType DZXY12 = (Z12-Z10-Z02+Z00)/(X1*Y2);
        valueType DZXY13 = (Z13-Z10-Z03+Z00)/(X1*Y3);
        valueType DZXY21 = (Z21-Z20-Z01+Z00)/(X2*Y1);
        valueType DZXY22 = (Z22-Z20-Z02+Z00)/(X2*Y2);
        valueType DZXY23 = (Z23-Z20-Z03+Z00)/(X2*Y3);
        valueType DZXY31 = (Z31-Z30-Z01+Z00)/(X3*Y1);
        valueType DZXY32 = (Z32-Z30-Z02+Z00)/(X3*Y2);
        valueType DZXY33 = (Z33-Z30-Z03+Z00)/(X3*Y3);
        valueType PEZXY  = CX1* (CY1*DZXY11+CY2*DZXY12+CY3*DZXY13) +
                           CX2* (CY1*DZXY21+CY2*DZXY22+CY3*DZXY23) +
                           CX3* (CY1*DZXY31+CY2*DZXY32+CY3*DZXY33) ;
        // Volatility factor and distance factor in the x and y directions for the primary estimate of zxy.
        valueType B00   = (B00X+B00Y)/2.0 ;
        valueType SXY   = SX*SY ;
        valueType SXXY  = SXX*SY ;
        valueType SXYY  = SX*SYY ;
        valueType SXXYY = SXX*SYY ;
        valueType SXYZ  = X1 * (Y1*Z11+Y2*Z12+Y3*Z13) +
                          X2 * (Y1*Z21+Y2*Z22+Y3*Z23) +
                          X3 * (Y1*Z31+Y2*Z32+Y3*Z33) ;
        valueType B11  = (SXYZ-B00*SXY-B10*SXXY-B01*SXYY)/SXXYY ;
        valueType DZ00 = Z00 - B00 ;
        valueType DZ01 = Z01 - (B00+B01*Y1) ;
        valueType DZ02 = Z02 - (B00+B01*Y2) ;
        valueType DZ03 = Z03 - (B00+B01*Y3) ;
        valueType DZ10 = Z10 - (B00+B10*X1) ;
        valueType DZ11 = Z11 - (B00+B01*Y1+X1*(B10+B11*Y1)) ;
        valueType DZ12 = Z12 - (B00+B01*Y2+X1*(B10+B11*Y2)) ;
        valueType DZ13 = Z13 - (B00+B01*Y3+X1*(B10+B11*Y3)) ;
        valueType DZ20 = Z20 - (B00+B10*X2) ;
        valueType DZ21 = Z21 - (B00+B01*Y1+X2*(B10+B11*Y1)) ;
        valueType DZ22 = Z22 - (B00+B01*Y2+X2*(B10+B11*Y2)) ;
        valueType DZ23 = Z23 - (B00+B01*Y3+X2*(B10+B11*Y3)) ;
        valueType DZ30 = Z30 - (B00+B10*X3) ;
        valueType DZ31 = Z31 - (B00+B01*Y1+X3*(B10+B11*Y1)) ;
        valueType DZ32 = Z32 - (B00+B01*Y2+X3*(B10+B11*Y2)) ;
        valueType DZ33 = Z33 - (B00+B01*Y3+X3*(B10+B11*Y3)) ;
        valueType volatility_factor = DZ00*DZ00 + DZ01*DZ01 +
                                      DZ02*DZ02 + DZ03*DZ03 +
                                      DZ10*DZ10 + DZ11*DZ11 +
                                      DZ12*DZ12 + DZ13*DZ13 +
                                      DZ20*DZ20 + DZ21*DZ21 +
                                      DZ22*DZ22 + DZ23*DZ23 +
                                      DZ30*DZ30 + DZ31*DZ31 +
                                      DZ32*DZ32 + DZ33*DZ33 ;
        valueType epsi = (Z00*Z00+Z01*Z01+Z02*Z02+Z03*Z03+
                          Z10*Z10+Z11*Z11+Z12*Z12+Z13*Z13+
                          Z20*Z20+Z21*Z21+Z22*Z22+Z23*Z23+
                          Z30*Z30+Z31*Z31+Z32*Z32+Z33*Z33)*1.0E-12 ;
        // Accumulates the weighted primary estimates of zxy and their weights.
        if ( volatility_factor > epsi ) { // finite weight.
          valueType WT = 1 / (volatility_factor*SXX*SYY) ;
          DXYF[1] += WT*PEZXY ;
          DXYF[0] += WT ;
        } else { // infinite weight.
          DXYI[1] += PEZXY ;
          DXYI[0] += 1.0 ;
        }
      }
    }
    DX  = DXI[0]  < 0.5 ? DXF[1]/DXF[0]   : DXI[1]/DXI[0]   ;
    DY  = DYI[0]  < 0.5 ? DYF[1]/DYF[0]   : DYI[1]/DYI[0]   ;
    DXY = DXYI[0] < 0.5 ? DXYF[1]/DXYF[0] : DXYI[1]/DXYI[0] ;
  }
    
  /*
   * This subroutine estimates three partial derivatives, zx, zy, and
   * zxy, of a bivariate function, z(x,y), on a rectangular grid in
   * the x-y plane.  It is based on the revised Akima method that has
   * the accuracy of a bicubic polynomial.
   */
  void
  Akima2Dspline::makeSpline() {
    DX.resize(Z.size()) ;
    DY.resize(Z.size()) ;
    DXY.resize(Z.size()) ;
    // calcolo derivate
    sizeType nx = sizeType(X.size()) ;
    sizeType ny = sizeType(Y.size()) ;
    
    std::fill(DX.begin(),DX.end(),0) ;
    std::fill(DY.begin(),DY.end(),0) ;
    std::fill(DXY.begin(),DXY.end(),0) ;
    
    valueType x_loc[9], y_loc[9], z_loc[9][9] ;

    for ( sizeType i0 = 0 ; i0 < nx ; ++i0 ) {
      sizeType imin = 4  > i0   ? 4-i0      : 0 ;
      sizeType imax = nx < 5+i0 ? 3+(nx-i0) : 8 ;

      for ( sizeType i = imin ; i <= imax ; ++i ) x_loc[i] = X[i+i0-4]-X[i0] ;

      for ( sizeType j0 = 0 ; j0 < ny ; ++j0 ) {
        sizeType jmin = 4 > j0    ? 4-j0      : 0 ;
        sizeType jmax = ny < 5+j0 ? 3+(ny-j0) : 8 ;

        for ( sizeType j = jmin ; j <= jmax ; ++j ) y_loc[j] = Y[j+j0-4]-Y[j0] ;

        for ( sizeType i = imin ; i <= imax ; ++i )
          for ( sizeType j = jmin ; j <= jmax ; ++j )
            z_loc[i][j] = Z[ipos_C(i+i0-4,j+j0-4,ny)] ;

        // if not enough points, extrapolate
        sizeType iadd = 0, jadd = 0 ;
        if ( imax < 3+imin ) {
          x_loc[imin-1] = 2*x_loc[imin] - x_loc[imax] ;
          x_loc[imax+1] = 2*x_loc[imax] - x_loc[imin] ;
          iadd = 1 ;
          if ( imax == 1+imin ) {
            valueType x0 = x_loc[imin] ;
            valueType x1 = x_loc[imax] ;
            for ( size_t j = jmin ; j <= jmax ; ++j ) {
              valueType z0 = z_loc[imin][j] ;
              valueType z1 = z_loc[imax][j] ;
              z_loc[imin-1][j] = Extrapolate2( x0-x1, x_loc[imin-1]-x1, z1, z0 ) ;
              z_loc[imax+1][j] = Extrapolate2( x1-x0, x_loc[imax+1]-x0, z0, z1 ) ;
            }
          } else {
            valueType x0 = x_loc[imin] ;
            valueType x1 = x_loc[imin+1] ;
            valueType x2 = x_loc[imax] ;
            for ( size_t j = jmin ; j <= jmax ; ++j ) {
              valueType z0 = z_loc[imin][j] ;
              valueType z1 = z_loc[imin+1][j] ;
              valueType z2 = z_loc[imax][j] ;
              z_loc[imin-1][j] = Extrapolate3( x1-x2, x0-x2, x_loc[imin-1]-x1, z2, z1, z0 ) ;
              z_loc[imax+1][j] = Extrapolate3( x1-x0, x2-x0, x_loc[imax+1]-x0, z0, z1, z2 ) ;
            }
          }
        }
        if ( jmax < 3+jmin ) {
          y_loc[jmin-1] = 2*y_loc[jmin] - y_loc[jmax] ;
          y_loc[jmax+1] = 2*y_loc[jmax] - y_loc[jmin] ;
          jadd = 1 ;
          if ( jmax-jmin == 1 ) {
            valueType y0 = y_loc[jmin] ;
            valueType y1 = y_loc[jmax] ;
            for ( sizeType i = imin-iadd ; i <= imax+iadd ; ++i ) {
              valueType z0 = z_loc[i][jmin] ;
              valueType z1 = z_loc[i][jmax] ;
              z_loc[i][jmin-1] = Extrapolate2( y0-y1, y_loc[jmin-1]-y1, z1, z0 ) ;
              z_loc[i][jmax+1] = Extrapolate2( y1-y0, y_loc[jmax+1]-y0, z0, z1 ) ;
            }
          } else {
            valueType y0 = y_loc[jmin] ;
            valueType y1 = y_loc[jmin+1] ;
            valueType y2 = y_loc[jmax] ;
            for ( sizeType i = imin-iadd ; i <= imax+iadd ; ++i ) {
              valueType z0 = z_loc[i][jmin] ;
              valueType z1 = z_loc[i][jmin+1] ;
              valueType z2 = z_loc[i][jmax] ;
              z_loc[i][imin-1] = Extrapolate3( y1-y2, y0-y2, y_loc[jmin-1]-y1, z2, z1, z0 ) ;
              z_loc[i][imax+1] = Extrapolate3( y1-y0, y2-y0, y_loc[jmax+1]-y0, z0, z1, z2 ) ;
            }
          }
        }

        AkimaSmooth( x_loc, imin-iadd, imax+iadd,
                     y_loc, jmin-jadd, jmax+jadd,
                     z_loc,
                     DX[ipos_C(i0,j0)],
                     DY[ipos_C(i0,j0)],
                     DXY[ipos_C(i0,j0)] ) ;
      }
    }
  }

  void
  Akima2Dspline::writeToStream( ostream & s ) const {
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
  Akima2Dspline::type_name() const
  { return "Akima2D" ; }

}
