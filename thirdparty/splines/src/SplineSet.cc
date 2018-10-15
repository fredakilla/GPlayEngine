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
#include <limits>
#include <cmath>

#ifdef __GCC__
#pragma GCC diagnostic ignored "-Wc++98-compat"
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif
#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++98-compat"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#endif

/**
 * 
 */

namespace Splines {

  using std::abs ;
  using std::sqrt ;

  //! spline constructor
  SplineSet::SplineSet( string const & name )
  : _name(name)
  , baseValue(name+"_values")
  , basePointer(name+"_pointers")
  , _npts(0)
  , _nspl(0)
  , _X(nullptr)
  , _Y(nullptr)
  , _Yp(nullptr)
  , _Ypp(nullptr)
  , _Ymin(nullptr)
  , _Ymax(nullptr)
  , lastInterval(0)
  {}

  //! spline destructor
  SplineSet::~SplineSet() {
    baseValue.free() ;
    basePointer.free() ;
  }

  void
  SplineSet::info( std::basic_ostream<char> & s ) const {
    s << "SplineSet[" << name() << "] n.points = "
      << _npts << " n.splines = " << _nspl << '\n' ;
    for ( sizeType i = 0 ; i < _nspl ; ++i ) {
      s << "\nSpline n." << i ;
      switch ( is_monotone[i] ) {
        case -2: s << " with NON monotone data\n" ; break ;
        case -1: s << " is NOT monotone\n"        ; break ;
        case  0: s << " is monotone\n"            ; break ;
        case  1: s << " is strictly monotone\n"   ; break ;
        default: SPLINE_ASSERT( false, "SplineSet::info classification: " << is_monotone[i] << " not in range {-2,-1,0,1}" ) ;
      }
      splines[i]->info(s) ;
    }
  }

  void
  SplineSet::dump_table( std::basic_ostream<char> & stream, sizeType num_points ) const {
    vector<valueType> vals ;
    stream << 's' ;
    for ( sizeType i = 0 ; i < numSplines() ; ++i ) stream << '\t' << header(i) ;
    stream << '\n' ;
  
    for ( sizeType j = 0 ; j < num_points ; ++j ) {
      valueType s = xMin() + ((xMax()-xMin())*j)/(num_points-1) ;
      this->eval( s, vals ) ;
      stream << s ;
      for ( sizeType i = 0 ; i < numSplines() ; ++i ) stream << '\t' << vals[i] ;
      stream << '\n' ;
    }
  }

  sizeType
  SplineSet::getPosition( char const * hdr ) const {
    map<string,sizeType>::const_iterator it = header_to_position.find(hdr) ;
    SPLINE_ASSERT( it != header_to_position.end(), "SplineSet::getPosition(\"" << hdr << "\") not found!" ) ;
    return it->second ;
  }

