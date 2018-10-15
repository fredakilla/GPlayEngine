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
#include <iomanip>

#ifdef __GCC__
#pragma GCC diagnostic ignored "-Wc++98-compat"
#endif
#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++98-compat"
#endif

/**
 * 
 */

namespace Splines {

  using namespace std ; // load standard namespace


  template <int degree>
  class BSplineBase {
  public:

    /*
    // dati i nodi knot[0]...knot[2*degree+1] calcola le basi
    // B[0]...B[degree] che hanno supporto knot[j]..knot[j+degree+1]
    // vale la condizione knot[degree] <= x <= knot[degree+1]
    */
    static
    void
    eval( valueType       x,
          valueType const knot[],
          valueType       Bbase[] ) {
      valueType B[2*degree+2] ;
      std::fill( B, B+2*degree+2, 0.0 ) ;
      B[degree] = 1 ;
      for ( sizeType r = 1 ; r <= degree; ++r ) {
        for ( sizeType j = 0 ; j <= 2*degree-r ; ++j ) {
          if ( knot[j] <= x && x <= knot[j+r+1] ) {
            valueType oma = 0 ;
            valueType omb = 0 ;
            if ( knot[j+r]   > knot[j]   ) oma = (x-knot[j])/(knot[j+r]-knot[j]) ;
            if ( knot[j+r+1] > knot[j+1] ) omb = (knot[j+r+1]-x)/(knot[j+r+1]-knot[j+1]) ;
            B[j] = oma*B[j] + omb*B[j+1] ;
          }
        }
      }
      std::copy( B, B+degree+1, Bbase ) ;
    }

    static
    void
    eval_D( valueType       x,
            valueType const knot[],
            valueType       B_D[] ) {
      valueType B[degree+2] ;
      B[0] = B[degree+1] = 0 ;
      BSplineBase<degree-1>::eval( x, knot+1, B+1 ) ;
      for ( sizeType j = 1 ; j <= degree ; ++j ) B[j] *= degree/(knot[j+degree]-knot[j]) ;
      for ( sizeType j = 0 ; j <= degree ; ++j ) B_D[j] = B[j]-B[j+1] ;
    }

    static
    void
    eval_DD( valueType       x,
             valueType const knot[],
             valueType       B_DD[] ) {
      valueType B_D[degree+2] ;
      B_D[0] = B_D[degree+1] = 0 ;
      BSplineBase<degree-1>::eval_D( x, knot+1, B_D+1 ) ;
      for ( sizeType j = 1 ; j <= degree ; ++j ) B_D[j] *= degree/(knot[j+degree]-knot[j]) ;
      for ( sizeType j = 0 ; j <= degree ; ++j ) B_DD[j] = B_D[j]-B_D[j+1] ;
    }

    static
    void
    eval_DDD ( valueType       x,
               valueType const knot[],
               valueType       B_DDD[] ) {
      valueType B_DD[degree+2] ;
      B_DD[0] = B_DD[degree+1] = 0 ;
      BSplineBase<degree-1>::eval_DD( x, knot+1, B_DD+1 ) ;
      for ( sizeType j = 1 ; j <= degree ; ++j ) B_DD[j] *= degree/(knot[j+degree]-knot[j]) ;
      for ( sizeType j = 0 ; j <= degree ; ++j ) B_DDD[j] = B_DD[j]-B_DD[j+1] ;
    }
  } ;


  template <>
  class BSplineBase<0> {
  public:

    static void
    eval( valueType       x,
          valueType const knot[],
          valueType       Bbase[] ) {
      Bbase[0] = knot[0] <= x && x <= knot[1] ? 1 : 0 ;
    }

    static void
    eval_D( valueType, valueType const [], valueType B_D[] ) { B_D[0] = 0 ; }

    static void
    eval_DD( valueType, valueType const [], valueType B_DD[] ) { B_DD[0] = 0 ; }

    static void
    eval_DDD( valueType, valueType const [], valueType B_DDD[] ) { B_DDD[0] = 0 ; }
  } ;

  template <int degree>
  class BSplineEval {
  public:

    /*
    //  Calcola il valore di
    //
    //  y[0] * B[0](x) + y[1] * B[1](x) + ... + y[degree] * B[degree](x)
    //
    //  Usando usando la ricorsione e l'algoritmo di De Boor.
    */
    static
    void
    eval_levels( valueType x, valueType const knot[], valueType y[] ) {
      sizeType j = degree ;
      do {
        if ( knot[j+degree] > knot[j] ) {
          valueType omega = (x-knot[j])/(knot[j+degree]-knot[j]) ;
          y[j] = (1-omega)*y[j-1]+omega*y[j] ;
        } else {
          y[j] = y[j-1] ;
        }
      } while ( --j > 0 ) ;
      BSplineEval<degree-1>::eval_levels( x, knot+1, y+1 ) ;
    }

    /*
    //  Calcola il valore di
    //
    //  y[0] * B[0](x) + y[1] * B[1](x) + ... + y[degree] * B[degree](x)
    //
    //  Usando usando la ricorsione e l'algoritmo di De Boor.
    //  offs e' tale che knot[degree-offs] <= x <= knot[degree-offs+1]
    */
    static
    valueType
    eval( valueType       x,
          valueType const knot[],
          valueType const y[] ) {
      valueType c[degree+1], kn[2*degree+2] ;
      std::copy( y,    y+degree+1,      c  ) ;
      std::copy( knot, knot+2*degree+2, kn ) ;
      BSplineEval<degree>::eval_levels( x, kn, c ) ;
      return c[degree] ;
    }

    static
    valueType
    eval_D( valueType       x,
            valueType const knot[],
            valueType const y[] ) {
      valueType d[degree] ; // poligono derivata
      for ( sizeType j = 0 ; j < degree ; ++j )
        d[j] = degree*(y[j+1]-y[j])/(knot[j+degree+1] - knot[j+1]) ;
      return BSplineEval<degree-1>::eval( x, knot+1, d ) ;
    }

    static
    valueType
    eval_DD( valueType       x,
             valueType const knot[],
             valueType const y[] ) {
      valueType d[degree] ; // poligono derivata
      for ( sizeType j = 0 ; j < degree ; ++j )
        d[j] = degree*(y[j+1]-y[j])/(knot[j+degree+1] - knot[j+1]) ;
      return BSplineEval<degree-1>::eval_D( x, knot+1, d ) ;
    }

    static
    valueType
    eval_DDD( valueType       x,
              valueType const knot[],
              valueType const y[] ) {
      valueType d[degree] ; // poligono derivata
      for ( sizeType j = 0 ; j < degree ; ++j )
        d[j] = degree*(y[j+1]-y[j])/(knot[j+degree+1] - knot[j+1]) ;
      return BSplineEval<degree-1>::eval_DD( x, knot+1, d ) ;
    }
  } ;

  template <>
  class BSplineEval<1> {
  public:

    static void
    eval_B( valueType       x,
            valueType const knot[],
            valueType       Bbase[] ) {
      valueType B[4] ;
      std::fill( B, B+4, 0.0 ) ;
      B[1] = 1 ;
      for ( sizeType j = 0 ; j <= 1 ; ++j ) {
        if ( knot[j] <= x && x <= knot[j+2] ) {
          valueType oma = 0 ;
          valueType omb = 0 ;
          if ( knot[j+1] > knot[j]   ) oma = (x-knot[j])/(knot[j+1]-knot[j]) ;
          if ( knot[j+2] > knot[j+1] ) omb = (knot[j+2]-x)/(knot[j+2]-knot[j+1]) ;
          B[j] = oma*B[j] + omb*B[j+1] ;
        }
      }
      std::copy( B, B+2, Bbase ) ;
    }

    static void
    eval_B_D( valueType, valueType const [], valueType B_D[] ) { B_D[0] = 0 ; }

    static void
    eval_B_DD( valueType, valueType const [], valueType B_DD[] ) { B_DD[0] = 0 ; }

    static void
    eval_B_DDD( valueType, valueType const [], valueType B_DDD[] ) { B_DDD[0] = 0 ; }

    static
    void
    eval_levels( valueType       x,
                 valueType const knot[],
                 valueType       y[] ) {
      valueType omega = (x-knot[1])/(knot[2]-knot[1]) ;
      y[1] = (1-omega)*y[0]+omega*y[1] ;
    }

    static
    valueType
    eval( valueType       x,
          valueType const knot[],
          valueType const y[] ) {
      valueType omega = (x-knot[1])/(knot[2]-knot[1]) ;
      return (1-omega)*y[0]+omega*y[1] ;
    }

