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
/****************************************************************************\
Copyright (c) 2016, Enrico Bertolazzi
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

#ifndef SPLINES_HH
#define SPLINES_HH

// Uncomment this if you do not want that Splines uses GenericContainer
#define SPLINES_DO_NOT_USE_GENERIC_CONTAINER 1

// some one may force the use of GenericContainer
#ifndef SPLINES_DO_NOT_USE_GENERIC_CONTAINER
  #include "GenericContainer.hh"
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <utility>      // std::pair
#include <algorithm>

//
// file: Splines
//
// if C++ < C++11 define nullptr
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
  #if _MSC_VER >= 1900
    #ifndef DO_NOT_USE_CXX11
      #define SPLINES_USE_CXX11
    #endif
  #else
    #include <cstdlib>
    #ifndef nullptr
      #include <cstddef>
      #define nullptr NULL
    #endif
  #endif
  #ifdef _MSC_VER
    #include <math.h>
  #endif
#else
  #if __cplusplus > 199711L
    #ifndef DO_NOT_USE_CXX11
      #define SPLINES_USE_CXX11
    #endif
  #else
    #include <cstdlib>
    #ifndef nullptr
      #include <cstddef>
      #define nullptr NULL
    #endif
  #endif
#endif

#ifndef SPLINE_ASSERT
  #include <stdexcept>
  #include <sstream>
  #define SPLINE_ASSERT(COND,MSG)           \
    if ( !(COND) ) {                        \
      std::ostringstream ost ;              \
      ost << "In spline: " << name()        \
          << " line: " << __LINE__          \
          << " file: " << __FILE__          \
          << '\n' << MSG << '\n' ;          \
      throw std::runtime_error(ost.str()) ; \
    }
#endif

#ifndef SPLINE_WARNING
  #include <stdexcept>
  #include <sstream>
  #define SPLINE_WARNING(COND,MSG)         \
    if ( !(COND) ) {                       \
      std::cout << "In spline: " << name() \
                << " line: " << __LINE__   \
                << " file: " << __FILE__   \
                << MSG << '\n' ;           \
    }
#endif

#ifdef DEBUG
  #define SPLINE_CHECK_NAN( PTR, MSG, DIM ) Splines::checkNaN( PTR, MSG, DIM )
#else
  #define SPLINE_CHECK_NAN( PTR, MSG, DIM )
#endif

#ifdef __GCC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#pragma GCC diagnostic ignored "-Wc++98-compat"
#endif
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wc++98-compat"
#endif

//! Various kind of splines
namespace Splines {

  using namespace ::std ; // load standard namespace

  typedef double   valueType ; //!< Floating point type for splines
  typedef unsigned sizeType  ; //!< Unsigned integer type for splines
  typedef int      indexType ; //!< Signed integer type for splines

  //! Associate a number for each type of splines implemented
  typedef enum { CONSTANT_TYPE   = 0,
                 LINEAR_TYPE     = 1,
                 CUBIC_TYPE      = 2,
                 AKIMA_TYPE      = 3,
                 BESSEL_TYPE     = 4,
                 PCHIP_TYPE      = 5,
                 QUINTIC_TYPE    = 6,
                 HERMITE_TYPE    = 7,
                 SPLINE_SET_TYPE = 8,
                 BSPLINE_TYPE    = 9 } SplineType ;

  extern char const *spline_type[] ;
  
  extern SplineType string_to_splineType( std::string const & n ) ;

  #ifndef SPLINES_DO_NOT_USE_GENERIC_CONTAINER
  using GenericContainerNamespace::GenericContainer ;
  using GenericContainerNamespace::vec_real_type ;
  using GenericContainerNamespace::vec_string_type ;
  using GenericContainerNamespace::vector_type ;
  using GenericContainerNamespace::map_type ;
  #endif

  pair<int,int>
  quadraticRoots( valueType const a[3],
                  valueType       real[2], 
                  valueType       imag[2] ) ;

  pair<int,int>
  cubicRoots( valueType const a[4],
              valueType       real[3], 
              valueType       imag[3] ) ;

  /*       _               _    _   _       _   _
  //   ___| |__   ___  ___| | _| \ | | __ _| \ | |
  //  / __| '_ \ / _ \/ __| |/ /  \| |/ _` |  \| |
  // | (__| | | |  __/ (__|   <| |\  | (_| | |\  |
  //  \___|_| |_|\___|\___|_|\_\_| \_|\__,_|_| \_|
  */
  void
  checkNaN( valueType const pv[],
            char      const v_name[],
            sizeType  const DIM ) ;

  /*
  //   _   _                     _ _       
  //  | | | | ___ _ __ _ __ ___ (_) |_ ___ 
  //  | |_| |/ _ \ '__| '_ ` _ \| | __/ _ \
  //  |  _  |  __/ |  | | | | | | | ||  __/
  //  |_| |_|\___|_|  |_| |_| |_|_|\__\___|
  */
  void Hermite3    ( valueType const x, valueType const H, valueType base[4]     ) ;
  void Hermite3_D  ( valueType const x, valueType const H, valueType base_D[4]   ) ;
  void Hermite3_DD ( valueType const x, valueType const H, valueType base_DD[4]  ) ;
  void Hermite3_DDD( valueType const x, valueType const H, valueType base_DDD[4] ) ;

  void Hermite5      ( valueType const x, valueType const H, valueType base[6]       ) ;
  void Hermite5_D    ( valueType const x, valueType const H, valueType base_D[6]     ) ;
  void Hermite5_DD   ( valueType const x, valueType const H, valueType base_DD[6]    ) ;
  void Hermite5_DDD  ( valueType const x, valueType const H, valueType base_DDD[6]   ) ;
  void Hermite5_DDDD ( valueType const x, valueType const H, valueType base_DDDD[6]  ) ;
  void Hermite5_DDDDD( valueType const x, valueType const H, valueType base_DDDDD[6] ) ;

  /*
  //   ____  _ _ _
  //  | __ )(_) (_)_ __   ___  __ _ _ __
  //  |  _ \| | | | '_ \ / _ \/ _` | '__|
  //  | |_) | | | | | | |  __/ (_| | |
  //  |____/|_|_|_|_| |_|\___|\__,_|_|
  */
  valueType
  bilinear3( valueType const p[4],
             valueType const M[4][4],
             valueType const q[4] ) ;

  valueType
  bilinear5( valueType const p[6],
             valueType const M[6][6],
             valueType const q[6] ) ;

  //! Check if cubic spline with this data is monotone, -1 no, 0 yes, 1 strictly monotone
  indexType
  checkCubicSplineMonotonicity( valueType const X[],
                                valueType const Y[],
                                valueType const Yp[],
                                sizeType        npts ) ;

  void
  updateInterval( sizeType      & lastInterval,
                  valueType       x,
                  valueType const X[],
                  sizeType        npts ) ;

  /*
  //   __  __       _ _
  //  |  \/  | __ _| | | ___   ___
  //  | |\/| |/ _` | | |/ _ \ / __|
  //  | |  | | (_| | | | (_) | (__
  //  |_|  |_|\__,_|_|_|\___/ \___|
  */

  //! Allocate memory
  template <typename T>
  class SplineMalloc {
    typedef T                 valueType           ;
    typedef valueType*        valuePointer        ;
    typedef const valueType*  valueConstPointer   ;
    typedef valueType&        valueReference      ;
    typedef const valueType&  valueConstReference ;

    typedef long              indexType           ;
    typedef indexType*        indexPointer        ;
    typedef const indexType*  indexConstPointer   ;
    typedef indexType&        indexReference      ;
    typedef const indexType&  indexConstReference ;

  private:

    std::string  _name ;
    size_t       numTotValues ;
    size_t       numTotReserved ;
    size_t       numAllocated ;
    valuePointer pMalloc ;

    SplineMalloc(SplineMalloc<T> const &) ; // block copy constructor
    SplineMalloc<T> const & operator = (SplineMalloc<T> &) const ; // block copy constructor

  public:

    //! malloc object constructor
    explicit
    SplineMalloc( std::string const & __name )
    : _name(__name)
    , numTotValues(0)
    , numTotReserved(0)
    , numAllocated(0)
    , pMalloc(nullptr)
    {}

    //! malloc object destructor
    ~SplineMalloc()
    { free() ; }

    //! allocate memory for `n` objects
    void
    allocate( size_t n ) {
      try {
        if ( n > numTotReserved ) {
          delete [] pMalloc ;
          numTotValues   = n ;
          numTotReserved = n + (n>>3) ; // 12% more values
          pMalloc = new T[numTotReserved] ;
        }
      }
      catch ( std::exception const & exc ) {
        std::cerr << "Memory allocation failed: " << exc.what()
                  << "\nTry to allocate " << n << " bytes for " << _name
                  << '\n' ;
        exit(0) ;
      }
      catch (...) {
        std::cerr << "SplineMalloc allocation failed for " << _name << ": memory exausted\n"
                  << "Requesting " << n << " blocks\n";
        exit(0) ;
      }
      numTotValues = n ;
      numAllocated = 0 ;
    }

    //! free memory
    void
    free(void) {
      if ( pMalloc != 0 ) {
        delete [] pMalloc ;
        numTotValues   = 0 ;
        numTotReserved = 0 ;
        numAllocated   = 0 ;
        pMalloc        = nullptr ;
      }
    }

    //! number of objects allocated
    indexType size(void) const { return numTotValues ; }

    //! get pointer of allocated memory for `sz` objets
    T * operator () ( size_t sz ) {
      size_t offs = numAllocated ;
      numAllocated += sz ;
      if ( numAllocated > numTotValues ) {
        std::ostringstream ost ;
        ost << "\nMalloc<" << _name << ">::operator () (" << sz << ") -- SplineMalloc EXAUSTED\n"
            << "request = " << numAllocated << " > " << numTotValues << " = available\n" ;
        throw std::runtime_error(ost.str()) ;
      }
      return pMalloc + offs ;
    }
  } ;

  /*
  //   ____        _ _            
  //  / ___| _ __ | (_)_ __   ___ 
  //  \___ \| '_ \| | | '_ \ / _ \
  //   ___) | |_) | | | | | |  __/
  //  |____/| .__/|_|_|_| |_|\___|
  //        |_|                   
  */
  //! Spline Management Class
  class Spline {
  protected:
  
    string _name ;
    bool   _check_range ;

    sizeType  npts, npts_reserved ;
    valueType *X ; // allocated in the derived class!
    valueType *Y ; // allocated in the derived class!
    
    mutable sizeType lastInterval ;

    sizeType
    search( valueType x ) const {
      SPLINE_ASSERT( npts > 1, "\nsearch(" << x << ") empty spline");
      if ( _check_range ) {
        valueType xl = X[0] ;
        valueType xr = X[npts-1] ;
        SPLINE_ASSERT( x >= xl && x <= xr,
                       "method search( " << x << " ) out of range: [" <<
                       xl << ", " << xr << "]" ) ;
      }
      Splines::updateInterval( lastInterval, x, X, npts ) ;
      return lastInterval;
    }

    Spline(Spline const &) ; // block copy constructor
    Spline const & operator = (Spline const &) ; // block copy method

  public:

    //! spline constructor
    Spline( string const & name = "Spline", bool ck = false )
    : _name(name)
    , _check_range(ck)
    , npts(0)
    , npts_reserved(0)
    , X(nullptr)
    , Y(nullptr)
    , lastInterval(0)
    {}

    //! spline destructor
    virtual 
    ~Spline()
    {}
    
    string const & name() const { return _name ; }

    void setCheckRange( bool ck ) { _check_range = ck ; }
    bool getCheckRange() const { return _check_range ; }

    //! return the number of support points of the spline.
    sizeType numPoints(void) const { return npts ; }

    //! return the i-th node of the spline (x component).
    valueType xNode( sizeType i ) const { return X[i] ; }

    //! return the i-th node of the spline (y component).
    valueType yNode( sizeType i ) const { return Y[i] ; }

    //! return first node of the spline (x component).
    valueType xBegin() const { return X[0] ; }

    //! return first node of the spline (y component).
    valueType yBegin() const { return Y[0] ; }

    //! return last node of the spline (x component).
    valueType xEnd() const { return X[npts-1] ; }

    //! return last node of the spline (y component).
    valueType yEnd() const { return Y[npts-1] ; }

    //! Allocate memory for `npts` points
    virtual void reserve( sizeType npts ) = 0 ;

    //! Add a support point (x,y) to the spline.
    void pushBack( valueType x, valueType y ) ;

    //! Drop a support point to the spline.
    void dropBack() {
      if ( npts > 0 ) --npts ;
    }

    //! Build a spline.
    // must be defined in derived classes
    virtual
    void
    build (void) = 0 ;

    #ifndef SPLINES_DO_NOT_USE_GENERIC_CONTAINER
    virtual void setup ( GenericContainer const & gc ) ;
    void build ( GenericContainer const & gc ) { setup(gc) ; }
    #endif

    //! Build a spline.
    /*!
     * \param x    vector of x-coordinates
     * \param incx access elements as x[0], x[incx], x[2*incx],...
     * \param y    vector of y-coordinates
     * \param incy access elements as y[0], y[incy], x[2*incy],...
     * \param n    total number of points
     */
    // must be defined in derived classes
    virtual
    void
    build( valueType const x[], sizeType incx,
           valueType const y[], sizeType incy,
           sizeType n ) {
      reserve( n ) ;
      for ( sizeType i = 0 ; i < n ; ++i ) X[i] = x[i*incx] ;
      for ( sizeType i = 0 ; i < n ; ++i ) Y[i] = y[i*incy] ;
      npts = n ;
      build() ;
    }

    //! Build a spline.
    /*!
     * \param x vector of x-coordinates
     * \param y vector of y-coordinates
     * \param n total number of points
     */
    inline
    void
    build( valueType const x[], valueType const y[], sizeType n )
    { build( x, 1, y, 1, n ) ; }

    //! Build a spline.
    /*!
     * \param x vector of x-coordinates
     * \param y vector of y-coordinates
     */
    inline
    void
    build ( vector<valueType> const & x, vector<valueType> const & y ) {
      sizeType N = sizeType(x.size()) ;
      if ( N > sizeType(y.size()) ) N = sizeType(y.size()) ;
      build( &x.front(), 1, &y.front(), 1, N ) ;
    }

    //! Cancel the support points, empty the spline.
    virtual
    void
    clear(void) = 0 ;

    //! return x-minumum spline value
    valueType xMin() const { return X[0] ; }

    //! return x-maximum spline value
    valueType xMax() const { return X[npts-1] ; }

    //! return y-minumum spline value
    valueType yMin() const {
      sizeType N = sizeType(npts) ;
      if ( type() == CONSTANT_TYPE ) --N ;
      return *std::min_element(Y,Y+N) ;
    }

    //! return y-maximum spline value
    valueType yMax() const {
      sizeType N = sizeType(npts) ;
      if ( type() == CONSTANT_TYPE ) --N ;
      return *std::max_element(Y,Y+N) ;
    }

    ///////////////////////////////////////////////////////////////////////////
    //! change X-origin of the spline
    void setOrigin( valueType x0 ) ;

    //! change X-range of the spline
    void setRange( valueType xmin, valueType xmax ) ;

    ///////////////////////////////////////////////////////////////////////////
    //! dump a sample of the spline
    void dump( ostream & s, sizeType nintervals, char const header[] = "x\ty" ) const ;

    void
    dump( char const fname[], sizeType nintervals, char const header[] = "x\ty" ) const
    { ofstream file(fname) ; dump( file, nintervals, header ) ; file.close() ; }

    ///////////////////////////////////////////////////////////////////////////
    //! Evaluate spline value
    virtual valueType operator () ( valueType x ) const = 0 ;

    //! First derivative
    virtual valueType D( valueType x ) const = 0 ;

    //! Second derivative
    virtual valueType DD( valueType x ) const = 0 ;

    //! Third derivative
    virtual valueType DDD( valueType x ) const = 0 ;

    //! Some aliases
    valueType eval( valueType x ) const { return (*this)(x) ; }
    valueType eval_D( valueType x ) const { return D(x) ; }
    valueType eval_DD( valueType x ) const { return DD(x) ; }
    valueType eval_DDD( valueType x ) const { return DDD(x) ; }

    //! get the piecewise polinomials of the spline
    virtual
    sizeType // order
    coeffs( valueType cfs[], valueType nodes[], bool transpose = false ) const = 0 ;

    virtual
    sizeType // order
    order() const = 0 ;

    //! Print spline information
    virtual void info( std::basic_ostream<char> & s ) const ;

    //! Print spline coefficients
    virtual void writeToStream( std::basic_ostream<char> & s ) const = 0 ;

    //! Return spline typename
    char const * type_name() const { return Splines::spline_type[type()] ; }

    //! Return spline type (as number)
    virtual unsigned type() const = 0 ;
    
    friend class SplineSet ;

  } ;

  /*
  //   ____                  _ _
  //  | __ )       ___ _ __ | (_)_ __   ___
  //  |  _ \ _____/ __| '_ \| | | '_ \ / _ \
  //  | |_) |_____\__ \ |_) | | | | | |  __/
  //  |____/      |___/ .__/|_|_|_| |_|\___|
  //                  |_|
  */

  //! B-spline base class
  template <int _degree>
  class BSpline : public Spline {
  protected:
    SplineMalloc<valueType> baseValue ;
    valueType * knots ;
    valueType * yPolygon ;
    bool        _external_alloc ;

    sizeType knot_search( valueType x ) const ;
    
    // extrapolation
    valueType s_L,   s_R   ;
    valueType ds_L,  ds_R  ;
    valueType dds_L, dds_R ;

  public:
  
    using Spline::build ;

    //! spline constructor
    BSpline( string const & name = "BSpline", bool ck = false )
    : Spline(name,ck)
    , baseValue(name+"_memory")
    , knots(nullptr)
    , yPolygon(nullptr)
    , _external_alloc(false)
    {}

    virtual
    ~BSpline()
    {}

    void copySpline( BSpline const & S ) ;

    //! return the i-th node of the spline (y' component).
    valueType yPoly( sizeType i ) const { return yPolygon[i] ; }

    //! change X-range of the spline
    void setRange( valueType xmin, valueType xmax ) ;

    //! Use externally allocated memory for `npts` points
    void reserve_external( sizeType     n,
                           valueType *& p_x,
                           valueType *& p_y,
                           valueType *& p_knots,
                           valueType *& p_yPolygon ) ;

    // --------------------------- VIRTUALS -----------------------------------

    //! Return spline type (as number)
    virtual unsigned type() const { return BSPLINE_TYPE ; }

    //! Evaluate spline value
    virtual valueType operator () ( valueType x ) const ;

    //! First derivative
    virtual valueType D( valueType x ) const ;

    //! Second derivative
    virtual valueType DD( valueType x ) const ;

    //! Third derivative
    virtual valueType DDD( valueType x ) const ;

    //! Print spline coefficients
    virtual void writeToStream( std::basic_ostream<char> & s ) const ;

    // --------------------------- VIRTUALS -----------------------------------

    //! Allocate memory for `npts` points
    virtual void reserve( sizeType npts ) ;

    virtual
    void
    build(void) ;

    //! Cancel the support points, empty the spline.
    virtual
    void
    clear(void) ;

    //! get the piecewise polinomials of the spline
    virtual
    sizeType // order
    coeffs( valueType cfs[], valueType nodes[], bool transpose = false ) const ;

    virtual
    sizeType // order
    order() const { return _degree+1 ; }

    // ---------------------------  UTILS  -----------------------------------

    //! Evaluate spline value
    static
    valueType eval( valueType x, sizeType n, valueType const Knots[], valueType const yPolygon[] ) ;

    //! First derivative
    static
    valueType eval_D( valueType x, sizeType n, valueType const Knots[], valueType const yPolygon[] ) ;

    //! Second derivative
    static
    valueType eval_DD( valueType x, sizeType n, valueType const Knots[], valueType const yPolygon[] ) ;

    //! Third derivative
    static
    valueType eval_DDD( valueType x, sizeType n, valueType const Knots[], valueType const yPolygon[] ) ;

    //! B-spline bases
    void bases( valueType x, valueType vals[] ) const ;
    void bases_D( valueType x, valueType vals[] ) const ;
    void bases_DD( valueType x, valueType vals[] ) const ;
    void bases_DDD( valueType x, valueType vals[] ) const ;

    sizeType bases_nz( valueType x, valueType vals[] ) const ;
    sizeType bases_D_nz( valueType x, valueType vals[] ) const ;
    sizeType bases_DD_nz( valueType x, valueType vals[] ) const ;
    sizeType bases_DDD_nz( valueType x, valueType vals[] ) const ;

    // Utilities
    static
    void
    knots_sequence( sizeType        n,
                    valueType const X[],
                    valueType     * Knots ) ;

    static
    void
    sample_bases( indexType           nx, // number of sample points
                  valueType const     X[],
                  indexType           nb, // number of bases
                  valueType const     Knots[],
                  vector<indexType> & II, // GCC on linux bugged for I
                  vector<indexType> & JJ,
                  vector<valueType> & vals ) ;

  } ;

  /*
  //    ____      _     _        ____        _ _              ____                 
  //   / ___|   _| |__ (_) ___  / ___| _ __ | (_)_ __   ___  | __ )  __ _ ___  ___ 
  //  | |  | | | | '_ \| |/ __| \___ \| '_ \| | | '_ \ / _ \ |  _ \ / _` / __|/ _ \
  //  | |__| |_| | |_) | | (__   ___) | |_) | | | | | |  __/ | |_) | (_| \__ \  __/
  //   \____\__,_|_.__/|_|\___| |____/| .__/|_|_|_| |_|\___| |____/ \__,_|___/\___|
  //                                  |_|
  */
  //! cubic spline base class
  class CubicSplineBase : public Spline {
  protected:
    SplineMalloc<valueType> baseValue ;
    mutable valueType base[4] ;
    mutable valueType base_D[4] ;
    mutable valueType base_DD[4] ;
    mutable valueType base_DDD[4] ;

    valueType         * Yp ;
    bool              _external_alloc ;

  public:

    using Spline::build ;

    //! spline constructor
    CubicSplineBase( string const & name = "CubicSplineBase", bool ck = false )
    : Spline(name,ck)
    , baseValue(name+"_memory")
    , Yp(nullptr)
    , _external_alloc(false)
    {}
    
    virtual
    ~CubicSplineBase()
    {}

    void copySpline( CubicSplineBase const & S ) ;

    //! return the i-th node of the spline (y' component).
    valueType ypNode( sizeType i ) const { return Yp[i] ; }

    //! change X-range of the spline
    void setRange( valueType xmin, valueType xmax ) ;

    //! Use externally allocated memory for `npts` points
    void reserve_external( sizeType     n,
                           valueType *& p_x,
                           valueType *& p_y,
                           valueType *& p_dy ) ;

    // --------------------------- VIRTUALS -----------------------------------
    //! Evaluate spline value
    virtual valueType operator () ( valueType x ) const ;

    //! First derivative
    virtual valueType D( valueType x ) const ;

    //! Second derivative
    virtual valueType DD( valueType x ) const ;

    //! Third derivative
    virtual valueType DDD( valueType x ) const ;

    //! Print spline coefficients
    virtual void writeToStream( std::basic_ostream<char> & s ) const ;

    // --------------------------- VIRTUALS -----------------------------------

    //! Allocate memory for `npts` points
    virtual void reserve( sizeType npts ) ;

    //! Build a spline.
    /*!
     * \param x     vector of x-coordinates
     * \param incx  access elements as `x[0]`, `x[incx]`, `x[2*incx]`,...
     * \param y     vector of y-coordinates
     * \param incy  access elements as `y[0]`, `y[incy]`, `x[2*incy]`,...
     * \param yp    vector of y'-coordinates
     * \param incyp access elements as `yp[0]`, `yp[incy]`, `xp[2*incy]`,...
     * \param n     total number of points
     */
    void
    build( valueType const x[],  sizeType incx,
           valueType const y[],  sizeType incy,
           valueType const yp[], sizeType incyp,
           sizeType n ) ;

    //! Build a spline.
    /*!
     * \param x  vector of x-coordinates
     * \param y  vector of y-coordinates
     * \param yp vector of y'-coordinates
     * \param n  total number of points
     */
    inline
    void
    build( valueType const x[],
           valueType const y[],
           valueType const yp[],
           sizeType n ) {
      build ( x, 1, y, 1, yp, 1, n ) ;
    }

    //! Build a spline.
    /*!
     * \param x  vector of x-coordinates
     * \param y  vector of y-coordinates
     * \param yp vector of y'-coordinates
     */
    inline
    void
    build ( vector<valueType> const & x,
            vector<valueType> const & y,
            vector<valueType> const & yp ) {
      sizeType N = sizeType(x.size()) ;
      if ( N > sizeType(y.size())  ) N = sizeType(y.size()) ;
      if ( N > sizeType(yp.size()) ) N = sizeType(yp.size()) ;
      build ( &x.front(), 1,
              &y.front(), 1,
              &yp.front(), 1,
              N ) ;
    }

    //! Cancel the support points, empty the spline.
    virtual
    void
    clear(void) ;

    //! get the piecewise polinomials of the spline
    virtual
    sizeType // order
    coeffs( valueType cfs[], valueType nodes[], bool transpose = false ) const ;

    virtual
    sizeType // order
    order() const ;

  } ;

  /*
  //    ____      _     _      ____        _ _            
  //   / ___|   _| |__ (_) ___/ ___| _ __ | (_)_ __   ___ 
  //  | |  | | | | '_ \| |/ __\___ \| '_ \| | | '_ \ / _ \
  //  | |__| |_| | |_) | | (__ ___) | |_) | | | | | |  __/
  //   \____\__,_|_.__/|_|\___|____/| .__/|_|_|_| |_|\___|
  //                                |_|                   
  */
  //! Cubic Spline Management Class
  class CubicSpline : public CubicSplineBase {
  private:
    valueType ddy0 ;
    valueType ddyn ;
  public:

    using CubicSplineBase::build ;
    using CubicSplineBase::reserve ;

    //! spline constructor
    CubicSpline( string const & name = "CubicSpline", bool ck = false )
    : CubicSplineBase( name, ck )
    , ddy0(0)
    , ddyn(0)
    {}

    //! spline destructor
    virtual
    ~CubicSpline()
    {}

    /*!
     * \param _ddy0  first boundary condition.
     *               The second derivative at initial point.
     * \param _ddyn  second boundary condition.
     *               The second derivative at final point.
     */
    void
    setbc( valueType _ddy0, valueType _ddyn ) {
      this -> ddy0 = _ddy0 ;
      this -> ddyn = _ddyn ;    
    }

    //! Return spline type (as number)
    virtual unsigned type() const { return CUBIC_TYPE ; }

    // --------------------------- VIRTUALS -----------------------------------

    virtual
    void
    build (void) ;

    #ifndef SPLINES_DO_NOT_USE_GENERIC_CONTAINER
    virtual void setup ( GenericContainer const & gc ) ;
    #endif

  } ;

  /*
  //      _    _    _                   ____        _ _            
  //     / \  | | _(_)_ __ ___   __ _  / ___| _ __ | (_)_ __   ___ 
  //    / _ \ | |/ / | '_ ` _ \ / _` | \___ \| '_ \| | | '_ \ / _ \
  //   / ___ \|   <| | | | | | | (_| |  ___) | |_) | | | | | |  __/
  //  /_/   \_\_|\_\_|_| |_| |_|\__,_| |____/| .__/|_|_|_| |_|\___|
  //                                         |_|                   
  */
  //! Akima spline class
  /*!
   *  Reference
   *  =========
   *  Hiroshi Akima, Journal of the ACM, Vol. 17, No. 4, October 1970, pages 589-602.
   */
  class AkimaSpline : public CubicSplineBase {
  public:

    using CubicSplineBase::build ;
    using CubicSplineBase::reserve ;

    //! spline constructor
    AkimaSpline( string const & name = "AkimaSpline", bool ck = false )
    : CubicSplineBase( name, ck )
    {}

    //! spline destructor
    virtual
    ~AkimaSpline()
    {}

    //! Return spline type (as number)
    virtual unsigned type() const { return AKIMA_TYPE ; }

    // --------------------------- VIRTUALS -----------------------------------

    //! Build an Akima spline from previously inserted points
    virtual
    void
    build (void) ;

  } ;

  /*
  //   ____                     _ ____        _ _            
  //  | __ )  ___  ___ ___  ___| / ___| _ __ | (_)_ __   ___ 
  //  |  _ \ / _ \/ __/ __|/ _ \ \___ \| '_ \| | | '_ \ / _ \
  //  | |_) |  __/\__ \__ \  __/ |___) | |_) | | | | | |  __/
  //  |____/ \___||___/___/\___|_|____/| .__/|_|_|_| |_|\___|
  //                                   |_|                   
  */
  //! Bessel spline class
  class BesselSpline : public CubicSplineBase {
  public:

    using CubicSplineBase::build ;
    using CubicSplineBase::reserve ;

    //! spline constructor
    BesselSpline( string const & name = "BesselSpline", bool ck = false )
    : CubicSplineBase( name, ck )
    {}

    //! spline destructor
    virtual
    ~BesselSpline()
    {}

    //! Return spline type (as number)
    virtual unsigned type() const { return BESSEL_TYPE ; }

    // --------------------------- VIRTUALS -----------------------------------

    //! Build a Bessel spline from previously inserted points
    virtual
    void
    build (void) ;
  } ;

  /*
  //   ____      _     _      ____        _ _            
  //  |  _ \ ___| |__ (_)_ __/ ___| _ __ | (_)_ __   ___ 
  //  | |_) / __| '_ \| | '_ \___ \| '_ \| | | '_ \ / _ \
  //  |  __/ (__| | | | | |_) |__) | |_) | | | | | |  __/
  //  |_|   \___|_| |_|_| .__/____/| .__/|_|_|_| |_|\___|
  //                    |_|        |_|                   
  */
  void
  pchip( valueType const X[],
         valueType const Y[],
         valueType       Yp[],
         sizeType        n ) ;

  //! Pchip (Piecewise Cubic Hermite Interpolating Polynomial) spline class
  class PchipSpline : public CubicSplineBase {
  public:

    using CubicSplineBase::build ;
    using CubicSplineBase::reserve ;

    //! spline constructor
    PchipSpline( string const & name = "PchipSpline", bool ck = false )
    : CubicSplineBase( name, ck )
    {}

    //! spline destructor
    virtual
    ~PchipSpline()
    {}

    //! Return spline type (as number)
    virtual unsigned type() const { return PCHIP_TYPE ; }

    // --------------------------- VIRTUALS -----------------------------------

    //! Build a Monotone spline from previously inserted points
    virtual
    void
    build (void) ;
  } ;

  /*
  //   _     _                       ____        _ _            
  //  | |   (_)_ __   ___  __ _ _ __/ ___| _ __ | (_)_ __   ___ 
  //  | |   | | '_ \ / _ \/ _` | '__\___ \| '_ \| | | '_ \ / _ \
  //  | |___| | | | |  __/ (_| | |   ___) | |_) | | | | | |  __/
  //  |_____|_|_| |_|\___|\__,_|_|  |____/| .__/|_|_|_| |_|\___|
  //                                      |_|                   
  */
  //! Linear spline class
  class LinearSpline : public Spline {
    SplineMalloc<valueType> baseValue ;
    bool                    _external_alloc ;
  public:

    using Spline::build ;

    LinearSpline( string const & name = "LinearSpline", bool ck = false )
    : Spline(name,ck)
    , baseValue( name+"_memory")
    , _external_alloc(false)
    {}

    virtual
    ~LinearSpline()
    {}

    //! Use externally allocated memory for `npts` points
    void reserve_external( sizeType n, valueType *& p_x, valueType *& p_y ) ;

    // --------------------------- VIRTUALS -----------------------------------

    //! Evalute spline value at `x`
    virtual
    valueType
    operator () ( valueType x ) const {
      SPLINE_ASSERT( npts > 0, "in LinearSpline::operator(), npts == 0!") ;
      if ( x < X[0]      ) return Y[0] ;
      if ( x > X[npts-1] ) return Y[npts-1] ;
      sizeType  i = search(x) ;
      valueType s = (x-X[i])/(X[i+1] - X[i]) ;
      return (1-s)*Y[i] + s * Y[i+1] ;
    }

    //! First derivative
    virtual
    valueType
    D( valueType x ) const {
      SPLINE_ASSERT( npts > 0, "in LinearSpline::operator(), npts == 0!") ;
      if ( x < X[0]      ) return 0 ;
      if ( x > X[npts-1] ) return 0 ;
      sizeType i = search(x) ;
      return ( Y[i+1] - Y[i] ) / ( X[i+1] - X[i] ) ;
    }

    //! Second derivative
    virtual valueType DD( valueType ) const { return 0 ; }

    //! Third derivative
    virtual valueType DDD( valueType ) const { return 0 ; }

    //! Print spline coefficients
    virtual void writeToStream( std::basic_ostream<char> & s ) const ;

    //! Return spline type (as number)
    virtual unsigned type() const { return LINEAR_TYPE ; }

    // --------------------------- VIRTUALS -----------------------------------

    //! Allocate memory for `npts` points
    virtual void reserve( sizeType npts ) ;

    //! added for compatibility with cubic splines
    virtual
    void
    build (void)
    {}

    //! Cancel the support points, empty the spline.
    virtual
    void
    clear(void) ;

    //! get the piecewise polinomials of the spline
    virtual
    sizeType // order
    coeffs( valueType cfs[], valueType nodes[], bool transpose = false ) const ;

    virtual
    sizeType // order
    order() const ;

  } ;

  /*  
  //    ____                _              _       ____        _ _            
  //   / ___|___  _ __  ___| |_ __ _ _ __ | |_ ___/ ___| _ __ | (_)_ __   ___ 
  //  | |   / _ \| '_ \/ __| __/ _` | '_ \| __/ __\___ \| '_ \| | | '_ \ / _ \
  //  | |__| (_) | | | \__ \ || (_| | | | | |_\__ \___) | |_) | | | | | |  __/
  //   \____\___/|_| |_|___/\__\__,_|_| |_|\__|___/____/| .__/|_|_|_| |_|\___|
  //                                                    |_|                   
  */
  //! Picewise constants spline class
  class ConstantSpline : public Spline {
    SplineMalloc<valueType> baseValue ;
    bool                    _external_alloc ;
  public:

    using Spline::build ;

    ConstantSpline( string const & name = "ConstantSpline", bool ck = false )
    : Spline(name,ck)
    , baseValue(name+"_memory")
    , _external_alloc(false)
    {}

    ~ConstantSpline() {}

    //! Use externally allocated memory for `npts` points
    void reserve_external( sizeType n, valueType *& p_x, valueType *& p_y ) ;

    // --------------------------- VIRTUALS -----------------------------------
    //! Build a spline.
    virtual
    void
    build (void)
    {} // nothing to do

    virtual
    void
    build( valueType const x[], sizeType incx,
           valueType const y[], sizeType incy,
           sizeType n ) ;

    //! Evaluate spline value at `x`
    virtual valueType operator () ( valueType x ) const ;

    //! First derivative
    virtual valueType D( valueType ) const { return 0 ; }
    
    //! Second derivative
    virtual valueType DD( valueType ) const { return 0 ; }

    //! Third derivative
    virtual valueType DDD( valueType ) const { return 0 ; }

    //! Print spline coefficients
    virtual void writeToStream( std::basic_ostream<char> & ) const ;

    //! Return spline type (as number)
    virtual unsigned type() const { return CONSTANT_TYPE ; }

    // --------------------------- VIRTUALS -----------------------------------

    //! Allocate memory for `npts` points
    virtual void reserve( sizeType npts ) ;

    //! Cancel the support points, empty the spline.
    virtual
    void
    clear(void) ;

    //! get the piecewise polinomials of the spline
    virtual
    sizeType // order
    coeffs( valueType cfs[], valueType nodes[], bool transpose = false ) const ;

    virtual
    sizeType // order
    order() const ;

  } ;
  
  /*
  //    _   _                     _ _       ____        _ _
  //   | | | | ___ _ __ _ __ ___ (_) |_ ___/ ___| _ __ | (_)_ __   ___
  //   | |_| |/ _ \ '__| '_ ` _ \| | __/ _ \___ \| '_ \| | | '_ \ / _ \
  //   |  _  |  __/ |  | | | | | | | ||  __/___) | |_) | | | | | |  __/
  //   |_| |_|\___|_|  |_| |_| |_|_|\__\___|____/| .__/|_|_|_| |_|\___|
  //                                             |_|
  */
  //! Hermite Spline Management Class
  class HermiteSpline : public CubicSplineBase {
  public:

    using CubicSplineBase::build ;
    using CubicSplineBase::reserve ;

    //! spline constructor
    HermiteSpline( string const & name = "HermiteSpline", bool ck = false )
    : CubicSplineBase( name, ck )
    {}

    //! spline destructor
    virtual
    ~HermiteSpline()
    {}

    //! Return spline type (as number)
    virtual unsigned type() const { return HERMITE_TYPE ; }

    // --------------------------- VIRTUALS -----------------------------------

    virtual
    void
    build(void)
    {} // nothing to do

    #ifndef SPLINES_DO_NOT_USE_GENERIC_CONTAINER
    virtual void setup( GenericContainer const & gc ) ;
    #endif

    // block method!
    virtual
    void
    build( valueType const [], sizeType,
           valueType const [], sizeType,
           sizeType ) ;
  } ;

  /*
  //    ___        _       _   _      ____        _ _            ____                 
  //   / _ \ _   _(_)_ __ | |_(_) ___/ ___| _ __ | (_)_ __   ___| __ )  __ _ ___  ___ 
  //  | | | | | | | | '_ \| __| |/ __\___ \| '_ \| | | '_ \ / _ \  _ \ / _` / __|/ _ \
  //  | |_| | |_| | | | | | |_| | (__ ___) | |_) | | | | | |  __/ |_) | (_| \__ \  __/
  //   \__\_\\__,_|_|_| |_|\__|_|\___|____/| .__/|_|_|_| |_|\___|____/ \__,_|___/\___|
  //                                       |_|                                        
  //  
  */
  //! cubic quintic base class
  class QuinticSplineBase : public Spline {
  protected:
    SplineMalloc<valueType> baseValue ;
    mutable valueType base[6] ;
    mutable valueType base_D[6] ;
    mutable valueType base_DD[6] ;
    mutable valueType base_DDD[6] ;
    mutable valueType base_DDDD[6] ;
    mutable valueType base_DDDDD[6] ;

    valueType * Yp ;
    valueType * Ypp ;
    bool      _external_alloc ;

  public:

    using Spline::build ;

    //! spline constructor
    QuinticSplineBase( string const & name = "Spline", bool ck = false )
    : Spline(name,ck)
    , baseValue(name+"_memeory")
    , Yp(nullptr)
    , Ypp(nullptr)
    , _external_alloc(false)
    {}
    
    virtual
    ~QuinticSplineBase()
    {}

    void copySpline( QuinticSplineBase const & S ) ;

    //! return the i-th node of the spline (y' component).
    valueType ypNode( sizeType i ) const { return Yp[i] ; }

    //! return the i-th node of the spline (y'' component).
    valueType yppNode( sizeType i ) const { return Ypp[i] ; }

    //! change X-range of the spline
    void setRange( valueType xmin, valueType xmax ) ;

    //! Use externally allocated memory for `npts` points
    void reserve_external( sizeType     n,
                           valueType *& p_x,
                           valueType *& p_y,
                           valueType *& p_Yp,
                           valueType *& p_Ypp ) ;

    // --------------------------- VIRTUALS -----------------------------------

    //! Evaluate spline value
    virtual valueType operator () ( valueType x ) const ;

    //! First derivative
    virtual valueType D( valueType x ) const ;

    //! Second derivative
    virtual valueType DD( valueType x ) const ;

    //! Third derivative
    virtual valueType DDD( valueType x ) const ;

    //! Fourth derivative
    virtual valueType DDDD( valueType x ) const ;

    //! Fifth derivative
    virtual valueType DDDDD( valueType x ) const ;

    //! Print spline coefficients
    virtual void writeToStream( std::basic_ostream<char> & s ) const ;

    //! Return spline type (as number)
    virtual unsigned type() const { return QUINTIC_TYPE ; }

    // --------------------------- VIRTUALS -----------------------------------

    //! Allocate memory for `npts` points
    virtual void reserve( sizeType npts ) ;

    //! Cancel the support points, empty the spline.
    virtual
    void
    clear(void) ;

    //! get the piecewise polinomials of the spline
    virtual
    sizeType // order
    coeffs( valueType cfs[], valueType nodes[], bool transpose = false ) const ;

    virtual
    sizeType // order
    order() const ;

  } ;
 
  /*
  //    ___        _       _   _      ____        _ _            
  //   / _ \ _   _(_)_ __ | |_(_) ___/ ___| _ __ | (_)_ __   ___ 
  //  | | | | | | | | '_ \| __| |/ __\___ \| '_ \| | | '_ \ / _ \
  //  | |_| | |_| | | | | | |_| | (__ ___) | |_) | | | | | |  __/
  //   \__\_\\__,_|_|_| |_|\__|_|\___|____/| .__/|_|_|_| |_|\___|
  //                                       |_|                   
  //  
  */
  //! Quintic spline class
  class QuinticSpline : public QuinticSplineBase {
  public:

    using Spline::build ;
    using QuinticSplineBase::reserve ;

    //! spline constructor
    QuinticSpline( string const & name = "Spline", bool ck = false )
    : QuinticSplineBase( name, ck )
    {}

    //! spline destructor
    virtual
    ~QuinticSpline()
    {}

    // --------------------------- VIRTUALS -----------------------------------
    //! Build a Monotone quintic spline from previously inserted points
    virtual
    void
    build (void) ;
  } ;

  /*
  //   ____        _ _            ____       _
  //  / ___| _ __ | (_)_ __   ___/ ___|  ___| |_
  //  \___ \| '_ \| | | '_ \ / _ \___ \ / _ \ __|
  //   ___) | |_) | | | | | |  __/___) |  __/ |_
  //  |____/| .__/|_|_|_| |_|\___|____/ \___|\__|
  //        |_|
  */

  //! Splines Management Class
  class SplineSet {

    SplineSet(SplineSet const &) ; // block copy constructor
    SplineSet const & operator = (SplineSet const &) ; // block copy method

  protected:

    string const _name ;

    SplineMalloc<valueType>  baseValue ;
    SplineMalloc<valueType*> basePointer ;
    
    sizeType _npts ;
    sizeType _nspl ;

    valueType *  _X ;
    valueType ** _Y ;
    valueType ** _Yp ;
    valueType ** _Ypp ;
    valueType *  _Ymin ;
    valueType *  _Ymax ;

    mutable sizeType lastInterval ;
    sizeType search( valueType x ) const ;
    
    vector<Spline*>      splines ;
    vector<int>          is_monotone ;
    map<string,sizeType> header_to_position ;

  private:

    //! find `x` value such that the monotone spline `(spline[spl])(x)` intersect the value `zeta`
    Spline const * intersect( sizeType spl, valueType zeta, valueType & x ) const ;

  public:

    //! spline constructor
    SplineSet( string const & name = "SplineSet" ) ;

    //! spline destructor
    virtual ~SplineSet() ;

    string const & name() const { return _name ; }
    string const & header( sizeType const i ) const { return splines[i]->name() ; }

    // return +1 = strict monotone, 0 weak monotone, -1 non monotone
    int isMonotone( sizeType const i ) const { return is_monotone[i] ; }

    //! return the number of support points of the splines
    sizeType numPoints(void) const { return _npts ; }

    //! return the number splines in the spline set
    sizeType numSplines(void) const { return _nspl ; }

    //! return the column with header(i) == hdr, return -1 if not found
    sizeType getPosition( char const * hdr ) const ;

    //! return the vector of values of x-nodes
    valueType const * xNodes() const { return _X ; }

    //! return the vector of values of x-nodes
    valueType const * yNodes( sizeType i ) const {
      SPLINE_ASSERT( i >=0 && i < _nspl,
                     "SplineSet::yNodes( " << i << ") argument out of range [0," << _nspl-1 << "]" ) ;
      return _Y[i] ;
    }

    //! return the i-th node of the spline (x component).
    valueType xNode( sizeType npt ) const { return _X[npt] ; }

    //! return the i-th node of the spline (y component).
    valueType yNode( sizeType npt, sizeType spl ) const { return _Y[spl][npt] ; }

    //! return x-minumum spline value
    valueType xMin() const { return _X[0] ; }

    //! return x-maximum spline value
    valueType xMax() const { return _X[_npts-1] ; }

    //! return y-minumum spline value
    valueType yMin( sizeType spl ) const { return _Ymin[spl] ; }

    //! return y-maximum spline value
    valueType yMax( sizeType spl ) const { return _Ymax[spl] ; }

    //! return y-minumum spline value
    valueType yMin( char const spl[] ) const { return _Ymin[getPosition(spl)] ; }

    //! return y-maximum spline value
    valueType yMax( char const spl[] ) const { return _Ymax[getPosition(spl)] ; }

    //! Return pointer to the `i`-th spline
    Spline * getSpline( sizeType i ) const {
      SPLINE_ASSERT( i < _nspl, "SplineSet::getSpline( " << i << ") argument out of range [0," << _nspl-1 << "]" ) ;
      return splines[i] ;
    }

    //! Return pointer to the `i`-th spline
    Spline * getSpline( char const * hdr ) const {
      return splines[getPosition(hdr)] ;
    }

    //! Evaluate spline value
    valueType operator () ( valueType x, sizeType spl ) const { return (*getSpline(spl))(x) ; }
    valueType eval( valueType x, sizeType spl ) const { return (*getSpline(spl))(x) ; }

    //! First derivative
    valueType D( valueType x, sizeType spl ) const { return getSpline(spl)->D(x) ; }
    valueType eval_D( valueType x, sizeType spl ) const { return getSpline(spl)->D(x) ; }

    //! Second derivative
    valueType DD( valueType x, sizeType spl ) const { return getSpline(spl)->DD(x) ; }
    valueType eval_DD( valueType x, sizeType spl ) const { return getSpline(spl)->DD(x) ; }

    //! Third derivative
    valueType DDD( valueType x, sizeType spl ) const { return getSpline(spl)->DDD(x) ; }
    valueType eval_DDD( valueType x, sizeType spl ) const { return getSpline(spl)->DDD(x) ; }

    //! Evaluate spline value
    valueType eval( valueType x, char const * name ) const { return (*getSpline(name))(x) ; }

    //! First derivative
    valueType eval_D( valueType x, char const * name ) const { return getSpline(name)->D(x) ; }

    //! Second derivative
    valueType eval_DD( valueType x, char const * name ) const { return getSpline(name)->DD(x) ; }

    //! Third derivative
    valueType eval_DDD( valueType x, char const * name ) const { return getSpline(name)->DDD(x) ; }

    // vectorial values
    //! fill a vector of strings with the names of the splines
    void getHeaders( vector<string> & h ) const ;

    //! Evaluate all the splines at `x`
    void eval( valueType x, vector<valueType> & vals ) const ;
    //! Evaluate all the splines at `x`
    void eval( valueType x, valueType vals[], indexType incy = 1 ) const ;

    //! Evaluate the fist derivative of all the splines at `x`
    void eval_D( valueType x, vector<valueType> & vals ) const ;
    //! Evaluate the fist derivative of all the splines at `x`
    void eval_D( valueType x, valueType vals[], indexType incy = 1 ) const ;

    //! Evaluate the second derivative of all the splines at `x`
    void eval_DD( valueType x, vector<valueType> & vals ) const ;
    //! Evaluate the second derivative of all the splines at `x`
    void eval_DD( valueType x, valueType vals[], indexType incy = 1 ) const ;

    //! Evaluate the third derivative of all the splines at `x`
    void eval_DDD( valueType x, vector<valueType> & vals ) const ;
    //! Evaluate the third derivative of all the splines at `x`
    void eval_DDD( valueType x, valueType vals[], indexType incy = 1 ) const ;

    // change independent variable
    //! Evaluate all the splines at `zeta` using spline[spl] as independent
    void eval2( sizeType spl, valueType zeta, vector<valueType> & vals ) const ;
    //! Evaluate all the splines at `zeta` using spline[spl] as independent
    void eval2( sizeType spl, valueType zeta, valueType vals[], indexType incy = 1 ) const ;

    //! Evaluate the fist derivative of all the splines at `zeta` using spline[spl] as independent
    void eval2_D( sizeType spl, valueType zeta, vector<valueType> & vals ) const ;
    //! Evaluate the fist derivative of all the splines at `zeta` using spline[spl] as independent
    void eval2_D( sizeType spl, valueType zeta, valueType vals[], indexType incy = 1 ) const ;

    //! Evaluate the second derivative of all the splines at `zeta` using spline[spl] as independent
    void eval2_DD( sizeType spl, valueType zeta, vector<valueType> & vals ) const ;
    //! Evaluate the second derivative of all the splines at `zeta` using spline[spl] as independent
    void eval2_DD( sizeType spl, valueType zeta, valueType vals[], indexType incy = 1 ) const ;

    //! Evaluate the third derivative of all the splines at `zeta` using spline[spl] as independent
    void eval2_DDD( sizeType spl, valueType zeta, vector<valueType> & vals ) const ;
    //! Evaluate the sizeType derivative of all the splines at `zeta` using spline[spl] as independent
    void eval2_DDD( sizeType spl, valueType zeta, valueType vals[], indexType incy = 1 ) const ;

    //! Evaluate the spline `name` at `zeta` using spline `indep` as independent
    valueType eval2( valueType zeta, char const * indep, char const * name ) const ;

    //! Evaluate first derivative of the spline `name` at `zeta` using spline `indep` as independent
    valueType eval2_D( valueType zeta, char const * indep, char const * name ) const ;

    //! Evaluate second derivative of the spline `name` at `zeta` using spline `indep` as independent
    valueType eval2_DD( valueType zeta, char const * indep, char const * name ) const ;

    //! Evaluate third derivative of the spline `name` at `zeta` using spline `indep` as independent
    valueType eval2_DDD( valueType zeta, char const * indep, char const * name ) const ;

    //! Evaluate the spline `spl` at `zeta` using spline `indep` as independent
    valueType eval2( valueType zeta, sizeType indep, sizeType spl ) const ;

    //! Evaluate first derivative of the spline `spl` at `zeta` using spline `indep` as independent
    valueType eval2_D( valueType zeta, sizeType indep, sizeType spl ) const ;

    //! Evaluate second derivative of the spline `spl` at `zeta` using spline `indep` as independent
    valueType eval2_DD( valueType zeta, sizeType indep, sizeType spl ) const ;

    //! Evaluate third derivative of the spline `spl` at `zeta` using spline `indep` as independent
    valueType eval2_DDD( valueType zeta, sizeType indep, sizeType spl ) const ;

    // interface with GenericContainer
    #ifndef SPLINES_DO_NOT_USE_GENERIC_CONTAINER
    //! Evaluate all the splines at `x` and fill a map of values in a GenericContainer
    void eval( valueType x, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `x` values contained in vec and fill a map of vector in a GenericContainer
    void eval( vec_real_type const & vec, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `x` and fill a map of values in a GenericContainer with keys in `columns`
    void eval( valueType x, vec_string_type const & columns, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `x` values contained in vec and fill a map of vector in a GenericContainer with keys in `columns`
    void eval( vec_real_type const & vec, vec_string_type const & columns, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `zeta` and fill a map of values in a GenericContainer and `indep` as independent spline
    void eval2( valueType zeta, sizeType indep, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `zeta` values contained in vec and fill a map of vector in a GenericContainer and `indep` as independent spline
    void eval2( vec_real_type const & zetas, sizeType indep, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `zeta` and fill a map of values in a GenericContainer with keys in `columns` and `indep` as independent spline
    void eval2( valueType zeta, sizeType indep, vec_string_type const & columns, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `zeta` values contained in vec and fill a map of vector in a GenericContainer with keys in `columns` and `indep` as independent spline
    void eval2( vec_real_type const & zetas, sizeType indep, vec_string_type const & columns, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `zeta` and fill a map of values in a GenericContainer and `indep` as independent spline
    void eval2( valueType zeta, char const * indep, GenericContainer & vals ) const
    { eval2( zeta, getPosition(indep), vals ) ; }

    //! Evaluate all the splines at `zeta` values contained in vec and fill a map of vector in a GenericContainer and `indep` as independent spline
    void eval2( vec_real_type const & zetas, char const * indep, GenericContainer & vals ) const
    { eval2( zetas, getPosition(indep), vals ) ; }

    //! Evaluate all the splines at `zeta` and fill a map of values in a GenericContainer with keys in `columns` and `indep` as independent spline
    void eval2( valueType zeta, char const * indep, vec_string_type const & columns, GenericContainer & vals ) const
    { eval2( zeta, getPosition(indep), columns, vals ) ; }

    //! Evaluate all the splines at `zeta` values contained in vec and fill a map of vector in a GenericContainer with keys in `columns` and `indep` as independent spline
    void eval2( vec_real_type const & zetas, char const * indep, vec_string_type const & columns, GenericContainer & vals ) const
    { eval2( zetas, getPosition(indep), columns, vals ) ; }

    //! Evaluate all the splines at `x` and fill a map of values in a GenericContainer
    void eval_D( valueType x, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `x` values contained in vec and fill a map of vector in a GenericContainer
    void eval_D( vec_real_type const & vec, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `x` and fill a map of values in a GenericContainer with keys in `columns`
    void eval_D( valueType x, vec_string_type const & columns, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `x` values contained in vec and fill a map of vector in a GenericContainer with keys in `columns`
    void eval_D( vec_real_type const & vec, vec_string_type const & columns, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `zeta` and fill a map of values in a GenericContainer and `indep` as independent spline
    void eval2_D( valueType zeta, sizeType indep, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `zeta` values contained in vec and fill a map of vector in a GenericContainer and `indep` as independent spline
    void eval2_D( vec_real_type const & zetas, sizeType indep, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `zeta` and fill a map of values in a GenericContainer with keys in `columns` and `indep` as independent spline
    void eval2_D( valueType zeta, sizeType indep, vec_string_type const & columns, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `zeta` values contained in vec and fill a map of vector in a GenericContainer with keys in `columns` and `indep` as independent spline
    void eval2_D( vec_real_type const & zetas, sizeType indep, vec_string_type const & columns, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `zeta` and fill a map of values in a GenericContainer and `indep` as independent spline
    void eval2_D( valueType zeta, char const * indep, GenericContainer & vals ) const
    { eval2_D( zeta, getPosition(indep), vals ) ; }

    //! Evaluate all the splines at `zeta` values contained in vec and fill a map of vector in a GenericContainer and `indep` as independent spline
    void eval2_D( vec_real_type const & zetas, char const * indep, GenericContainer & vals ) const
    { eval2_D( zetas, getPosition(indep), vals ) ; }

    //! Evaluate all the splines at `zeta` and fill a map of values in a GenericContainer with keys in `columns` and `indep` as independent spline
    void eval2_D( valueType zeta, char const * indep, vec_string_type const & columns, GenericContainer & vals ) const
    { eval2_D( zeta, getPosition(indep), columns, vals ) ; }

    //! Evaluate all the splines at `zeta` values contained in vec and fill a map of vector in a GenericContainer with keys in `columns` and `indep` as independent spline
    void eval2_D( vec_real_type const & zetas, char const * indep, vec_string_type const & columns, GenericContainer & vals ) const
    { eval2_D( zetas, getPosition(indep), columns, vals ) ; }

    //! Evaluate all the splines at `x` and fill a map of values in a GenericContainer
    void eval_DD( valueType x, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `x` values contained in vec and fill a map of vector in a GenericContainer
    void eval_DD( vec_real_type const & vec, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `x` and fill a map of values in a GenericContainer with keys in `columns`
    void eval_DD( valueType x, vec_string_type const & columns, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `x` values contained in vec and fill a map of vector in a GenericContainer with keys in `columns`
    void eval_DD( vec_real_type const & vec, vec_string_type const & columns, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `zeta` and fill a map of values in a GenericContainer and `indep` as independent spline
    void eval2_DD( valueType zeta, sizeType indep, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `zeta` values contained in vec and fill a map of vector in a GenericContainer and `indep` as independent spline
    void eval2_DD( vec_real_type const & zetas, sizeType indep, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `zeta` and fill a map of values in a GenericContainer with keys in `columns` and `indep` as independent spline
    void eval2_DD( valueType zeta, sizeType indep, vec_string_type const & columns, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `zeta` values contained in vec and fill a map of vector in a GenericContainer with keys in `columns` and `indep` as independent spline
    void eval2_DD( vec_real_type const & zetas, sizeType indep, vec_string_type const & columns, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `zeta` and fill a map of values in a GenericContainer and `indep` as independent spline
    void eval2_DD( valueType zeta, char const * indep, GenericContainer & vals ) const
    { eval2_DD( zeta, getPosition(indep), vals ) ; }

    //! Evaluate all the splines at `zeta` values contained in vec and fill a map of vector in a GenericContainer and `indep` as independent spline
    void eval2_DD( vec_real_type const & zetas, char const * indep, GenericContainer & vals ) const
    { eval2_DD( zetas, getPosition(indep), vals ) ; }

    //! Evaluate all the splines at `zeta` and fill a map of values in a GenericContainer with keys in `columns` and `indep` as independent spline
    void eval2_DD( valueType zeta, char const * indep, vec_string_type const & columns, GenericContainer & vals ) const
    { eval2_DD( zeta, getPosition(indep), columns, vals ) ; }

    //! Evaluate all the splines at `zeta` values contained in vec and fill a map of vector in a GenericContainer with keys in `columns` and `indep` as independent spline
    void eval2_DD( vec_real_type const & zetas, char const * indep, vec_string_type const & columns, GenericContainer & vals ) const
    { eval2_DD( zetas, getPosition(indep), columns, vals ) ; }

    //! Evaluate all the splines at `x` and fill a map of values in a GenericContainer
    void eval_DDD( valueType x, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `x` values contained in vec and fill a map of vector in a GenericContainer
    void eval_DDD( vec_real_type const & vec, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `x` and fill a map of values in a GenericContainer with keys in `columns`
    void eval_DDD( valueType x, vec_string_type const & columns, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `x` values contained in vec and fill a map of vector in a GenericContainer with keys in `columns`
    void eval_DDD( vec_real_type const & vec, vec_string_type const & columns, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `zeta` and fill a map of values in a GenericContainer and `indep` as independent spline
    void eval2_DDD( valueType zeta, sizeType indep, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `zeta` values contained in vec and fill a map of vector in a GenericContainer and `indep` as independent spline
    void eval2_DDD( vec_real_type const & zetas, sizeType indep, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `zeta` and fill a map of values in a GenericContainer with keys in `columns` and `indep` as independent spline
    void eval2_DDD( valueType zeta, sizeType indep, vec_string_type const & columns, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `zeta` values contained in vec and fill a map of vector in a GenericContainer with keys in `columns` and `indep` as independent spline
    void eval2_DDD( vec_real_type const & zetas, sizeType indep, vec_string_type const & columns, GenericContainer & vals ) const ;

    //! Evaluate all the splines at `zeta` and fill a map of values in a GenericContainer and `indep` as independent spline
    void eval2_DDD( valueType zeta, char const * indep, GenericContainer & vals ) const
    { eval2_DD( zeta, getPosition(indep), vals ) ; }

    //! Evaluate all the splines at `zeta` values contained in vec and fill a map of vector in a GenericContainer and `indep` as independent spline
    void eval2_DDD( vec_real_type const & zetas, char const * indep, GenericContainer & vals ) const
    { eval2_DD( zetas, getPosition(indep), vals ) ; }

    //! Evaluate all the splines at `zeta` and fill a map of values in a GenericContainer with keys in `columns` and `indep` as independent spline
    void eval2_DDD( valueType zeta, char const * indep, vec_string_type const & columns, GenericContainer & vals ) const
    { eval2_DD( zeta, getPosition(indep), columns, vals ) ; }

    //! Evaluate all the splines at `zeta` values contained in vec and fill a map of vector in a GenericContainer with keys in `columns` and `indep` as independent spline
    void eval2_DDD( vec_real_type const & zetas, char const * indep, vec_string_type const & columns, GenericContainer & vals ) const
    { eval2_DD( zetas, getPosition(indep), columns, vals ) ; }

    #endif

    ///////////////////////////////////////////////////////////////////////////
    /*! Build a set of splines
     * \param nspl       the number of splines
     * \param npts       the number of points of each splines
     * \param headers    the names of the splines
     * \param stype      the type of each spline
     * \param X          pointer to X independent values
     * \param Y          vector of `nspl` pointers to Y depentendent values.
     */

    void
    build( sizeType   const nspl,
           sizeType   const npts,
           char       const *headers[],
           SplineType const stype[],
           valueType  const X[],
           valueType  const *Y[],
           valueType  const *Yp[] = nullptr ) ;

    #ifndef SPLINES_DO_NOT_USE_GENERIC_CONTAINER
    virtual void setup ( GenericContainer const & gc ) ;
    void build ( GenericContainer const & gc ) { setup(gc) ; }
    #endif

    //! Return spline type (as number)
    virtual unsigned type() const { return SPLINE_SET_TYPE ; }

    //! Get info of splines collected
    void info( std::basic_ostream<char> & s ) const ;
    
    void dump_table( std::basic_ostream<char> & s, sizeType num_points ) const ;

  } ;

  /*
  //   ____        _ _            ____              __
  //  / ___| _ __ | (_)_ __   ___/ ___| _   _ _ __ / _|
  //  \___ \| '_ \| | | '_ \ / _ \___ \| | | | '__| |_
  //   ___) | |_) | | | | | |  __/___) | |_| | |  |  _|
  //  |____/| .__/|_|_|_| |_|\___|____/ \__,_|_|  |_|
  //        |_|
  */
  //! Spline Management Class
  class SplineSurf {

    SplineSurf(SplineSurf const &) ; // block copy constructor
    SplineSurf const & operator = (SplineSurf const &) ; // block copy method

  protected:
  
    string const _name ;
    bool         _check_range ;

    vector<valueType> X, Y, Z ;
    
    valueType Z_min, Z_max ;

    mutable sizeType lastInterval_x ;

    sizeType
    search_x( valueType x ) const {
      sizeType npts_x = sizeType(X.size()) ;
      SPLINE_ASSERT( npts_x > 1, "\nsearch_x(" << x << ") empty spline");
      if ( _check_range ) {
        valueType xl = X.front() ;
        valueType xr = X.back() ;
        SPLINE_ASSERT( x >= xl && x <= xr,
                       "method search_x( " << x << " ) out of range: [" <<
                       xl << ", " << xr << "]" ) ;
      }
      Splines::updateInterval( lastInterval_x, x, &X.front(), npts_x ) ;
      return lastInterval_x;
    }

    mutable sizeType lastInterval_y ;

    sizeType
    search_y( valueType y ) const {
      sizeType npts_y = sizeType(Y.size()) ;
      SPLINE_ASSERT( npts_y > 1, "\nsearch_y(" << y << ") empty spline");
      if ( _check_range ) {
        valueType yl = Y.front() ;
        valueType yr = Y.back() ;
        SPLINE_ASSERT( y >= yl && y <= yr,
                      "method search_y( " << y << " ) out of range: [" <<
                       yl << ", " << yr << "]" ) ;
      }
      Splines::updateInterval( lastInterval_y, y, &Y.front(), npts_y ) ;
      return lastInterval_y;
    }

    sizeType ipos_C( sizeType i, sizeType j, sizeType ldZ ) const { return i*ldZ + j ; }
    sizeType ipos_F( sizeType i, sizeType j, sizeType ldZ ) const { return i + ldZ*j ; }

    sizeType ipos_C( sizeType i, sizeType j ) const { return ipos_C(i,j,sizeType(Y.size())) ; }
    sizeType ipos_F( sizeType i, sizeType j ) const { return ipos_F(i,j,sizeType(X.size())) ; }

    virtual void makeSpline() = 0 ;

  public:

    //! spline constructor
    SplineSurf( string const & name = "Spline", bool ck = false )
    : _name(name)
    , _check_range(ck)
    , X()
    , Y()
    , Z()
    , Z_min(0)
    , Z_max(0)
    , lastInterval_x(0)
    , lastInterval_y(0)
    {}

    //! spline destructor
    virtual ~SplineSurf() {}

    string const & name() const { return _name ; }

    void setCheckRange( bool ck ) { _check_range = ck ; }
    bool getCheckRange() const { return _check_range ; }

    //! Cancel the support points, empty the spline.
    virtual
    void
    clear (void) {
      X.clear() ;
      Y.clear() ;
      Z.clear() ;
      Z_min = Z_max = 0 ;
      lastInterval_x = 0 ;
      lastInterval_y = 0 ;
    }

    //! return the number of support points of the spline along x direction
    sizeType numPointX(void) const { return sizeType(X.size()) ; }

    //! return the number of support points of the spline along y direction
    sizeType numPointY(void) const { return sizeType(Y.size()) ; }

    //! return the i-th node of the spline (x component).
    valueType xNode( sizeType i ) const { return X[i] ; }

    //! return the i-th node of the spline (y component).
    valueType yNode( sizeType i ) const { return Y[i] ; }

    //! return the i-th node of the spline (y component).
    valueType zNode( sizeType i, sizeType j ) const { return Z[ipos_C(i,j)] ; }

    //! return x-minumum spline value
    valueType xMin() const { return X.front() ; }

    //! return x-maximum spline value
    valueType xMax() const { return X.back()  ; }

    //! return y-minumum spline value
    valueType yMin() const { return Y.front() ; }

    //! return y-maximum spline value
    valueType yMax() const { return Y.back()  ; }

    //! return z-minumum spline value
    valueType zMin() const { return Z_min ; }

    //! return z-maximum spline value
    valueType zMax() const { return Z_max ; }

    ///////////////////////////////////////////////////////////////////////////
    /*! Build surface spline
     * \param x       vector of x-coordinates
     * \param incx    access elements as x[0], x[incx], x[2*incx],...
     * \param y       vector of y-coordinates
     * \param incy    access elements as y[0], y[incy], x[2*incy],...
     * \param z       matrix of z-values
     * \param ldZ     leading dimension of the matrix. Elements are stored
     *                by row Z(i,j) = z[i*ldZ+j] as C-matrix
     * \param nx      total number of points in direction x
     * \param ny      total number of points in direction y
     * \param fortran_storage if true elements are stored by column
     *                        i.e. Z(i,j) = z[i+j*ldZ] as Fortran-matrix
     * \param transposed      if true matrix Z is stored transposed
     */
    void
    build ( valueType const x[], sizeType incx,
            valueType const y[], sizeType incy,
            valueType const z[], sizeType ldZ,
            sizeType nx, sizeType ny,
            bool fortran_storage = false,
            bool transposed      = false ) ;

    /*! Build surface spline
     * \param x       vector of x-coordinates, nx = x.size()
     * \param y       vector of y-coordinates, ny = y.size()
     * \param z       matrix of z-values. Elements are stored
     *                by row Z(i,j) = z[i*ny+j] as C-matrix
     * \param fortran_storage if true elements are stored by column
     *                        i.e. Z(i,j) = z[i+j*nx] as Fortran-matrix
     * \param transposed      if true matrix Z is stored transposed
     */
    void
    build ( vector<valueType> const & x,
            vector<valueType> const & y,
            vector<valueType> const & z,
            bool fortran_storage = false,
            bool transposed      = false ) {
      bool xyswp = (fortran_storage && transposed) ||
                   (!fortran_storage && !transposed) ;
      build ( &x.front(), 1, &y.front(), 1, &z.front(),
              sizeType(xyswp ? y.size() : x.size()),
              sizeType(x.size()), sizeType(y.size()),
              fortran_storage, transposed ) ;
    }

    /*! Build surface spline
     * \param z               matrix of z-values. Elements are stored
     *                        by row Z(i,j) = z[i*ny+j] as C-matrix
     * \param ldZ             leading dimension of the matrix. Elements are stored
     *                        by row Z(i,j) = z[i*ldZ+j] as C-matrix
     * \param fortran_storage if true elements are stored by column
     *                        i.e. Z(i,j) = z[i+j*nx] as Fortran-matrix
     * \param transposed      if true matrix Z is stored transposed
     */
    void
    build ( valueType const z[], sizeType ldZ, sizeType nx, sizeType ny,
            bool fortran_storage = false,
            bool transposed      = false ) ;

    /*! Build surface spline
     * \param z               matrix of z-values. Elements are stored
     *                        by row Z(i,j) = z[i*ny+j] as C-matrix.
     *                        ldZ leading dimension of the matrix is ny for C-storage
     *                        and nx for Fortran storage.
     * \param fortran_storage if true elements are stored by column
     *                        i.e. Z(i,j) = z[i+j*nx] as Fortran-matrix
     * \param transposed      if true matrix Z is stored transposed
     */
    void
    build ( vector<valueType> const & z, sizeType nx, sizeType ny,
            bool fortran_storage = false,
            bool transposed      = false ) {
      if ( fortran_storage ) build ( &z.front(), nx, nx, ny, fortran_storage, transposed ) ;
      else                   build ( &z.front(), ny, nx, ny, fortran_storage, transposed ) ;
    }

    #ifndef SPLINES_DO_NOT_USE_GENERIC_CONTAINER
    virtual void setup ( GenericContainer const & gc ) ;
    void build ( GenericContainer const & gc ) { setup(gc) ; }
    #endif

    //! Evaluate spline value
    virtual valueType operator () ( valueType x, valueType y ) const = 0 ;

    //! First derivative
    virtual void D( valueType x, valueType y, valueType d[3] ) const = 0 ;
    virtual valueType Dx( valueType x, valueType y ) const = 0 ;
    virtual valueType Dy( valueType x, valueType y ) const = 0 ;

    //! Second derivative
    virtual void DD( valueType x, valueType y, valueType dd[6] ) const = 0 ;
    virtual valueType Dxx( valueType x, valueType y ) const = 0 ;
    virtual valueType Dxy( valueType x, valueType y ) const = 0 ;
    virtual valueType Dyy( valueType x, valueType y ) const = 0 ;

    //! Evaluate spline value
    valueType eval( valueType x, valueType y ) const { return (*this)(x,y) ; }

    //! First derivative
    virtual valueType eval_D_1( valueType x, valueType y ) const { return Dx(x,y) ; }
    virtual valueType eval_D_2( valueType x, valueType y ) const { return Dy(x,y) ; }

    //! Second derivative
    virtual valueType eval_D_1_1( valueType x, valueType y ) const { return Dxx(x,y) ; }
    virtual valueType eval_D_1_2( valueType x, valueType y ) const { return Dxy(x,y) ; }
    virtual valueType eval_D_2_2( valueType x, valueType y ) const { return Dyy(x,y) ; }

    //! Print spline information
    virtual void info( ostream & s ) const ;

    //! Print spline coefficients
    virtual void writeToStream( ostream & s ) const = 0 ;

    //! Return spline typename
    virtual char const * type_name() const = 0 ;

  } ;

  /*
  //   ____  _ _ _                       ____        _ _
  //  | __ )(_) (_)_ __   ___  __ _ _ __/ ___| _ __ | (_)_ __   ___
  //  |  _ \| | | | '_ \ / _ \/ _` | '__\___ \| '_ \| | | '_ \ / _ \
  //  | |_) | | | | | | |  __/ (_| | |   ___) | |_) | | | | | |  __/
  //  |____/|_|_|_|_| |_|\___|\__,_|_|  |____/| .__/|_|_|_| |_|\___|
  //                                          |_|
  */
  //! bilinear spline base class
  class BilinearSpline : public SplineSurf {
    virtual void makeSpline() {}
  public:
  
    //! spline constructor
    BilinearSpline( string const & name = "Spline", bool ck = false )
    : SplineSurf(name,ck)
    {}
    
    virtual
    ~BilinearSpline()
    {}

    //! Evaluate spline value
    virtual valueType operator () ( valueType x, valueType y ) const ;

    //! First derivative
    virtual void D( valueType x, valueType y, valueType d[3] ) const ;
    virtual valueType Dx( valueType x, valueType y ) const ;
    virtual valueType Dy( valueType x, valueType y ) const ;

    //! Second derivative
    virtual void DD( valueType x, valueType y, valueType dd[6] ) const { D(x,y,dd) ; dd[3] = dd[4] = dd[5] = 0 ; }
    virtual valueType Dxx( valueType , valueType ) const { return 0 ; }
    virtual valueType Dxy( valueType , valueType ) const { return 0 ; }
    virtual valueType Dyy( valueType , valueType ) const { return 0 ; }

    //! Print spline coefficients
    virtual void writeToStream( ostream & s ) const ;

    //! Return spline typename
    virtual char const * type_name() const ;

  } ;

  /*
  //   ____  _  ____      _     _      ____        _ _            ____
  //  | __ )(_)/ ___|   _| |__ (_) ___/ ___| _ __ | (_)_ __   ___| __ )  __ _ ___  ___
  //  |  _ \| | |  | | | | '_ \| |/ __\___ \| '_ \| | | '_ \ / _ \  _ \ / _` / __|/ _ \
  //  | |_) | | |__| |_| | |_) | | (__ ___) | |_) | | | | | |  __/ |_) | (_| \__ \  __/
  //  |____/|_|\____\__,_|_.__/|_|\___|____/| .__/|_|_|_| |_|\___|____/ \__,_|___/\___|
  //                                        |_|
  */
  //! Bi-cubic spline base class
  class BiCubicSplineBase : public SplineSurf {
  protected:
  
    vector<valueType> DX, DY, DXY ;
    mutable valueType u[4] ;
    mutable valueType u_D[4] ;
    mutable valueType u_DD[4] ;
    mutable valueType v[4] ;
    mutable valueType v_D[4] ;
    mutable valueType v_DD[4] ;
    mutable valueType bili3[4][4] ;

    void load( sizeType i, sizeType j ) const ;

  public:
  
    //! spline constructor
    BiCubicSplineBase( string const & name = "Spline", bool ck = false )
    : SplineSurf( name, ck )
    , DX()
    , DY()
    {}
    
    virtual
    ~BiCubicSplineBase()
    {}

    valueType DxNode ( sizeType i, sizeType j ) const { return DX[ipos_C(i,j)] ; }
    valueType DyNode ( sizeType i, sizeType j ) const { return DY[ipos_C(i,j)] ; }
    valueType DxyNode( sizeType i, sizeType j ) const { return DXY[ipos_C(i,j)] ; }

    //! Evaluate spline value
    virtual valueType operator () ( valueType x, valueType y ) const ;

    //! First derivative
    virtual void D( valueType x, valueType y, valueType d[3] ) const ;
    virtual valueType Dx( valueType x, valueType y ) const ;
    virtual valueType Dy( valueType x, valueType y ) const ;

    //! Second derivative
    virtual void DD( valueType x, valueType y, valueType dd[6] ) const ;
    virtual valueType Dxx( valueType x, valueType y ) const ;
    virtual valueType Dxy( valueType x, valueType y ) const ;
    virtual valueType Dyy( valueType x, valueType y ) const ;
  } ;

  /*
  //   ____  _  ____      _     _      ____        _ _            
  //  | __ )(_)/ ___|   _| |__ (_) ___/ ___| _ __ | (_)_ __   ___
  //  |  _ \| | |  | | | | '_ \| |/ __\___ \| '_ \| | | '_ \ / _ \
  //  | |_) | | |__| |_| | |_) | | (__ ___) | |_) | | | | | |  __/
  //  |____/|_|\____\__,_|_.__/|_|\___|____/| .__/|_|_|_| |_|\___|
  //                                        |_|
  */
  //! cubic spline base class
  class BiCubicSpline : public BiCubicSplineBase {
    virtual void makeSpline() ;

  public:
  
    //! spline constructor
    BiCubicSpline( string const & name = "Spline", bool ck = false )
    : BiCubicSplineBase( name, ck )
    {}
    
    virtual
    ~BiCubicSpline()
    {}

    //! Print spline coefficients
    virtual void writeToStream( ostream & s ) const ;

    //! Return spline typename
    virtual char const * type_name() const ;

  } ;

  /*
  //      _    _    _                 ____  ____            _ _
  //     / \  | | _(_)_ __ ___   __ _|___ \|  _ \ ___ _ __ | (_)_ __   ___
  //    / _ \ | |/ / | '_ ` _ \ / _` | __) | | | / __| '_ \| | | '_ \ / _ \
  //   / ___ \|   <| | | | | | | (_| |/ __/| |_| \__ \ |_) | | | | | |  __/
  //  /_/   \_\_|\_\_|_| |_| |_|\__,_|_____|____/|___/ .__/|_|_|_| |_|\___|
  //                                                 |_|
  */
  //! cubic spline base class
  class Akima2Dspline : public BiCubicSplineBase {
    virtual void makeSpline() ;

  public:
  
    //! spline constructor
    Akima2Dspline( string const & name = "Spline", bool ck = false )
    : BiCubicSplineBase( name, ck )
    {}
    
    virtual
    ~Akima2Dspline()
    {}

    //! Print spline coefficients
    virtual void writeToStream( ostream & s ) const ;

    //! Return spline typename
    virtual char const * type_name() const ;

  } ;
  
  /*
  //   ____  _  ___        _       _   _      ____        _ _            ____
  //  | __ )(_)/ _ \ _   _(_)_ __ | |_(_) ___/ ___| _ __ | (_)_ __   ___| __ )  __ _ ___  ___
  //  |  _ \| | | | | | | | | '_ \| __| |/ __\___ \| '_ \| | | '_ \ / _ \  _ \ / _` / __|/ _ \
  //  | |_) | | |_| | |_| | | | | | |_| | (__ ___) | |_) | | | | | |  __/ |_) | (_| \__ \  __/
  //  |____/|_|\__\_\\__,_|_|_| |_|\__|_|\___|____/| .__/|_|_|_| |_|\___|____/ \__,_|___/\___|
  //                                               |_|
  */
  //! Bi-quintic spline base class
  class BiQuinticSplineBase : public SplineSurf {
  protected:

    vector<valueType> DX, DXX, DY, DYY, DXY, DXYY, DXXY, DXXYY ;
    mutable valueType u[6] ;
    mutable valueType u_D[6] ;
    mutable valueType u_DD[6] ;
    mutable valueType v[6] ;
    mutable valueType v_D[6] ;
    mutable valueType v_DD[6] ;
    mutable valueType bili5[6][6] ;

    void load( sizeType i, sizeType j ) const ;

  public:
  
    //! spline constructor
    BiQuinticSplineBase( string const & name = "Spline", bool ck = false )
    : SplineSurf( name, ck )
    , DX()
    , DXX()
    , DY()
    , DYY()
    , DXY()
    {}
    
    virtual
    ~BiQuinticSplineBase()
    {}

    valueType DxNode ( sizeType i, sizeType j ) const { return DX[ipos_C(i,j)] ; }
    valueType DyNode ( sizeType i, sizeType j ) const { return DY[ipos_C(i,j)] ; }
    valueType DxxNode( sizeType i, sizeType j ) const { return DXX[ipos_C(i,j)] ; }
    valueType DyyNode( sizeType i, sizeType j ) const { return DYY[ipos_C(i,j)] ; }
    valueType DxyNode( sizeType i, sizeType j ) const { return DXY[ipos_C(i,j)] ; }

    //! Evaluate spline value
    virtual valueType operator () ( valueType x, valueType y ) const ;

    //! First derivative
    virtual void D( valueType x, valueType y, valueType d[3] ) const ;
    virtual valueType Dx( valueType x, valueType y ) const ;
    virtual valueType Dy( valueType x, valueType y ) const ;

    //! Second derivative
    virtual void DD( valueType x, valueType y, valueType dd[6] ) const ;
    virtual valueType Dxx( valueType x, valueType y ) const ;
    virtual valueType Dxy( valueType x, valueType y ) const ;
    virtual valueType Dyy( valueType x, valueType y ) const ;
  } ;

  /*
  //   ____  _  ___        _       _   _      ____        _ _
  //  | __ )(_)/ _ \ _   _(_)_ __ | |_(_) ___/ ___| _ __ | (_)_ __   ___
  //  |  _ \| | | | | | | | | '_ \| __| |/ __\___ \| '_ \| | | '_ \ / _ \ 
  //  | |_) | | |_| | |_| | | | | | |_| | (__ ___) | |_) | | | | | |  __/ 
  //  |____/|_|\__\_\\__,_|_|_| |_|\__|_|\___|____/| .__/|_|_|_| |_|\___|
  //                                               |_|
  */
  //! cubic spline base class
  class BiQuinticSpline : public BiQuinticSplineBase {
    virtual void makeSpline() ;
  public:
  
    //! spline constructor
    BiQuinticSpline( string const & name = "Spline", bool ck = false )
    : BiQuinticSplineBase( name, ck )
    {}
    
    virtual
    ~BiQuinticSpline()
    {}

    //! Print spline coefficients
    virtual void writeToStream( ostream & s ) const ;

    //! Return spline typename
    virtual char const * type_name() const ;

  } ;

}

namespace SplinesLoad {

  using Splines::Spline ;
  using Splines::BSpline ;
  using Splines::CubicSplineBase ;
  using Splines::CubicSpline ;
  using Splines::AkimaSpline ;
  using Splines::BesselSpline ;
  using Splines::PchipSpline ;
  using Splines::LinearSpline ;
  using Splines::ConstantSpline ;
  using Splines::QuinticSpline ;

  using Splines::BilinearSpline ;
  using Splines::BiCubicSpline ;
  using Splines::BiQuinticSpline ;
  using Splines::Akima2Dspline ;

  using Splines::SplineSet ;
  using Splines::SplineType ;

  using Splines::quadraticRoots ;
  using Splines::cubicRoots ;

}

#ifdef __GCC__
#pragma GCC diagnostic pop
#endif
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif
