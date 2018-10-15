/*--------------------------------------------------------------------------*\
 |                                                                          |
 |  Copyright (C) 1998                                                      |
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
/****************************************************************************\
Copyright (c) 2015, Enrico Bertolazzi
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.
\****************************************************************************/

/*!

\mainpage  Splines
\author    Enrico Bertolazzi (enrico.bertolazzi@unitn.it), homepage: http://www.ing.unitn.it/~bertolaz
\version   1.0.0
\note      first release Jan 12, 1998
\date      2016
\copyright GNU Public License.
\par       Affiliation:
           Department of Industrial Engineering<br>
           University of Trento<br>
           Via Sommarive 9, I -- 38123 Trento, Italy <br>
           enrico.bertolazzi@unitn.it

\details

This library is available at

 - https://github.com/ebertolazzi/Splines
 - https://bitbucket.org/ebertolazzi/splines
 
Splines
=======

`Splines` is a set of C++ classes which implements various spline interpolation.
The classes are the following:

  - ConstantSpline, for piecewise constants functions
  - LinearSpline, for piecewise linear interpolation
  - CubicSpline, for classical cubic spline interpolation
  - AkimaSpline, for Akima "non oscillatory" spline interpolation
  - BesselSpline, for Bessel "non oscillatory" spline interpolation
  - PchipSpline,
  - QuinticSpline, Simple quintic spline based oin PCHIP with 4th
    derivative set to 0 at nodal points

_References_

  - F.N. Fritsch and R.E. Carlson,
    Monotone Piecewise Cubic Interpolation,
    SIAM Journal of Numerical Analysis, Vol. 17, No. 2, pp. 238-246,
    April 1980.

Usage
=====

  - \subpage begin

*/


/*!

\page begin Simple spline examples

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

 spline.pushBack( 1, 3 ) ;
 spline.pushBack( 2, 1 ) ;
 spline.pushBack( 3, 1 ) ;
 spline.pushBack( 4, 3 ) ;
 spline.build() ;

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
 spline.build(x,y) ;

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

*/
