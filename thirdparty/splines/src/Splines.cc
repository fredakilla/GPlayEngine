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
/*!
 *
 * \date     October 28, 2015
 * \version  5.1
 * \note     first release Jan 12, 1998
 *
 * \author   Enrico Bertolazzi
 *
 * \par      Affiliation:
 *           Department of Industrial Engineering<br>
 *           University of Trento<br>
 *           Via Sommarive 9, I -- 38123 Trento, Italy <br>
 *           enrico.bertolazzi@unitn.it
 *
 */

#include "Splines.hh"
#include <cmath>
#include <limits> // std::numeric_limits

#ifdef __GCC__
#pragma GCC diagnostic ignored "-Wc++98-compat"
#endif
#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++98-compat"
#endif

//! Various kind of splines
namespace Splines {

  using std::abs ;

  // cbrt is not available on WINDOWS? or C++ < C++11?
  #ifdef _MSC_VER
    using std::sqrt ;
    using std::pow ;
    static
    inline
    valueType
    cbrt( valueType x )
    { return pow( x, 1.0/3.0 ) ; }
  #else
    using std::sqrt ;
    using std::pow ;
    #if __cplusplus <= 199711L
      static
      inline
      valueType
      cbrt( valueType x )
      { return pow( x, 1.0/3.0 ) ; }
    #else
      using std::pow ;
    #endif
  #endif

  char const * spline_type[] = {
    "constant",    // 0
    "linear",      // 1
    "cubic",       // 2
    "akima",       // 3
    "bessel",      // 4
    "pchip",       // 5
    "quintic",     // 6
    "hermite",     // 7
    "spline set",  // 8
    "b-spline",    // 9
    nullptr
  } ;
  
  SplineType
  string_to_splineType( std::string const & nin ) {
    std::string n = nin ;
    std::transform(n.begin(), n.end(), n.begin(), ::tolower) ;
    for ( sizeType j = 0 ; spline_type[j] != nullptr ; ++j ) {
      if ( Splines::spline_type[j] == n ) return SplineType(j) ;
    }
    std::ostringstream ost ;
    ost << "string_to_splineType(" << n << ") unknown type\n" ;
    throw std::runtime_error(ost.str()) ;
  }

  static valueType const machineEps = std::numeric_limits<valueType>::epsilon() ;
  static valueType const m_2pi      = 6.28318530717958647692528676656  ; // 2*pi

  //! quadratic polinomial roots
  /*!
    Compute the roots of the polynomial
    
    \f[ a_0 + a_1 z + a_2 z^2 \f]
    
    and store the results is `real` and `imag`.
    It is assumed that \f$ a_2 \f$ is nonzero.
  */
  /*
    Converted to be compatible with ELF90 by Alan Miller
    amiller @ bigpond.net.au
    WWW-page: http://users.bigpond.net.au/amiller
    Latest revision - 27 February 1997
  */

  // num real roots, num complex root
  pair<int,int>
  quadraticRoots( valueType const a[3],
                  valueType       real[2], 
                  valueType       imag[2] ) {

    // A x^2 + B x + C
    valueType const & C = a[0] ;
    valueType const & B = a[1] ;
    valueType const & A = a[2] ;

    real[0] = real[1] = imag[0] = imag[1] = 0 ;

    pair<int,int> res(0,0) ;
    if ( a[0] == 0 ) {
      real[0] = -B/A ;
      res.first = 1 ; // una singola radice reale
    } else {
      valueType twoA = 2*A ;
      valueType d    = B*B - 4*A*C ;
      valueType absd = abs(d) ;
      if ( absd <= 2*machineEps*B*B ) { 
        real[0] = -B/twoA ; // EQUAL REAL ROOTS
        res.first = 1 ; // 2 radici reali coincidenti
      } else {
        valueType r = sqrt(absd) ;
        if ( d < 0 ) { // COMPLEX ROOTS
          real[0] = real[1] = -B/twoA ;
          imag[0] = abs(r/twoA) ;
          imag[1] = -imag[0] ;
          res.second = 2 ; // 2 radici complesse coniugate
        } else {
          // DISTINCT REAL ROOTS
          if ( B == 0  ) {
            real[0] = abs(r/twoA) ;
            real[1] = -real[0] ;
          } else {
            valueType w = -B ;
            if ( w > 0 ) w += r ; else w -= r ;
            w *= 0.5 ;
            real[0] = C/w ;
            real[1] = w/A ;
          }
          res.first = 2 ; // 2 radici reali distinte
        }
      }
    }
    return res ;
  }
  
