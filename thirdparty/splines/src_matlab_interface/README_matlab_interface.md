MATLAB interface for C++ Splines library
=======

`Splines` is a set of C++ classes which implements various spline interpolation.

**Compilation**

To compile the mex files run the script `Compile.m` in MATLAB.
After the compilation the scripts `spline1d` for univariate
spline and `spline2d` for bivariate spline are available.

**Univariate splines**

To build a univariate spline use to command

~~~
spline1d( 'name', 'type', X, Y ) ; 
~~~

- `name` is a string with the name of the spline, subsequent call
         of `spline1d` must refer to this name.
- `type` is a string with the type of spline you are building.
         Possibile values are:
         linear, cubic, akima, bessel, pchip, quintic.
- X, Y   two vectors with the x and y coordinates of the point 
         to be interpolated, X must be increasing.

after the build spline can be used as follows

~~~
% build the spline
Y = spline1d('pippo','akima',X,Y) ;

...
x = [1,2,3] ;
y = spline1d('pippo',X) ; % compute values
~~~

it is possibile to get also the derivative of the spline

~~~
[y,dy] = spline1d('pippo',X) ;
[y,dy,ddy] = spline1d('pippo',X) ;
~~~

where `dy` is a vector with the first derivative and 
`ddy` is a vector with the second derivative.

if you prefer a `pp` MATLAB structure can be generated

~~~
% build the spline and return a 
% MATLAB piecewise polynomial structure
pp = spline1d('akima',X,Y) ;

...
x = [1,2,3] ;
y = ppval(pp,X) ; % compute values
~~~

**Bivariate splines**

To build a bivariate spline use to command

~~~
spline2d( 'name', 'type', X, Y, Z ) ; 
~~~

- `name` is a string with the name of the spline, subsequent call
         of `spline1d` must refer to this name.
- `type` is a string with the type of spline you are building.
         Possibile values are:
         bilinear, bicubic, akima, biquintic.
- X, Y   two vectors with the x and y coordinates of the mesh 
         to be interpolated, X and Y must be increasing.
- Z      matrix of dimension `length(X)` x `length(Y)`
         where `Z(i,j)` is the z elevation at point
         `(X(i),Z(j))`.

after the build spline can be used as follows

~~~
% build the spline
Y = spline2d('pippo','akima',X,Y,Z) ;

...
x = [1,2,3] ;
y = [2,3,4] ;
z = spline2d('pippo',x,y) ; % compute values
% z is a matrix 3x3 with z(i,j) the spline S(x(i),y(j))

...
xy = [ 1,2,3 ; 1,2,1 ] ;
z = spline2d('pippo',xy) ; % compute values
% z is a matrix 1x3 with z(i) the spline S(xy(1,i),xy(2,j))
~~~

it is possibile to get also the derivative of the spline

~~~
[z,dx,dy] = spline1d('pippo',X,Y) ;
[z,dx,dy,dxx,dxy,dyy] = spline1d('pippo',X,Y) ;
~~~

where

- `dx` derivative respect to `x`
- `dy` derivative respect to `y`
- `dxx` second derivative respect to `x`
- `dxy` second derivative respect to `x` and `y`
- `dyy` second derivative respect to `y`

* * *

Enrico Bertolazzi<br>
Dipartimento di Ingegneria Industriale<br>
Universita` degli Studi di Trento<br>
email: enrico.bertolazzi@unitn.it