  void
  SplineSet::build( sizeType   const nspl,
                    sizeType   const npts,
                    char       const *headers[],
                    SplineType const stype[],
                    valueType  const X[],
                    valueType  const *Y[],
                    valueType  const *Yp[] ) {
    SPLINE_ASSERT( nspl > 0, "SplineSet::build expected positive nspl = " << nspl ) ;
    SPLINE_ASSERT( npts > 1, "SplineSet::build expected npts = " << npts << " greather than 1" ) ;
    _nspl = nspl ;
    _npts = npts ;
    // allocate memory
    splines.resize(_nspl) ;
    is_monotone.resize(_nspl) ;
    sizeType mem = npts ;
    for ( sizeType spl = 0 ; spl < sizeType(nspl) ; ++spl ) {
      switch (stype[spl]) {
      case QUINTIC_TYPE:
        mem += npts ; // Y, Yp, Ypp
      case CUBIC_TYPE:
      case AKIMA_TYPE:
      case BESSEL_TYPE:
      case PCHIP_TYPE:
      case HERMITE_TYPE:
        mem += npts ; // Y, Yp
      case CONSTANT_TYPE:
      case LINEAR_TYPE:
        mem += npts ;
      break;
      case SPLINE_SET_TYPE:
      case BSPLINE_TYPE:
        SPLINE_ASSERT( false,
                       "SplineSet::build\nAt spline n. " << spl <<
                       " named " << headers[spl] <<
                       " cannot be done for type = " << stype[spl] ) ;
      }
    }

    baseValue.allocate( sizeType(mem + 2*nspl) ) ;
    basePointer.allocate( sizeType(3*nspl) ) ;

    _Y    = basePointer(_nspl) ;
    _Yp   = basePointer(_nspl) ;
    _Ypp  = basePointer(_nspl) ;
    _X    = baseValue(_npts) ;
    _Ymin = baseValue(_nspl) ;
    _Ymax = baseValue(_nspl) ;
    std::copy( X, X+npts, _X ) ;
    for ( sizeType spl = 0 ; spl < sizeType(nspl) ; ++spl ) {
      valueType *& pY   = _Y[spl] ;
      valueType *& pYp  = _Yp[spl] ;
      valueType *& pYpp = _Ypp[spl] ;
      pY = baseValue( _npts ) ;
      std::copy( Y[spl], Y[spl]+npts, pY ) ;
      if ( stype[spl] == CONSTANT_TYPE ) {
        _Ymin[spl] = *std::min_element( pY,pY+npts-1 ) ;
        _Ymax[spl] = *std::max_element( pY,pY+npts-1 ) ;        
      } else {
        _Ymin[spl] = *std::min_element( pY,pY+npts ) ;
        _Ymax[spl] = *std::max_element( pY,pY+npts ) ;
      }
      pYpp = pYp = nullptr ;
      switch ( stype[spl] ) {
      case QUINTIC_TYPE:
        pYpp = baseValue( _npts ) ;
      case CUBIC_TYPE:
      case AKIMA_TYPE:
      case BESSEL_TYPE:
      case PCHIP_TYPE:
      case HERMITE_TYPE:
        pYp = baseValue( _npts ) ;
        if ( stype[spl] == HERMITE_TYPE ) {
          SPLINE_ASSERT( Yp != nullptr && Yp[spl] != nullptr,
                         "SplineSet::build\nAt spline n. " << spl << " named " << headers[spl] <<
                         "\nexpect to find derivative values" ) ;
          std::copy( Yp[spl], Yp[spl]+npts, pYp ) ;
        }
      case CONSTANT_TYPE:
      case LINEAR_TYPE:
      case SPLINE_SET_TYPE:
        break;
      case BSPLINE_TYPE: // nothing to do (not implemented)
        break ;
      }
      string h = headers[spl] ;
      Spline * & s = splines[spl] ;
      
      is_monotone[spl] = -1 ;
      switch (stype[spl]) {
      case CONSTANT_TYPE:
        s = new ConstantSpline(h) ;
        static_cast<ConstantSpline*>(s)->reserve_external( _npts, _X, pY ) ;
        static_cast<ConstantSpline*>(s)->npts = _npts ;
        static_cast<ConstantSpline*>(s)->build() ;
        break;

      case LINEAR_TYPE:
        s = new LinearSpline(h) ;
        static_cast<LinearSpline*>(s)->reserve_external( _npts, _X, pY ) ;
        static_cast<LinearSpline*>(s)->npts = _npts ;
        static_cast<LinearSpline*>(s)->build() ;
        // check monotonicity of data
        { indexType flag = 1 ;
          for ( sizeType j = 1 ; j < _npts ; ++j ) {
            if ( pY[j-1] > pY[j] ) { flag = -1 ; break ; } // non monotone data
            if ( pY[j-1] == pY[j] && _X[j-1] < _X[j] ) flag = 0 ; // non strict monotone
          }
          is_monotone[spl] = flag ;
        }
        break;

      case CUBIC_TYPE:
        s = new CubicSpline(h) ;
        static_cast<CubicSpline*>(s)->reserve_external( _npts, _X, pY, pYp ) ;
        static_cast<CubicSpline*>(s)->npts = _npts ;
        static_cast<CubicSpline*>(s)->build() ;
        is_monotone[spl] = checkCubicSplineMonotonicity( _X, pY, pYp, _npts ) ;
        break;

      case AKIMA_TYPE:
        s = new AkimaSpline(h) ;
        static_cast<AkimaSpline*>(s)->reserve_external( _npts, _X, pY, pYp ) ;
        static_cast<AkimaSpline*>(s)->npts = _npts ;
        static_cast<AkimaSpline*>(s)->build() ;
        is_monotone[spl] = checkCubicSplineMonotonicity( _X, pY, pYp, _npts ) ;
        break ;

      case BESSEL_TYPE:
        s = new BesselSpline(h) ;
        static_cast<BesselSpline*>(s)->reserve_external( _npts, _X, pY, pYp ) ;
        static_cast<BesselSpline*>(s)->npts = _npts ;
        static_cast<BesselSpline*>(s)->build() ;
        is_monotone[spl] = checkCubicSplineMonotonicity( _X, pY, pYp, _npts ) ;
        break ;

      case PCHIP_TYPE:
        s = new PchipSpline(h) ;
        static_cast<PchipSpline*>(s)->reserve_external( _npts, _X, pY, pYp ) ;
        static_cast<PchipSpline*>(s)->npts = _npts ;
        static_cast<PchipSpline*>(s)->build() ;
        is_monotone[spl] = checkCubicSplineMonotonicity( _X, pY, pYp, _npts ) ;
        break ;

      case HERMITE_TYPE:
        s = new HermiteSpline(h) ;
        static_cast<CubicSpline*>(s)->reserve_external( _npts, _X, pY, pYp ) ;
        static_cast<CubicSpline*>(s)->npts = _npts ;
        static_cast<CubicSpline*>(s)->build() ;
        is_monotone[spl] = checkCubicSplineMonotonicity( _X, pY, pYp, _npts ) ;
        break;

      case QUINTIC_TYPE:
        s = new QuinticSpline(h) ;
        static_cast<QuinticSpline*>(s)->reserve_external( _npts, _X, pY, pYp, pYpp ) ;
        static_cast<QuinticSpline*>(s)->npts = _npts ;
        static_cast<QuinticSpline*>(s)->build() ;
        break;

      case SPLINE_SET_TYPE:
        SPLINE_ASSERT( false,
                       "SplineSet::build\nAt spline n. " << spl << " named " << headers[spl] <<
                       "\nSPLINE_SET_TYPE not allowed as spline type\nin SplineSet::build for " << spl << "-th spline" ) ;
      case BSPLINE_TYPE:
        SPLINE_ASSERT( false,
                       "SplineSet::build\nAt spline n. " << spl << " named " << headers[spl] <<
                       "\nBSPLINE_TYPE not allowed as spline type\nin SplineSet::build for " << spl << "-th spline" ) ;
      }
      header_to_position[s->name()] = spl ;
    }

  }

  // vectorial values
  void
  SplineSet::getHeaders( vector<string> & h ) const {
    h.resize(_nspl) ;
    for ( sizeType i = 0 ; i < _nspl ; ++i )
      h[i] = splines[i]->name() ;
  }

  void
  SplineSet::eval( valueType x, vector<valueType> & vals ) const {
    vals.resize(_nspl) ;
    for ( sizeType i = 0 ; i < _nspl ; ++i )
      vals[i] = (*splines[i])(x) ;
  }

  void
  SplineSet::eval( valueType x, valueType vals[], indexType incy ) const {
    sizeType ii = 0 ;
    for ( sizeType i = 0 ; i < _nspl ; ++i, ii += sizeType(incy) )
      vals[ii] = (*splines[i])(x) ;
  }

  void
  SplineSet::eval_D( valueType x, vector<valueType> & vals ) const {
    vals.resize(_nspl) ;
    for ( sizeType i = 0 ; i < _nspl ; ++i )
      vals[i] = splines[i]->D(x) ;
  }

  void
  SplineSet::eval_D( valueType x, valueType vals[], indexType incy ) const {
    sizeType ii = 0 ;
    for ( sizeType i = 0 ; i < _nspl ; ++i, ii += sizeType(incy) )
      vals[ii] = splines[i]->D(x) ;
  }

  void
  SplineSet::eval_DD( valueType x, vector<valueType> & vals ) const {
    vals.resize(_nspl) ;
    for ( sizeType i = 0 ; i < _nspl ; ++i )
      vals[i] = splines[i]->DD(x) ;
  }

  void
  SplineSet::eval_DD( valueType x, valueType vals[], indexType incy ) const {
    sizeType ii = 0 ;
    for ( sizeType i = 0 ; i < _nspl ; ++i, ii += sizeType(incy) )
      vals[ii] = splines[i]->DD(x) ;
  }

  void
  SplineSet::eval_DDD( valueType x, vector<valueType> & vals ) const {
    vals.resize(_nspl) ;
    for ( sizeType i = 0 ; i < _nspl ; ++i )
      vals[i] = splines[i]->DDD(x) ;
  }

  void
  SplineSet::eval_DDD( valueType x, valueType vals[], indexType incy ) const {
    sizeType ii = 0 ;
    for ( sizeType i = 0 ; i < _nspl ; ++i, ii += sizeType(incy) )
      vals[ii] = splines[i]->DDD(x) ;
  }

  // vectorial values
  
