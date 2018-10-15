X = 0:0.1:1 ;
Y = 0:0.25:2 ;
[XX,YY] = ndgrid(X,Y) ;
ZZ = franke(XX,YY) ;

spline2d('bc','bicubic',X,Y,ZZ) ;
spline2d('ak','akima',X,Y,ZZ) ;
spline2d('bl','bilinear',X,Y,ZZ) ;
spline2d('bq','biquintic',X,Y,ZZ) ;

surf(XX,YY,ZZ), view(145,-2), set(gca,'Fontsize',16) ;

X = 0:0.01:1 ;
Y = 0:0.01:2 ;
[XX,YY] = ndgrid(X,Y) ;

Z1 = spline2d('bc',X,Y) ;
Z2 = spline2d('ak',X,Y) ;
Z3 = spline2d('bl',X,Y) ;
Z4 = spline2d('bq',X,Y) ;

subplot(2,2,1) ;
surf(XX,YY,Z1,'Linestyle',':'), view(145,40), set(gca,'Fontsize',16) ;
title('bicubic') ;

subplot(2,2,2) ;
surf(XX,YY,Z2,'Linestyle',':'), view(145,40), set(gca,'Fontsize',16) ;
title('akima') ;

subplot(2,2,3) ;
surf(XX,YY,Z3,'Linestyle',':'), view(145,40), set(gca,'Fontsize',16) ;
title('bilinear') ;

subplot(2,2,4) ;
surf(XX,YY,Z4,'Linestyle',':'), view(145,40), set(gca,'Fontsize',16) ;
title('biquintic') ;
