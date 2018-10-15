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
#include <fstream>

#ifdef __GCC__
#pragma GCC diagnostic ignored "-Wc++98-compat"
#endif
#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++98-compat"
#endif

using namespace SplinesLoad ;
using namespace std ;
using Splines::valueType ;
using Splines::indexType ;
using Splines::sizeType ;

static valueType x[] = {  0,  1,  2,  3,  4,  5 } ;
static valueType y[] = {  0,  1,  2,  3,  4,  5 } ;
static valueType z[] = {  10, 10, 10,   10,  10, 11,
                          10, 10, 10.5, 11,  9,  9,
                          11, 12, 13,   8,   7,  9,
                          11, 12, 13,   8,   7,  9,
                          11, 12, 13,   8,   7,  9,
                          11, 12, 13,   8,   7,  9 } ;

int
main() {

  BiCubicSpline   bc ;
  BiQuinticSpline bq ;
  BilinearSpline  bl ;
  Akima2Dspline   ak ;
  
  vector<valueType> X(4), Y(6), Z(6*6) ;
  
  std::copy( x, x+4,   X.begin() ) ;
  std::copy( y, y+6,   Y.begin() ) ;
  std::copy( z, z+6*6, Z.begin() ) ;
  
  //bc.build( x, 1, y, 1, z, 6, 4, 6 ) ;
  //bl.build( x, 1, y, 1, z, 6, 4, 6 ) ;
  //ak.build( x, 1, y, 1, z, 6, 4, 6 ) ;
  bc.build( X, Y, Z, 6 ) ;
  bq.build( X, Y, Z, 6 ) ;
  bl.build( X, Y, Z, 6 ) ;
  ak.build( X, Y, Z, 6 ) ;
  
  //bl.writeToStream( cout ) ;
  
  {
    ofstream file_bl("out/bilinear.txt") ;
    ofstream file_bc("out/bicubic.txt") ;
    ofstream file_bq("out/biquintic.txt") ;
    ofstream file_ak("out/akima2d.txt") ;

    for ( int i = 0 ; i <= 100 ; ++i ) {
      valueType xxx = bc.xMin() + (bc.xMax()-bc.xMin())*i/100.0 ;
      for ( int j = 0 ; j <= 100 ; ++j ) {
        valueType yyy = bc.yMin() + (bc.yMax()-bc.yMin())*j/100.0 ;
        file_bc << bc(xxx,yyy) << '\t' ;
        file_bq << bq(xxx,yyy) << '\t' ;
        file_bl << bl(xxx,yyy) << '\t' ;
        file_ak << ak(xxx,yyy) << '\t' ;
      }
      file_bc << '\n' ;
      file_bq << '\n' ;
      file_bl << '\n' ;
      file_ak << '\n' ;
    }

    file_bc.close() ;
    file_bq.close() ;
    file_bl.close() ;
    file_ak.close() ;
  }

  {
    ofstream file_bl("out/bilinear_Dx.txt") ;
    ofstream file_bc("out/bicubic_Dx.txt") ;
    ofstream file_bq("out/biquintic_Dx.txt") ;
    ofstream file_ak("out/akima2d_Dx.txt") ;

    for ( int i = 0 ; i <= 100 ; ++i ) {
      valueType xxx = bc.xMin() + (bc.xMax()-bc.xMin())*i/100.0 ;
      for ( int j = 0 ; j <= 100 ; ++j ) {
        valueType yyy = bc.yMin() + (bc.yMax()-bc.yMin())*j/100.0 ;
        file_bc << bc.Dx(xxx,yyy) << '\t' ;
        file_bq << bq.Dx(xxx,yyy) << '\t' ;
        file_bl << bl.Dx(xxx,yyy) << '\t' ;
        file_ak << ak.Dx(xxx,yyy) << '\t' ;
      }
      file_bc << '\n' ;
      file_bq << '\n' ;
      file_bl << '\n' ;
      file_ak << '\n' ;
    }

    file_bc.close() ;
    file_bq.close() ;
    file_bl.close() ;
    file_ak.close() ;
  }

  {
    ofstream file_bl("out/bilinear_Dxx.txt") ;
    ofstream file_bc("out/bicubic_Dxx.txt") ;
    ofstream file_bq("out/biquintic_Dxx.txt") ;
    ofstream file_ak("out/akima2d_Dxx.txt") ;

    for ( int i = 0 ; i <= 100 ; ++i ) {
      valueType xxx = bc.xMin() + (bc.xMax()-bc.xMin())*i/100.0 ;
      for ( int j = 0 ; j <= 100 ; ++j ) {
        valueType yyy = bc.yMin() + (bc.yMax()-bc.yMin())*j/100.0 ;
        file_bc << bc.Dxx(xxx,yyy) << '\t' ;
        file_bq << bq.Dxx(xxx,yyy) << '\t' ;
        file_bl << bl.Dxx(xxx,yyy) << '\t' ;
        file_ak << ak.Dxx(xxx,yyy) << '\t' ;
      }
      file_bc << '\n' ;
      file_bq << '\n' ;
      file_bl << '\n' ;
      file_ak << '\n' ;
    }

    file_bc.close() ;
    file_bq.close() ;
    file_bl.close() ;
    file_ak.close() ;
  }

  {
    ofstream file_bl("out/bilinear_Dy.txt") ;
    ofstream file_bc("out/bicubic_Dy.txt") ;
    ofstream file_bq("out/biquintic_Dy.txt") ;
    ofstream file_ak("out/akima2d_Dy.txt") ;

    for ( int i = 0 ; i <= 100 ; ++i ) {
      valueType xxx = bc.xMin() + (bc.xMax()-bc.xMin())*i/100.0 ;
      for ( int j = 0 ; j <= 100 ; ++j ) {
        valueType yyy = bc.yMin() + (bc.yMax()-bc.yMin())*j/100.0 ;
        file_bc << bc.Dy(xxx,yyy) << '\t' ;
        file_bq << bq.Dy(xxx,yyy) << '\t' ;
        file_bl << bl.Dy(xxx,yyy) << '\t' ;
        file_ak << ak.Dy(xxx,yyy) << '\t' ;
      }
      file_bc << '\n' ;
      file_bq << '\n' ;
      file_bl << '\n' ;
      file_ak << '\n' ;
    }

    file_bc.close() ;
    file_bq.close() ;
    file_bl.close() ;
    file_ak.close() ;
  }

  {
    ofstream file_bl("out/bilinear_Dyy.txt") ;
    ofstream file_bc("out/bicubic_Dyy.txt") ;
    ofstream file_bq("out/biquintic_Dyy.txt") ;
    ofstream file_ak("out/akima2d_Dyy.txt") ;

    for ( int i = 0 ; i <= 100 ; ++i ) {
      valueType xxx = bc.xMin() + (bc.xMax()-bc.xMin())*i/100.0 ;
      for ( int j = 0 ; j <= 100 ; ++j ) {
        valueType yyy = bc.yMin() + (bc.yMax()-bc.yMin())*j/100.0 ;
        file_bc << bc.Dyy(xxx,yyy) << '\t' ;
        file_bq << bq.Dyy(xxx,yyy) << '\t' ;
        file_bl << bl.Dyy(xxx,yyy) << '\t' ;
        file_ak << ak.Dyy(xxx,yyy) << '\t' ;
      }
      file_bc << '\n' ;
      file_bq << '\n' ;
      file_bl << '\n' ;
      file_ak << '\n' ;
    }

    file_bc.close() ;
    file_bq.close() ;
    file_bl.close() ;
    file_ak.close() ;
  }

  {
    ofstream file_bl("out/bilinear_Dxy.txt") ;
    ofstream file_bc("out/bicubic_Dxy.txt") ;
    ofstream file_bq("out/biquintic_Dxy.txt") ;
    ofstream file_ak("out/akima2d_Dxy.txt") ;

    for ( int i = 0 ; i <= 100 ; ++i ) {
      valueType xxx = bc.xMin() + (bc.xMax()-bc.xMin())*i/100.0 ;
      for ( int j = 0 ; j <= 100 ; ++j ) {
        valueType yyy = bc.yMin() + (bc.yMax()-bc.yMin())*j/100.0 ;
        file_bc << bc.Dxy(xxx,yyy) << '\t' ;
        file_bq << bq.Dxy(xxx,yyy) << '\t' ;
        file_bl << bl.Dxy(xxx,yyy) << '\t' ;
        file_ak << ak.Dxy(xxx,yyy) << '\t' ;
      }
      file_bc << '\n' ;
      file_bq << '\n' ;
      file_bl << '\n' ;
      file_ak << '\n' ;
    }

    file_bc.close() ;
    file_bq.close() ;
    file_bl.close() ;
    file_ak.close() ;
  }

  cout << "ALL DONE!\n" ;
}