  Spline const *
  SplineSet::intersect( sizeType spl, valueType zeta, valueType & x ) const {
    SPLINE_ASSERT( spl >= 0 && spl < _nspl,
                   "Spline n." << spl << " is not in SplineSet") ;
    SPLINE_ASSERT( is_monotone[sizeType(spl)]>0,
                   "Spline n." << spl << " is not monotone and can't be used as independent") ;
    Spline const * S = splines[sizeType(spl)] ;
    // cerco intervallo intersezione
    valueType const * X = _Y[sizeType(spl)] ;
    SPLINE_ASSERT( zeta >= X[0] && zeta <= X[_npts-1],
                   "SplineSet, evaluation at zeta = " << zeta <<
                   " is out of range: [" << X[0] << ", " << X[_npts-1] << "]" ) ;

    sizeType interval = sizeType(lower_bound( X, X+_npts, zeta ) - X) ;
    if ( interval > 0 ) --interval ;
    if ( X[interval] == X[interval+1] ) ++interval ; // degenerate interval for duplicated nodes
    if ( interval >= _npts-1 ) interval = _npts-2 ;

    // compute intersection
    valueType a  = _X[interval] ;
    valueType b  = _X[interval+1] ;
    valueType ya = X[interval] ;
    valueType yb = X[interval+1] ;
    valueType DX = b-a ;
    valueType DY = yb-ya ;
    SPLINE_ASSERT( zeta >= ya && zeta <= yb,
                   "SplineSet, Bad interval [ " << ya << "," << yb << "] for zeta = " << zeta ) ;
    SPLINE_ASSERT( a < b,
                   "SplineSet, Bad x interval [ " << a << "," << b << "]" ) ;
    if ( S->type() == LINEAR_TYPE ) {
      x = a + (b-a)*(zeta-ya)/(yb-ya) ;
    } else {
      valueType const * dX = _Yp[sizeType(spl)] ;
      valueType        dya = dX[interval] ;
      valueType        dyb = dX[interval+1] ;
      valueType coeffs[4] = { ya-zeta, dya, (3*DY/DX-2*dya-dyb)/DX, (dyb+dya-2*DY/DX)/(DX*DX) } ;
      valueType real[3], imag[3] ;
      pair<int,int> icase = cubicRoots( coeffs, real, imag ) ;
      SPLINE_ASSERT( icase.first > 0,
                     "SplineSet, No intersection found with independent spline at zeta = " << zeta ) ;
      // cerca radice buona
      bool ok = false ;
      for ( indexType i = 0 ; i < icase.first && !ok ; ++i ) {
        ok = real[i] >= 0 && real[i] <= DX ;
        if ( ok ) x = a + real[i] ;
      }
      SPLINE_ASSERT( ok, "SplineSet, failed to find intersection with independent spline at zeta = " << zeta ) ;
    }
    return S ;
  }

  void
  SplineSet::eval2( sizeType            spl,
                    valueType           zeta,
                    vector<valueType> & vals ) const {
    valueType x ;
    intersect( spl, zeta, x ) ;
    vals.resize(_nspl) ;
    for ( sizeType i = 0 ; i < _nspl ; ++i )
      vals[i] = (*splines[i])(x) ;
  }

  void
  SplineSet::eval2( sizeType  spl,
                    valueType zeta,
                    valueType vals[],
                    indexType incy ) const {
    valueType x ;
    intersect( spl, zeta, x ) ;
    sizeType ii = 0 ;
    for ( sizeType i = 0 ; i < _nspl ; ++i, ii += sizeType(incy) )
      vals[ii] = (*splines[i])(x) ;
  }

  void
  SplineSet::eval2_D( sizeType            spl,
                      valueType           zeta,
                      vector<valueType> & vals ) const {
    valueType x ;
    Spline const * S = intersect( spl, zeta, x ) ;
    valueType ds = S->D(x) ;
    vals.resize(_nspl) ;
    for ( sizeType i = 0 ; i < _nspl ; ++i )
      vals[i] = splines[i]->D(x)/ds ;
  }

  void
  SplineSet::eval2_D( sizeType  spl,
                      valueType zeta,
                      valueType vals[],
                      indexType incy ) const {
    valueType x ;
    Spline const * S = intersect( spl, zeta, x ) ;
    valueType ds = S->D(x) ;
    sizeType ii = 0 ;
    for ( sizeType i = 0 ; i < _nspl ; ++i, ii += sizeType(incy) )
      vals[ii] = splines[i]->D(x)/ds ;
  }

  void
  SplineSet::eval2_DD( sizeType            spl,
                       valueType           zeta,
                       vector<valueType> & vals ) const {
    valueType x ;
    Spline const * S = intersect( spl, zeta, x ) ;
    valueType dt  = 1/S->D(x) ;
    valueType dt2 = dt*dt ;
    valueType ddt = -S->DD(x)*(dt*dt2) ;
    vals.resize(_nspl) ;
    for ( sizeType i = 0 ; i < _nspl ; ++i )
      vals[i] = splines[i]->DD(x)*dt2+splines[i]->D(x)*ddt ;
  }

