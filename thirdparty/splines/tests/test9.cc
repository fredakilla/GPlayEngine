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
using namespace GenericContainerNamespace ;
using Splines::valueType ;
using Splines::indexType ;
using Splines::sizeType ;

int
main() {
  
  unsigned npts = 10 ;
  unsigned nspl = 4 ;
  
  GenericContainer gc ;
  
  vec_real_type   & X           = gc["xdata"].set_vec_real() ;
  vec_string_type & spline_type = gc["spline_type"].set_vec_string() ;
  vec_string_type & headers     = gc["headers"].set_vec_string() ;
  mat_real_type   & Y           = gc["ydata"].set_mat_real(npts,nspl) ;
  map_type        & Yp          = gc["ypdata"].set_map() ;
  
  headers.push_back("sp1") ;
  headers.push_back("sp2") ;
  headers.push_back("sp3") ;
  headers.push_back("sp4") ;

  spline_type.push_back("cubic") ;
  spline_type.push_back("akima") ;
  spline_type.push_back("bessel") ;
  spline_type.push_back("hermite") ;
  
  X.push_back(0) ;
  X.push_back(1) ;
  X.push_back(2) ;
  X.push_back(3) ;
  X.push_back(4) ;
  X.push_back(4.1) ;
  X.push_back(4.2) ;
  X.push_back(5) ;
  X.push_back(6) ;
  X.push_back(7) ;
  
  vec_real_type & yp = Yp["sp4"].set_vec_real() ;

  yp.push_back(0) ;
  yp.push_back(1) ;
  yp.push_back(2) ;
  yp.push_back(3) ;
  yp.push_back(4) ;
  yp.push_back(4.1) ;
  yp.push_back(4.2) ;
  yp.push_back(5) ;
  yp.push_back(6) ;
  yp.push_back(7) ;
  
  SplineSet ss ;

  // check constructor
  ss.build(gc) ;
  
  cout << "ALL DONE!\n" ;
}