  //! cubic polinomial roots
  /*!
    Compute the roots of the polynomial
    
    \f[ a_0 + a_1 z + a_2 z^2 + a_3 z^3 \f]
    
    and store the results is `real` and `imag`.
    It is assumed that \f$ a_3 \f$ is nonzero.
  */

  pair<int,int>
  cubicRoots( valueType const a[4],
              valueType       real[3], 
              valueType       imag[3] ) {

    // initialize roots
    real[0] = real[1] = real[2] = 
    imag[0] = imag[1] = imag[2] = 0 ;

    // trivial case
    if ( a[0] == 0 ) {
      pair<int,int> res = quadraticRoots( a+1, real+1, imag+1 ) ; // quadratica degenerata
      ++res.first ;
      return res ;
    }

    // trivial case
    if ( a[3] == 0 ) return quadraticRoots( a, real, imag ) ; // cubica degenerata

    // x^3 + A x^2 + B x + C
    valueType const C = a[0]/a[3] ;
    valueType const B = a[1]/a[3] ;
    valueType const A = a[2]/a[3] ;
    
    // p(y-A/3) = y^3 + p*y + q
    valueType const A3 = A/3 ;
    valueType const p  = B-A*A3 ;
    valueType const q  = C+A3*(2*(A3*A3)-B) ;
    
    // scaling equation p(S*z)/S^3 = z^3 + 3*(p/S^2/3)*z + 2*(q/S^3/2)
    valueType const S = max( sqrt(abs(p)), cbrt(abs(q)) ) ;

    // check for a triple root
    if ( S <= machineEps ) {
      real[0] = -A3 ;
      return pair<int,int>(1,0) ; // 3 radici reali coincidenti
    }

    valueType const P     = (p/3)/S/S ;
    valueType const sqrtP = sqrt(abs(p/3))/S ;
    valueType const Q     = (q/2)/S/S/S ;

    valueType const d     = P*P*P + Q*Q ;
    valueType const sqrtd = sqrt(abs(d)) ;

    pair<int,int> res(0,0) ;
    if ( sqrtd < abs(q)*machineEps ) {
      // P^3 = - Q^2
      // (x+2*a)(x-a)^2 = x^3 - 3*x*a^2 + 2*a^3
      // cioè -a^2 = P, a^3 = Q ==> a = sqrt(-P)
      valueType tmp = Q > 0 ? sqrtP : -sqrtP ;
      real[0] = tmp ;
      real[1] = -2*tmp ;
      res.first = 2 ; // 3 radici reali, 2 coincidenti
    } else if ( d > 0 ) {
      // w1 = (- Q + sqrt( P^3 + Q^2 ))^(1/3)
      // w2 = (- Q - sqrt( P^3 + Q^2 ))^(1/3)
      valueType w1, w2 ;
      if ( Q > 0 ) {
        w2 = - pow( sqrtd + Q, 1.0 / 3.0 ) ;
        w1 = - P / w2 ;
      } else {
        w1 =   pow( sqrtd - Q, 1.0 / 3.0 ) ;
        w2 = - P / w1 ;
      }
      real[0] = w1 + w2 ;
      real[1] =
      real[2] = -0.5*real[0] ;
      imag[1] = (w1-w2)*sqrt(3.0/4.0) ;
      imag[2] = -imag[1] ;
      res.first  = 1 ;
      res.second = 2 ; // 1 reale 2 complesse coniugate
    } else { // 3 radici reali
      // w1 = (- Q + I*sqrt(|P^3 + Q^2|) )^(1/3)
      // w2 = (- Q - I*sqrt(|P^3 + Q^2|) )^(1/3)
      valueType angle  = atan2( sqrtd, -Q ) ;
      if ( angle < 0 ) angle += m_2pi ;
      angle /= 3 ;
      valueType re = sqrtP * cos(angle) ;
      valueType im = sqrtP * sin(angle) ;
      //if ( Q > 0 ) re = -re ;
      real[0]  = 2*re ;
      real[1]  = real[2] = -re ;
      real[1] += sqrt(3.0) * im ;
      real[2] -= sqrt(3.0) * im ;
      res.first = 3 ; // 3 radici reali distinte
    }

    for ( indexType i = 0 ; i < res.first+res.second ; ++i ) {
      // scalo radici
      real[i] *= S ;
      imag[i] *= S ;
      // traslo radici
      real[i] -= A3 ;
    }

    return res ;
  }

