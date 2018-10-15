Splines [![Build Status](https://travis-ci.org/ebertolazzi/Splines.svg?branch=master)](https://travis-ci.org/ebertolazzi/Splines)
=======

`Splines` is a set of C++ classes which implements varios spline interpolation.
The classes are the following:
 
  - ConstantSpline, for piecewise constants functions
  - LinearSpline, for piecewise linear interpolation
  - CubicSpline, for classical cubic spline interpolation
  - AkimaSpline, for Akima "non oscillatory" spline interpolation 
  - BesselSpline, for Bessel "non oscillatory" spline interpolation 
  - PchipSpline, 
  - QuinticSpline, Simple quintic spline based oin PCHIP with 4th
    derivative set to 0 at nodal points

**References**

- F.N. Fritsch and R.E. Carlson,
  Monotone Piecewise Cubic Interpolation,
  SIAM Journal of Numerical Analysis, Vol. 17, No. 2, pp. 238-246,
  April 1980.
 
**Usage**

The usage is simple:

~~~~~~~~~~~~~
#include "Splines.hh"
using namespace SplinesLoad ;

....

CubicSpline spline ;
double x[] = {1,2,3,4} ;
double y[] = {3,1,1,3} ;
spline.build(x,y,4) ; // build a cubic spline with 4 points
  
cout << spline(1.1) << '\n';     // spline at x = 1.1
cout << spline.D(1.1) << '\n';   // spline first derivative at x = 1.1
cout << spline.DD(1.1) << '\n';  // spline second derivative at x = 1.1
cout << spline.DDD(1.1) << '\n'; // spline third derivative at x = 1.1
~~~~~~~~~~~~~

splines can be built incrementally 

~~~~~~~~~~~~~
#include "Splines.hh"
using namespace SplinesLoad ;

....

CubicSpline spline ;
  
spline . pushBack( 1, 3 ) ;
spline . pushBack( 2, 1 ) ;
spline . pushBack( 3, 1 ) ;
spline . pushBack( 4, 3 ) ;
spline . build() ;
  
cout << spline(1.1) << '\n';     // spline at x = 1.1
cout << spline.D(1.1) << '\n';   // spline first derivative at x = 1.1
cout << spline.DD(1.1) << '\n';  // spline second derivative at x = 1.1
cout << spline.DDD(1.1) << '\n'; // spline third derivative at x = 1.1
~~~~~~~~~~~~~

or by using standard vector 

~~~~~~~~~~~~~
#include "Splines.hh"
#include <vector>
using namespace SplinesLoad ;
using namespace std ;

....

CubicSpline spline ;
std::vector x, y ;
x.push_back(1) ; y.push_back(3) ;
x.push_back(2) ; y.push_back(1) ;
x.push_back(3) ; y.push_back(1) ;
x.push_back(4) ; y.push_back(3) ;
spline . build(x,y) ;
  
cout << spline(1.1) << '\n';     // spline at x = 1.1
cout << spline.D(1.1) << '\n';   // spline first derivative at x = 1.1
cout << spline.DD(1.1) << '\n';  // spline second derivative at x = 1.1
cout << spline.DDD(1.1) << '\n'; // spline third derivative at x = 1.1
~~~~~~~~~~~~~

**Compile and tests**

Edit makefile file to match compiler of your OS and do:

  make

To run the test

  make run

To generate documentation (using DOXYGEN: http://www.stack.nl/~dimitri/doxygen/index.html)

make doc

**DOXYGEN documentation**

Available at: http://www.ing.unitn.it/~bertolaz/4-software/Splines/index.html

* * *

Enrico Bertolazzi<br>
Dipartimento di Ingegneria Industriale<br>
Universita` degli Studi di Trento<br>
email: enrico.bertolazzi@unitn.it
