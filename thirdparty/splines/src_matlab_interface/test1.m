xx0 = [  0,  1,  2,  3,  4,  5,    6,  7,  8,  9, 10 ] ; 
yy0 = [ 10, 10, 10, 10, 10, 10, 10.5, 15, 50, 60, 85 ] ;

xx1 = [  0,  1,  3,  4,  6,  7,    9, 10, 12, 13, 15 ] ;
yy1 = [ 10, 10, 10, 10, 10, 10, 10.5, 15, 50, 60, 85 ] ;

xx2 = [  0,  2,  3,  5,  6,  8,    9, 11, 12, 14, 15 ] ;
yy2 = [ 10, 10, 10, 10, 10, 10, 10.5, 15, 50, 60, 85 ] ;

xx3 = [ 7.99, 8.09,       8.19,       8.7,      9.2,      10,       12,       15,       20       ] ;
yy3 = [ 0,    2.76429e-5, 4.37498e-2, 0.169183, 0.469428, 0.943740, 0.998636, 0.999919, 0.999994 ] ;

xx4 = [ 595,   635,   695,   795,   855,   875,   895,   915,   935,   985,   1035,  1075  ] ;
yy4 = [ 0.644, 0.652, 0.644, 0.694, 0.907, 1.336, 2.169, 1.598, 0.916, 0.607, 0.603, 0.608 ] ;

xx5 = [ 0.11, 0.12, 0.15, 0.16 ] ;
yy5 = [ 0.0003, 0.0003, 0.0004, 0.0004 ] ;

XXX = { xx1, xx2, xx3, xx4 } ;
YYY = { yy1, yy2, yy3, yy4 } ;
LOC = {'northwest','northwest','southeast','northwest'} ;

for k=1:4
  X = XXX{k} ;
  Y = YYY{k} ;

  spline1d('li','linear',X,Y) ;
  spline1d('cu','cubic',X,Y) ;
  spline1d('ak','akima',X,Y) ;
  spline1d('be','bessel',X,Y) ;
  spline1d('pc','pchip',X,Y) ;
  spline1d('qu','quintic',X,Y) ;

  XX = X(1):(X(end)-X(1))/1000:X(end) ; ...

  Y1 = spline1d('li',XX) ;
  Y2 = spline1d('cu',XX) ;
  Y3 = spline1d('ak',XX) ;
  Y4 = spline1d('be',XX) ;
  Y5 = spline1d('pc',XX) ;
  Y6 = spline1d('qu',XX) ;
	
  subplot(2,2,k) ;

  plot( X,  Y, 'o', ...
        XX, Y1, ...
        XX, Y2, ...
        XX, Y3, ...
        XX, Y4, ...
        XX, Y5, ...
        XX, Y6 ) ;

  legend('data','linear','cubic','akima','bessel','pchip','quintic') ;
  legend('boxoff') ;
  legend('Location',LOC{k}) ;

end