  //! Check if cubic spline with this data is monotone, -2 non monotone data, -1 no, 0 yes, 1 strictly monotone
  indexType
  checkCubicSplineMonotonicity( valueType const X[],
                                valueType const Y[],
                                valueType const Yp[],
                                sizeType        npts ) {
    // check monotonicity of data: (assuming X monotone)
    indexType flag = 1 ;
    for ( sizeType i = 1 ; i < npts ; ++i ) {
      if ( Y[i-1] > Y[i] ) return -2 ; // non monotone data
      if ( Y[i-1] == Y[i] && X[i-1] < X[i] ) flag = 0 ; // non strict monotone
    }
    // pag 146 Methods of Shape-Preserving Spline Approximation, K
    for ( sizeType i = 1 ; i < npts ; ++i ) {
      if ( X[i] <= X[i-1] ) continue ; // skip duplicate points
      valueType dd = (Y[i]-Y[i-1])/(X[i]-X[i-1]) ;
      valueType m0 = Yp[i-1]/dd ;
      valueType m1 = Yp[i]/dd ;
      if ( m0 < 0 || m1 < 0 ) return -1 ; // non monotone
      if ( m0 <= 3 && m1 <= 3 ) {
        if ( flag > 0 && i > 1      && (m0 == 0 || m0 == 3) ) flag = 0 ;
        if ( flag > 0 && i < npts-1 && (m1 == 0 || m1 == 3) ) flag = 0 ;
      } else {
        valueType tmp1 = 2*m0+m1-3 ;
        valueType tmp2 = 2*(m0+m1-2) ;
        valueType tmp3 = m0*tmp2-(tmp1*tmp1)  ;
        if ( tmp2 >= 0 ) {
          if ( tmp3 < 0 ) return -1 ; // non monotone spline
        } else {
          if ( tmp3 > 0 ) return -1 ;
        }
        if ( tmp3 == 0 ) flag = 0 ;
      }
    }
    return flag ; // passed all check
  }

  void
  updateInterval( sizeType      & lastInterval,
                  valueType       x,
                  valueType const X[],
                  sizeType        npts ) {

    if ( npts <= 2 ) { lastInterval = 0 ; return ; } // nothing to search

    // find the interval of the support of the B-spline
    valueType const * XL = X+lastInterval ;
    if ( XL[1] <= x ) { // x on the right
      if ( x >= X[npts-2] ) { // x in [X[npt-2],X[npts-1]]
        lastInterval = npts-2 ; // last interval
      } else if ( x < XL[2] ) { // x in [XL[1],XL[2])
        ++lastInterval ;
      } else { // x >= XL[2] search the right interval
        valueType const * XE = X+npts ;
        lastInterval += sizeType(std::lower_bound( XL, XE, x )-XL) ;
        if ( X[lastInterval] > x ) --lastInterval ;
      }
    } else if ( x < XL[0] ) { // on the left
      if ( x < X[1] ) { // x in [X[0],X[1])
        lastInterval = 0 ; // first interval
      } else if ( XL[-1] <= x ) { // x in [XL[-1],XL[0])
        --lastInterval ;
      } else {
        lastInterval = sizeType(std::lower_bound( X, XL, x )-X) ;
        if ( X[lastInterval] > x ) --lastInterval ;
      }
    } else {
      // x in the interval [XL[0],XL[1]) nothing to do
    }
  }

  void
  Spline::info( std::basic_ostream<char> & s ) const {
    s << "Spline `" << _name
      << "` of type: " << type_name()
      << " of order: " << order() ;
    if ( npts > 0 )
      s << "\nxMin = " << xMin() << " xMax = " << xMax()
        << "\nyMin = " << yMin() << " yMax = " << yMax() ;
    s << '\n' ;
  }

