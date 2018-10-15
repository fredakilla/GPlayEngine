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

//
// file: SplineCinterface.cc
//

/*
 \file SplineCinterface.cc
 This file contains the sources for the C interface to `Splines`
 */

#include "Splines.hh"
#include "SplinesCinterface.h"

using namespace SplinesLoad ;

#include <vector>
#include <map>
#include <string>
#include <deque>

#include <string.h>

#ifdef __GCC__
#pragma GCC diagnostic ignored "-Wc++98-compat"
#pragma GCC diagnostic ignored "-Wglobal-constructors"
#endif
#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++98-compat"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#endif

using namespace std ;

#define EXTERN_C extern "C"

typedef std::map<std::string,Spline*> MAP_SPLINE ;

static std::map<std::string,Spline*> spline_stored ;
static Spline * head = nullptr ;

EXTERN_C
int
SPLINE_new( char const id[], char const type[] ) {
  cout << "SPLINE_new, id = " << id << " type = " << type << "\n" ;
  MAP_SPLINE::iterator it = spline_stored.find(id) ;
  if ( it != spline_stored.end() ) delete it->second ;
  int ok = 0 ;
  if ( strcmp( type, Splines::spline_type[Splines::AKIMA_TYPE]) == 0 ) {
    head = spline_stored[id] = new AkimaSpline ;
  } else if ( strcmp( type, Splines::spline_type[Splines::BESSEL_TYPE]) == 0 ) {
    head = spline_stored[id] = new BesselSpline ;
  } else if ( strcmp( type, Splines::spline_type[Splines::PCHIP_TYPE]) == 0 ) {
    head = spline_stored[id] = new PchipSpline ;
  } else if ( strcmp( type, Splines::spline_type[Splines::CUBIC_TYPE] ) == 0 ) {
    head = spline_stored[id] = new CubicSpline ;
  } else if ( strcmp( type, Splines::spline_type[Splines::LINEAR_TYPE] ) == 0 ) {
    head = spline_stored[id] = new LinearSpline ;
  } else if ( strcmp( type, Splines::spline_type[Splines::CONSTANT_TYPE] ) == 0 ) {
    head = spline_stored[id] = new ConstantSpline ;
  } else if ( strcmp( type, Splines::spline_type[Splines::QUINTIC_TYPE]) == 0 ) {
    head = spline_stored[id] = new QuinticSpline ;
  } else {
    head = nullptr ;
    ok = -1 ;
  }
  return ok ;
}

EXTERN_C
int
SPLINE_select( char const id[] ) {
  MAP_SPLINE::iterator it = spline_stored.find(id) ;
  if ( it != spline_stored.end() ) {
    head = it->second ;
  } else {
    return -1 ; // spline non trovata
  }
  return 0 ;
}

EXTERN_C
int
SPLINE_delete( char const id[] ) {
  MAP_SPLINE::iterator it = spline_stored.find(id) ;
  if ( it != spline_stored.end() ) {
    delete it->second ;
    spline_stored.erase(it) ;
    head = nullptr ;
  } else {
    return -1 ; // spline non trovata
  }
  return 0 ;
}

EXTERN_C
int
SPLINE_print() {
  if ( head != nullptr ) {
    head -> writeToStream( cout ) ;
    return 0 ;
  } else {
    cout << "No Spline!\n";
    return -1 ;
  }
}

EXTERN_C
char const *
SPLINE_get_type_name() {
  if ( head == nullptr ) return "NOTYPE - head = nullptr" ;
  return head -> type_name() ;
}

EXTERN_C
void *
SPLINE_mem_ptr( char const id[] ) {
  // check if exists ?
  return static_cast<void*>(&spline_stored[id]) ;
}

EXTERN_C
int
SPLINE_init() {
  if ( head != nullptr ) {
    head -> clear() ;
    return 0 ;
  } else {
    return -1 ;
  }
}

EXTERN_C
int
SPLINE_push( double const x, double const y ) {
  if ( head != nullptr ) {
    head -> pushBack(x,y) ;
    return 0 ;
  } else {
    return -1 ;
  }
}

EXTERN_C
int
SPLINE_build() {
  if ( head != nullptr ) {
    head -> build() ;
    return 0 ;
  } else {
    return -1 ;
  }
}

EXTERN_C
int
SPLINE_build2( double const x[], double const y[], int const n ) {
  if ( head != nullptr ) {
    head -> build( x, y, Splines::sizeType(n) ) ;
    return 0 ;
  } else {
    return -1 ;
  }
}


EXTERN_C
double
SPLINE_eval( double const x ) {
  if ( head != nullptr ) {
    return head -> operator()(x) ;
  } else {
    return 0 ;
  }
}

EXTERN_C
double
SPLINE_eval_D( double const x ) {
  if ( head != nullptr ) {
    return head -> D(x) ;
  } else {
    return 0 ;
  }
}

EXTERN_C
double
SPLINE_eval_DD( double const x ) {
  if ( head != nullptr ) {
    return head -> DD(x) ;
  } else {
    return 0 ;
  }
}

EXTERN_C
double
SPLINE_eval_DDD( double const x ) {
  if ( head != nullptr ) {
    return head -> DDD(x) ;
  } else {
    return 0 ;
  }
}

//
// eof: SplineCinterface.cc
//