    static
    valueType
    eval_D( valueType             ,
            valueType const knot[],
            valueType const y[] ) {
      return (y[1]-y[0])/(knot[2] - knot[1]) ;
    }

    static
    valueType
    eval_DD( valueType         ,
             valueType const [],
             valueType const [] ) {
      return 0 ;
    }

    static
    valueType
    eval_DDD( valueType         ,
              valueType const [],
              valueType const [] ) {
      return 0 ;
    }

  } ;
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // standard average nodes
  template <int _degree>
  void
  BSpline<_degree>::knots_sequence( sizeType        n,
                                    valueType const X[],
                                    valueType     * Knots ) {
    for ( sizeType j = 0 ; j <= _degree ; ++j ) Knots[j] = X[0] ;
    Knots += _degree+1 ;
    for ( sizeType j = 0 ; j < n-_degree-1 ; ++j ) {
      valueType tmp = 0 ;
      for ( sizeType k = 1 ; k <= _degree ; ++k ) tmp += X[j+k] ;
      Knots[j] = tmp / _degree ;
    }
    Knots += n-_degree-1 ;
    for ( sizeType j = 0 ; j <= _degree ; ++j ) Knots[j] = X[n-1] ;
  }

  template <int _degree>
  void
  BSpline<_degree>::sample_bases( indexType           nx,
                                  valueType const     X[],
                                  indexType           nb,
                                  valueType const     Knots[],
                                  vector<indexType> & II,
                                  vector<indexType> & JJ,
                                  vector<valueType> & vals ) {
    valueType row[_degree+1] ;
    II.clear()    ; II.reserve( sizeType(nx) ) ;
    JJ.clear()    ; JJ.reserve( sizeType(nx) ) ;
    vals.clear() ; vals.reserve( sizeType(nx) ) ;
    for ( indexType i = 0 ; i < nx ; ++i ) {
      indexType ii = indexType(lower_bound( Knots+_degree, Knots+nb+_degree, X[i] ) - Knots ) ;
      if ( ii > _degree ) --ii ;
      ii -= _degree ;
      BSplineBase<_degree>::eval( X[i], Knots+ii, row ) ;
      for ( indexType j = 0 ; j <= _degree ; ++j ) {
        II.push_back( i ) ;
        JJ.push_back( ii+j ) ;
        vals.push_back( row[j] ) ;
      }
    }
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  template <int _degree>
  sizeType
  BSpline<_degree>::knot_search( valueType x ) const {
    SPLINE_ASSERT( npts > 0, "\nknot_search(" << x << ") empty spline");
    if ( x < knots[lastInterval] || knots[lastInterval+1] < x ) {
      if ( _check_range ) {
        SPLINE_ASSERT( x >= X[0] && x <= X[npts-1],
                       "method search( " << x << " ) out of range: [" <<
                       X[0] << ", " << X[npts-1] << "]" ) ;
      }
      // 0 1 2 3
      lastInterval = sizeType(lower_bound( knots+_degree, knots+npts+1, x ) - knots) ;
      if ( lastInterval > _degree ) --lastInterval ;
    }
    return lastInterval-_degree ;
  }

  /*
  // Solve banded linear system
  */
  static
  void
  solveBanded( valueType * rows,
               valueType * rhs,
               sizeType    n,
               sizeType    ndiag ) {
    // forward
    sizeType rsize = 1+2*ndiag ;
    sizeType i = 0 ;
    valueType * rowsi = rows + ndiag ;
    do {
      // pivot
      valueType pivot = rowsi[0] ; // elemento sulla diagonale
      
      // scala equazione
      for ( sizeType j = 0 ; j <= ndiag ; ++j ) rowsi[j] /= pivot ;
      rhs[i] /= pivot ;

      // azzera colonna
      sizeType nr = i+ndiag >= n ? n-i-1 : ndiag ;
      for ( sizeType k = 1 ; k <= nr ; ++k ) {
        valueType * rowsk = rowsi + k * (rsize-1) ;
        valueType tmp = rowsk[0] ;
        rowsk[0] = 0 ;
        for ( sizeType j = 1 ; j <= nr ; ++j ) rowsk[j] -= tmp*rowsi[j] ;
        rhs[i+k] -= tmp*rhs[i] ;
      }
      rowsi += rsize ;
    } while ( ++i < n ) ;
    // backward
    while ( i > 0 ) {
      --i ;
      rowsi -= rsize ;
      sizeType nr = i+ndiag >= n ? n-i-1 : ndiag ;
      for ( sizeType j = 1 ; j <= nr ; ++j ) rhs[i] -= rhs[i+j]*rowsi[j] ;
    }
  }

  template <int _degree>
  void
  BSpline<_degree>::build(void) {
    std::vector<valueType> band(npts*(2*_degree+1)) ;
    
    std::fill( band.begin(), band.end(), 0 ) ;
    knots_sequence( npts, X, knots ) ;
    
    // costruzione sistema lineare

    // calcola il valore delle basi non zero quando
    // knot[degree] <= x <= knot[degree+1]
    sizeType nr = 2*_degree+1 ;
    for ( sizeType i = 0 ; i < npts ; ++i ) {
      valueType * rowi = &band[nr * i] ;
      sizeType ii = knot_search( X[i] ) ;
      BSplineBase<_degree>::eval( X[i], knots+ii, (rowi + ii + _degree) - i ) ;
      yPolygon[i] = Y[i] ;
    }
    solveBanded( &band.front(), yPolygon, npts, _degree ) ;
    // extrapolation
    valueType const * knots_R    = knots    + npts - _degree - 1 ;
    valueType const * yPolygon_R = yPolygon + npts - _degree - 1 ;
    valueType x_L = X[0] ;
    valueType x_R = X[npts-1] ;
    s_L   = BSplineEval<_degree>::eval(x_L,knots,yPolygon) ;
    s_R   = BSplineEval<_degree>::eval(x_R,knots_R,yPolygon_R) ;
    ds_L  = BSplineEval<_degree>::eval_D(x_L,knots,yPolygon) ;
    ds_R  = BSplineEval<_degree>::eval_D(x_R,knots_R,yPolygon_R) ;
    dds_L = BSplineEval<_degree>::eval_DD(x_L,knots,yPolygon) ;
    dds_R = BSplineEval<_degree>::eval_DD(x_R,knots_R,yPolygon_R) ;
  }
  
  template <int _degree>
  void
  BSpline<_degree>::clear(void) {
    if ( !_external_alloc ) baseValue.free() ;
    npts = npts_reserved = 0 ;
    _external_alloc = false ;
    X = Y = knots = yPolygon = nullptr ;
  }

  template <int _degree>
  void
  BSpline<_degree>::reserve( sizeType n ) {
    if ( _external_alloc && n <= npts_reserved ) {
      // nothing to do!, already allocated
    } else {
      npts_reserved = n ;
      baseValue.allocate(4*n+_degree+1) ;
      X        = baseValue(n) ;
      Y        = baseValue(n) ;
      knots    = baseValue(n+_degree+1) ;
      yPolygon = baseValue(n) ;
      _external_alloc = false ;
    }
    npts         = 0 ;
    lastInterval = _degree ;
  }

  template <int _degree>
  void
  BSpline<_degree>::reserve_external( sizeType     n,
                                      valueType *& p_x,
                                      valueType *& p_y,
                                      valueType *& p_knots,
                                      valueType *& p_yPolygon ) {
    npts_reserved   = n ;
    X               = p_x ;
    Y               = p_y ;
    knots           = p_knots ;
    yPolygon        = p_yPolygon ;
    npts            = 0 ;
    lastInterval    = _degree ;
    _external_alloc = true ;
  }

  template <int _degree>
  valueType
  BSpline<_degree>::eval( valueType x, sizeType n, valueType const Knots[], valueType const yPoly[] ) {
    sizeType idx = sizeType(lower_bound( Knots+_degree, Knots+n+1, x ) - Knots) ;
    if ( idx > _degree ) --idx ;
    idx -= _degree ;
    return BSplineEval<_degree>::eval(x,Knots+idx,yPoly+idx) ;
  }

  template <int _degree>
  valueType
  BSpline<_degree>::eval_D( valueType x, sizeType n, valueType const Knots[], valueType const yPoly[] ) {
    sizeType idx = sizeType(lower_bound( Knots+_degree, Knots+n+1, x ) - Knots) ;
    if ( idx > _degree ) --idx ;
    idx -= _degree ;
    return BSplineEval<_degree>::eval_D(x,Knots+idx,yPoly+idx) ;
  }

  template <int _degree>
  valueType
  BSpline<_degree>::eval_DD( valueType x, sizeType n, valueType const Knots[], valueType const yPoly[] ) {
    sizeType idx = sizeType(lower_bound( Knots+_degree, Knots+n+1, x ) - Knots) ;
    if ( idx > _degree ) --idx ;
    idx -= _degree ;
    return BSplineEval<_degree>::eval_DD(x,Knots+idx,yPoly+idx) ;
  }

  template <int _degree>
  valueType
  BSpline<_degree>::eval_DDD( valueType x, sizeType n, valueType const Knots[], valueType const yPoly[] ) {
    sizeType idx = sizeType(lower_bound( Knots+_degree, Knots+n+1, x ) - Knots) ;
    if ( idx > _degree ) --idx ;
    idx -= _degree ;
    return BSplineEval<_degree>::eval_DDD(x,Knots+idx,yPoly+idx) ;
  }

  template <int _degree>
  valueType
  BSpline<_degree>::operator () ( valueType x ) const {
    if ( x >= X[npts-1] ) {
      valueType dx = x - X[npts-1] ;
      return s_R + dx * ( ds_R + 0.5 * dds_R * dx ) ;
    } else if ( x <= X[0] ) {
      valueType dx = x - X[0] ;
      return s_L + dx * ( ds_L + 0.5 * dds_L * dx ) ;
    } else {
      sizeType idx = knot_search( x ) ;
      return BSplineEval<_degree>::eval(x,knots+idx,yPolygon+idx) ;
    }
  }

  template <int _degree>
  valueType
  BSpline<_degree>::D( valueType x ) const {
    if ( x >= X[npts-1] ) {
      valueType dx = x - X[npts-1] ;
      return ds_R + dds_R * dx ;
    } else if ( x <= X[0] ) {
      valueType dx = x - X[0] ;
      return ds_L + dds_L * dx ;
    } else {
      sizeType idx = knot_search( x ) ;
      return BSplineEval<_degree>::eval_D(x,knots+idx,yPolygon+idx) ;
    }
  }

  template <int _degree>
  valueType
  BSpline<_degree>::DD( valueType x ) const {
    if ( x >= X[npts-1] ) {
      return dds_R ;
    } else if ( x <= X[0] ) {
      return dds_L ;
    } else {
      sizeType idx = knot_search( x ) ;
      return BSplineEval<_degree>::eval_DD(x,knots+idx,yPolygon+idx) ;
    }
  }

  template <int _degree>
  valueType
  BSpline<_degree>::DDD( valueType x ) const {
    if ( x >= X[npts-1] || x <= X[0] ) {
      return 0 ;
    } else {
      sizeType idx = knot_search( x ) ;
      return BSplineEval<_degree>::eval_DDD(x,knots+idx,yPolygon+idx) ;
    }
  }

  template <int _degree>
  sizeType // order
  BSpline<_degree>::coeffs( valueType cfs[], valueType nodes[], bool transpose ) const {
    SPLINE_ASSERT( false, "BSpline<_degree>::coeffs not yet implemented" ) ;
#if 0
    sizeType n = npts > 0 ? npts-1 : 0 ;
    for ( sizeType i = 0 ; i < n ; ++i ) {
      nodes[i] = X[i] ;
      valueType H  = X[i+1]-X[i] ;
      valueType DY = (Y[i+1]-Y[i])/H ;
      valueType a = Y[i] ;
      valueType b = Yp[i] ;
      valueType c = (3*DY-2*Yp[i]-Yp[i+1])/H;
      valueType d = (Yp[i+1]+Yp[i]-2*DY)/(H*H) ;
      if ( transpose ) {
        cfs[4*i+3] = a ;
        cfs[4*i+2] = b ;
        cfs[4*i+1] = c ;
        cfs[4*i+0] = d ;
      } else {
        cfs[i+3*n] = a ;
        cfs[i+2*n] = b ;
        cfs[i+1*n] = c ;
        cfs[i+0*n] = d ;
      }
    }
#endif
    return _degree+1 ;
  }

  // Implementation
  template <int _degree>
  void
  BSpline<_degree>::copySpline( BSpline const & S ) {
    BSpline::reserve(S.npts) ;
    npts = S.npts ;
    std::copy( S.X,        S.X+npts,               X ) ;
    std::copy( S.Y,        S.Y+npts,               Y ) ;
    std::copy( S.knots,    S.knots+npts+_degree+1, knots ) ;
    std::copy( S.yPolygon, S.yPolygon+npts,        yPolygon ) ;
  }

  //! change X-range of the spline
  template <int _degree>
  void
  BSpline<_degree>::setRange( valueType xmin, valueType xmax ) {
    Spline::setRange( xmin, xmax ) ;
    valueType recS = ( X[npts-1] - X[0] ) / (xmax - xmin) ;
    valueType * iy = Y ;
    while ( iy < Y + npts ) *iy++ *= recS ;
  }

  template <int _degree>
  void
  BSpline<_degree>::writeToStream( std::basic_ostream<char> & s ) const {
    sizeType nseg = npts > 0 ? npts - 1 : 0 ;
    for ( sizeType i = 0 ; i < nseg ; ++i )
      s << "segment N." << setw(4) << i
        << " X:[" << X[i] << ", " << X[i+1]
        << "] Y:[" << Y[i] << ", " << Y[i+1]
        << "] slope: " << (Y[i+1]-Y[i])/(X[i+1]-X[i])
        << '\n' ; 
  }

  template <int _degree>
  void
  BSpline<_degree>::bases( valueType x, valueType vals[] ) const {
    std::fill( vals, vals+npts, 0 ) ;
    if ( x >= X[0] && x <= X[npts-1] ) {
      sizeType idx = knot_search( x ) ;
      BSplineBase<_degree>::eval( x, knots+idx, vals+idx ) ;
    }
  }

  template <int _degree>
  void
  BSpline<_degree>::bases_D( valueType x, valueType vals[] ) const {
    std::fill( vals, vals+npts, 0 ) ;
    if ( x >= X[0] && x <= X[npts-1] ) {
      sizeType idx = knot_search( x ) ;
      BSplineBase<_degree>::eval_D( x, knots+idx, vals+idx ) ;
    }
  }

  template <int _degree>
  void
  BSpline<_degree>::bases_DD( valueType x, valueType vals[] ) const {
    std::fill( vals, vals+npts, 0 ) ;
    if ( x >= X[0] && x <= X[npts-1] ) {
      sizeType idx = knot_search( x ) ;
      BSplineBase<_degree>::eval_DD( x, knots+idx, vals+idx ) ;
    }
  }

  template <int _degree>
  void
  BSpline<_degree>::bases_DDD( valueType x, valueType vals[] ) const {
    std::fill( vals, vals+npts, 0 ) ;
    if ( x >= X[0] && x <= X[npts-1] ) {
      sizeType idx = knot_search( x ) ;
      BSplineBase<_degree>::eval_DDD( x, knots+idx, vals+idx ) ;
    }
  }

  template <int _degree>
  sizeType
  BSpline<_degree>::bases_nz( valueType x, valueType vals[] ) const {
    sizeType idx = knot_search( x ) ;
    if ( x >= X[0] && x <= X[npts-1] ) BSplineBase<_degree>::eval( x, knots+idx, vals ) ;
    return idx ;
  }

  template <int _degree>
  sizeType
  BSpline<_degree>::bases_D_nz( valueType x, valueType vals[] ) const {
    sizeType idx = knot_search( x ) ;
    if ( x >= X[0] && x <= X[npts-1] ) BSplineBase<_degree>::eval_D( x, knots+idx, vals ) ;
    return idx ;
  }

  template <int _degree>
  sizeType
  BSpline<_degree>::bases_DD_nz( valueType x, valueType vals[] ) const {
    sizeType idx = knot_search( x ) ;
    if ( x >= X[0] && x <= X[npts-1] ) BSplineBase<_degree>::eval_DD( x, knots+idx, vals ) ;
    return idx ;
  }

  template <int _degree>
  sizeType
  BSpline<_degree>::bases_DDD_nz( valueType x, valueType vals[] ) const {
    sizeType idx = knot_search( x ) ;
    if ( x >= X[0] && x <= X[npts-1] ) BSplineBase<_degree>::eval_DDD( x, knots+idx, vals ) ;
    return idx ;
  }

  #ifdef __clang__
    #pragma clang diagnostic ignored "-Wweak-template-vtables"
  #endif

  template class BSpline<1> ;
  template class BSpline<2> ;
  template class BSpline<3> ;
  template class BSpline<4> ;
  template class BSpline<5> ;
  template class BSpline<6> ;
  template class BSpline<7> ;
  template class BSpline<8> ;
  template class BSpline<9> ;
  template class BSpline<10> ;

}