  void
  Spline::pushBack( valueType x, valueType y ) {
    if ( npts > 0 ) {
      SPLINE_ASSERT( x >= X[npts-1], // ammetto punti doppi
                     "Spline::pushBack, non monotone insert at insert N. " << npts <<
                     "\nX[ " << npts-1 << "] = " << X[npts-1] <<
                     "\nX[ " << npts   << "] = " << x ) ;
    }
    if ( npts_reserved == 0 ) {
      reserve( 2 ) ;
    } else if ( npts >= npts_reserved ) {
      // riallocazione & copia
      sizeType saved_npts = npts ; // salvo npts perche reserve lo azzera
      vector<valueType> Xsaved(npts) ;
      vector<valueType> Ysaved(npts) ;

      std::copy( X, X+npts, Xsaved.begin() ) ;
      std::copy( Y, Y+npts, Ysaved.begin() ) ;
      reserve( (npts+1) * 2 ) ;
      npts = saved_npts ;
      std::copy( Xsaved.begin(), Xsaved.end(), X ) ;
      std::copy( Ysaved.begin(), Ysaved.end(), Y ) ;
    }
    X[npts] = x ;
    Y[npts] = y ;
    ++npts ;
  }

  ///////////////////////////////////////////////////////////////////////////
  void
  Spline::setOrigin( valueType x0 ) {
    valueType Tx = x0 - X[0] ;
    valueType *ix = X ;
    while ( ix < X+npts ) *ix++ += Tx ;
  }

  void
  Spline::setRange( valueType xmin, valueType xmax ) {
    SPLINE_ASSERT( xmax > xmin, "Spline::setRange( " << xmin << " , " << xmax << " ) bad range ") ;
    valueType S  = (xmax - xmin) / ( X[npts-1] - X[0] ) ;
    valueType Tx = xmin - S * X[0] ;
    for( valueType *ix = X ; ix < X+npts ; ++ix ) *ix = *ix * S + Tx ;
  }

  ///////////////////////////////////////////////////////////////////////////
  void
  Spline::dump( ostream & s, sizeType nintervals, char const header[] ) const {
    s << header << '\n' ;
    valueType dx = (xMax()-xMin())/nintervals ;
    for ( sizeType i = 0 ; i <= nintervals ; ++i ) {
      valueType x = xMin() + i*dx ;
      s << x << '\t' << (*this)(x) << '\n' ;
    }
  }

  /*
  //    ____  ____   ____                               _
  //   / ___|/ ___| / ___| _   _ _ __  _ __   ___  _ __| |_
  //  | |  _| |     \___ \| | | | '_ \| '_ \ / _ \| '__| __|
  //  | |_| | |___   ___) | |_| | |_) | |_) | (_) | |  | |_
  //   \____|\____| |____/ \__,_| .__/| .__/ \___/|_|   \__|
  //                            |_|   |_|
  */

  #ifndef SPLINES_DO_NOT_USE_GENERIC_CONTAINER

  using GenericContainerNamespace::GC_VEC_REAL ;
  using GenericContainerNamespace::vec_real_type ;

  void
  Spline::setup( GenericContainer const & gc ) {
    /*
    // gc["x"]
    // gc["y"]
    //
    */
    SPLINE_ASSERT( gc.exists("x"), "Spline[" << _name << "]::setup missing `x` field!") ;
    SPLINE_ASSERT( gc.exists("y"), "Spline[" << _name << "]::setup missing `y` field!") ;

    GenericContainer const & gc_x = gc("x") ;
    GenericContainer const & gc_y = gc("y") ;

    vec_real_type x, y ;
    {
      std::ostringstream ost ;
      ost << "Spline[" << _name << "]::setup, field `x'" ;
      gc_x.copyto_vec_real ( x, ost.str().c_str() ) ;
    }
    {
      std::ostringstream ost ;
      ost << "Spline[" << _name << "]::setup, field `y'" ;
      gc_y.copyto_vec_real ( y, ost.str().c_str() ) ;
    }
    build( x, y ) ;
  }
  #endif

}
