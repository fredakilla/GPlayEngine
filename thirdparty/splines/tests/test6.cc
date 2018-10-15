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

#include "GenericContainer.hh"
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

// monotone
static valueType xx[] = { 0, 0.9, 2.1, 3, 4.5 } ;
static valueType yy[] = { 0, 1, 1.99, 2.0, 2.1 } ;

static sizeType  n = 5 ;

int
main() {

  SplineSet ss ;
  ofstream  file, file_D, fileR, fileR_D ;

  file.open("out/SplineSet.txt") ;
  file_D.open("out/SplineSet_D.txt") ;
  fileR.open("out/SplineSetR.txt") ;
  fileR_D.open("out/SplineSetR_D.txt") ;
  
  valueType xmin = xx[0] ;
  valueType xmax = xx[n-1] ;

  sizeType  nspl = 7 ;
  sizeType  npts = n ;
  valueType val[7], val_D[7] ;

  char const *headers[] = {
    "constant",
    "linear",
    "cubic",
    "akima",
    "bessel",
    "pchip",
    "quintic"
  } ;

  valueType const *Y[] = { yy, yy, yy, yy, yy, yy, yy, yy } ;

  GC::GenericContainer gc ;

  GC::vec_string_type & t = gc["spline_type"].set_vec_string() ;
  GC::vec_string_type & h = gc["headers"].set_vec_string() ;
  t.resize(nspl) ;
  h.resize(nspl) ;
  std::copy( headers, headers+nspl, h.begin() ) ;
  std::copy( headers, headers+nspl, t.begin() ) ;

  GC::vector_type & data = gc["ydata"].set_vector() ;
  data.resize(nspl) ;
  for ( sizeType i = 0 ; i < nspl ; ++i ) {
    GC::GenericContainer & di = data[i] ;
    GC::vec_real_type    & v  = di.set_vec_real() ;
    v.resize(npts) ;
    std::copy( Y[i], Y[i]+npts, v.begin() ) ;
  }

  GC::vec_real_type & xdata = gc["xdata"].set_vec_real() ;
  xdata.resize(npts) ;
  std::copy( xx, xx+npts, xdata.begin() ) ;

  gc.print(cout) ;
  ss.build( gc ) ; // nspl, npts, headers, stype, xx, Y, Yp ) ;
  ss.info(cout) ;

  file   << "x" ;
  file_D << "x" ;
  for ( sizeType i = 0 ; i < nspl ; ++i ) {
    file   << '\t' << ss.header(i) ;
    file_D << '\t' << ss.header(i) ;
  }
  file   << '\n' ;
  file_D << '\n' ;
  for ( valueType x = xmin-(xmax-xmin)*0.01 ;
        x <= xmax+(xmax-xmin)*0.01 ;
        x += (xmax-xmin)/1000 ) {
    file   << x ;
    file_D << x ;
    ss.eval( x, val ) ;
    ss.eval_D( x, val_D ) ;
    for ( sizeType i = 0 ; i < nspl ; ++i ) {
      file   << '\t' << val[i] ;
      file_D << '\t' << val_D[i] ;
    }
    file   << '\n' ;
    file_D << '\n' ;
  }
  file.close() ;
  file_D.close() ;


  xmin = yy[0] ;
  xmax = yy[n-1] ;

  fileR   << "x" ;
  fileR_D << "x" ;
  for ( sizeType i = 0 ; i < nspl ; ++i ) {
    fileR   << '\t' << ss.header(i) ;
    fileR_D << '\t' << ss.header(i) ;
  }
  fileR   << '\n' ;
  fileR_D << '\n' ;

  for ( valueType x = xmin ; x <= xmax ; x += (xmax-xmin)/1000 ) {
    fileR   << x ;
    fileR_D << x ;
    ss.eval2( 5, x, val ) ;
    ss.eval2_D( 5, x, val_D ) ;
    for ( indexType i = 0 ; i < nspl ; ++i ) {
      fileR   << '\t' << val[i] ;
      fileR_D << '\t' << val_D[i] ;
    }
    fileR   << '\n' ;
    fileR_D << '\n' ;
  }
  fileR.close() ;
  fileR_D.close() ;

  cout << "ALL DONE!\n" ;
}
