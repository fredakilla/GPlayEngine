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

// Test problem for Akima interpolation
// Ref. : Hiroshi Akima, Journal of the ACM, Vol. 17, No. 4, October 1970, pages 589-602.

static valueType xx0[] = {  0,  1,  2,  3,  4,  5,    6,  7,  8,  9, 10 } ; 
static valueType yy0[] = { 10, 10, 10, 10, 10, 10, 10.5, 15, 50, 60, 85 } ;

static valueType xx1[] = {  0,  1,  3,  4,  6,  7,    9, 10, 12, 13, 15 } ;
static valueType yy1[] = { 10, 10, 10, 10, 10, 10, 10.5, 15, 50, 60, 85 } ;

static valueType xx2[] = {  0,  2,  3,  5,  6,  8,    9, 11, 12, 14, 15 } ;
static valueType yy2[] = { 10, 10, 10, 10, 10, 10, 10.5, 15, 50, 60, 85 } ;

// RPN 14
static valueType xx3[] = { 7.99, 8.09,       8.19,       8.7,      9.2,      10,       12,       15,       20       } ;
static valueType yy3[] = { 0,    2.76429e-5, 4.37498e-2, 0.169183, 0.469428, 0.943740, 0.998636, 0.999919, 0.999994 } ;

// Titanium
static valueType xx4[] = { 595,   635,   695,   795,   855,   875,   895,   915,   935,   985,   1035,  1075  } ;
static valueType yy4[] = { 0.644, 0.652, 0.644, 0.694, 0.907, 1.336, 2.169, 1.598, 0.916, 0.607, 0.603, 0.608 } ;

// toolpath
static valueType xx5[] = { 0.11, 0.12, 0.15, 0.16 } ;
static valueType yy5[] = { 0.0003, 0.0003, 0.0004, 0.0004 } ;

static sizeType  n[]   = { 11, 11, 11, 9, 12, 4 } ;

int
main() {

  LinearSpline   li ;
  ConstantSpline co ;
  AkimaSpline    ak ;
  CubicSpline    cs ;
  BesselSpline   be ;
  PchipSpline    pc ;
  QuinticSpline  qs ;
  ofstream       file_li ;
  ofstream       file_co ;
  ofstream       file_ak ;
  ofstream       file_cs ;
  ofstream       file_be ;
  ofstream       file_pc ;
  ofstream       file_qs ;

  for ( indexType k = 0 ; k < 6 ; ++ k ) {
    cout << "\n\nk = " << k << '\n' ;
    valueType * xx = nullptr, * yy = nullptr ;
    switch ( k ) {
      case 0: xx = xx0 ; yy = yy0 ; break ;
      case 1: xx = xx1 ; yy = yy1 ; break ;
      case 2: xx = xx2 ; yy = yy2 ; break ;
      case 3: xx = xx3 ; yy = yy3 ; break ;
      case 4: xx = xx4 ; yy = yy4 ; break ;
      case 5: xx = xx5 ; yy = yy5 ; break ;
    }
    char fname[100] ;
    sprintf( fname, "out/Linear%d.txt",   k) ; file_li.open(fname) ;
    sprintf( fname, "out/Constant%d.txt", k) ; file_co.open(fname) ;
    sprintf( fname, "out/Akima%d.txt",    k) ; file_ak.open(fname) ;
    sprintf( fname, "out/Cubic%d.txt",    k) ; file_cs.open(fname) ;
    sprintf( fname, "out/Bessel%d.txt",   k) ; file_be.open(fname) ;
    sprintf( fname, "out/Pchip%d.txt",    k) ; file_pc.open(fname) ;
    sprintf( fname, "out/Quintic%d.txt",  k) ; file_qs.open(fname) ;
    valueType xmin = xx[0] ;
    valueType xmax = xx[n[k]-1] ;

    #define SAVE(S) \
    cout << #S": n[k] = " << n[k] << '\n' ; \
    S.clear() ; \
    S.reserve(n[k]) ; \
    for ( indexType i = 0 ; i < n[k] ; ++i ) S.pushBack(xx[i],yy[i]) ; \
    S.build() ; /*( xx, yy, n[k] ) ;*/ \
    cout << #S": xMin    = " << S.xMin()   << '\n' ; \
    cout << #S": xMax    = " << S.xMax()   << '\n' ; \
    cout << #S": xx[0]   = " << xx[0]      << '\n' ; \
    cout << #S": xx[end] = " << xx[n[k]-1] << '\n' ; \
    file_##S << "x\ty\tDy\tDDy\n" ; \
    for ( valueType x = xmin-(xmax-xmin)*0.01 ; x <= xmax+(xmax-xmin)*0.01 ; x += (xmax-xmin)/1000 ) \
      file_##S << x << '\t' << S(x) << '\t' << S.D(x) << '\t' << S.DD(x) << '\n' ; \
    file_##S.close()
    
    SAVE(li) ;
    SAVE(co) ;
    SAVE(ak) ;
    SAVE(cs) ;
    SAVE(be) ;
    SAVE(pc) ;
    SAVE(qs) ;
  }
  cout << "ALL DONE!\n" ;
}