  void
  SplineSet::eval2_DD( sizeType  spl,
                       valueType zeta,
                       valueType vals[],
                       indexType incy ) const {
    valueType x ;
    Spline const * S = intersect( spl, zeta, x ) ;
    valueType dt  = 1/S->D(x) ;
    valueType dt2 = dt*dt ;
    valueType ddt = -S->DD(x)*(dt*dt2) ;
    sizeType ii = 0 ;
    for ( sizeType i = 0 ; i < _nspl ; ++i, ii += sizeType(incy) )
      vals[ii] = splines[i]->DD(x)*dt2+splines[i]->D(x)*ddt ;
  }

  void
  SplineSet::eval2_DDD( sizeType            spl,
                        valueType           zeta,
                        vector<valueType> & vals ) const {
    valueType x ;
    Spline const * S = intersect( spl, zeta, x ) ;
    valueType dt  = 1/S->D(x) ;
    valueType dt3 = dt*dt*dt ;
    valueType ddt = -S->DD(x)*dt3 ;
    valueType dddt = 3*(ddt*ddt)/dt-S->DDD(x)*(dt*dt3) ;
    vals.resize(_nspl) ;
    for ( sizeType i = 0 ; i < _nspl ; ++i )
      vals[i] = splines[i]->DDD(x)*dt3 +
                3*splines[i]->DD(x)*dt*ddt +
                splines[i]->D(x)*dddt ;
  }

  void
  SplineSet::eval2_DDD( sizeType  spl,
                        valueType zeta,
                        valueType vals[],
                        indexType incy ) const {
    valueType x ;
    Spline const * S = intersect( spl, zeta, x ) ;
    valueType dt  = 1/S->D(x) ;
    valueType dt3 = dt*dt*dt ;
    valueType ddt = -S->DD(x)*dt3 ;
    valueType dddt = 3*(ddt*ddt)/dt-S->DDD(x)*(dt*dt3) ;
    sizeType ii = 0 ;
    for ( sizeType i = 0 ; i < _nspl ; ++i, ii += sizeType(incy) )
      vals[ii] = splines[i]->DDD(x)*dt3 +
                 3*splines[i]->DD(x)*dt*ddt +
                 splines[i]->D(x)*dddt ;
  }

  valueType
  SplineSet::eval2( valueType    zeta,
                    char const * indep,
                    char const * name ) const {
    vector<valueType> vals ;
    eval2( getPosition(indep), zeta, vals ) ;
    return vals[unsigned(getPosition(name))] ;
  }

  valueType
  SplineSet::eval2_D( valueType    zeta,
                      char const * indep,
                      char const * name ) const {
    vector<valueType> vals ;
    eval2_D( getPosition(indep), zeta, vals ) ;
    return vals[unsigned(getPosition(name))] ;
  }

  valueType
  SplineSet::eval2_DD( valueType    zeta,
                       char const * indep,
                       char const * name ) const {
    vector<valueType> vals ;
    eval2_DD( getPosition(indep), zeta, vals ) ;
    return vals[unsigned(getPosition(name))] ;
  }

  valueType
  SplineSet::eval2_DDD( valueType    zeta,
                        char const * indep,
                        char const * name ) const {
    vector<valueType> vals ;
    eval2_DDD( getPosition(indep), zeta, vals ) ;
    return vals[unsigned(getPosition(name))] ;
  }

  valueType
  SplineSet::eval2( valueType zeta, sizeType indep, sizeType spl ) const {
    vector<valueType> vals ;
    eval2( indep, zeta, vals ) ;
    return vals[spl] ;
  }

  valueType
  SplineSet::eval2_D( valueType zeta, sizeType indep, sizeType spl ) const {
    vector<valueType> vals ;
    eval2_D( indep, zeta, vals ) ;
    return vals[spl] ;
  }

  valueType
  SplineSet::eval2_DD( valueType zeta, sizeType indep, sizeType spl ) const {
    vector<valueType> vals ;
    eval2_DD( indep, zeta, vals ) ;
    return vals[spl] ;
  }

  valueType
  SplineSet::eval2_DDD( valueType zeta, sizeType indep, sizeType spl ) const {
    vector<valueType> vals ;
    eval2_DDD( indep, zeta, vals ) ;
    return vals[spl] ;
  }
}
