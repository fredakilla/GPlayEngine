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
#endif
#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++98-compat"
#endif

/**
 * 
 */

// interface with GenericContainer
#ifndef SPLINES_DO_NOT_USE_GENERIC_CONTAINER

namespace Splines {

  using GenericContainerNamespace::GC_INTEGER ;
  using GenericContainerNamespace::GC_VEC_BOOL ;
  using GenericContainerNamespace::GC_VEC_INTEGER ;
  using GenericContainerNamespace::GC_VEC_REAL ;
  using GenericContainerNamespace::GC_VEC_STRING ;
  using GenericContainerNamespace::GC_VECTOR ;
  using GenericContainerNamespace::GC_MAP ;
  using GenericContainerNamespace::GC_MAT_REAL ;
  using GenericContainerNamespace::mat_real_type ;
  using GenericContainerNamespace::vec_int_type ;
  using GenericContainerNamespace::vec_real_type ;
  using GenericContainerNamespace::vec_string_type ;
  using GenericContainerNamespace::vector_type ;
  using GenericContainerNamespace::map_type ;

  void
  SplineSet::setup( GenericContainer const & gc ) {
    /*
    // gc["headers"]
    // gc["spline_type"]
    // gc["data"]
    // gc["independent"]
    //
    */
    vec_string_type       spline_type_vec ;
    vec_real_type         X ;
    vector<SplineType>    stype ;
    vec_string_type       headers ;
    vector<vec_real_type> Y ;
    vector<vec_real_type> Yp ;
    
    SPLINE_ASSERT( gc.exists("spline_type"), "[SplineSet[" << _name << "]::setup] missing `spline_type` field!") ;
    gc("spline_type").copyto_vec_string( spline_type_vec, "SplineSet::setup -- in reading `spline_type'\n" ) ;
    _nspl = sizeType(spline_type_vec.size()) ;
    stype.resize(_nspl) ;
    headers.resize(_nspl) ;
    for ( sizeType spl = 0 ; spl < _nspl ; ++spl )
      stype[spl] = string_to_splineType( spline_type_vec[spl] ) ;
    
    SPLINE_ASSERT( gc.exists("xdata"), "[SplineSet[" << _name << "]::setup] missing `xdata` field!") ;
    gc("xdata").copyto_vec_real( X, "SplineSet::setup reading `xdata'" ) ;
    _npts = sizeType( X.size() ) ;
    
    SPLINE_ASSERT( gc.exists("ydata") , "[SplineSet[" << _name << "]::setup] missing `ydata` field!") ;
    GenericContainer const & gc_ydata = gc("ydata") ;

    // allocate for _nspl splines
    Y.resize(_nspl) ;
    Yp.resize(_nspl) ;
    
    // se tipo vettore o matrice deve esserci headers
    if ( GC_MAT_REAL == gc_ydata.get_type() || GC_VECTOR == gc_ydata.get_type() ) {
      SPLINE_ASSERT( gc.exists("headers"), "[SplineSet[" << _name << "]::setup] missing `headers` field!") ;
      GenericContainer const & gc_headers = gc("headers") ;
      gc_headers.copyto_vec_string( headers, "SplineSet::setup reading `headers'\n" ) ;
      SPLINE_ASSERT( headers.size() == _nspl,
                     "[SplineSet[" << _name << "]::setup] field `headers` expected to be of size " << _nspl <<
                    " found of size " << headers.size() ) ;
    }
    
    if ( GC_MAT_REAL == gc_ydata.get_type() ) {
      // leggo matrice
      mat_real_type const & data = gc_ydata.get_mat_real() ;
      SPLINE_ASSERT( _nspl == data.numCols(),
                     "[SplineSet[" << _name << "]::setup] number of headers [" << _nspl <<
                     "] differs from the number of columns [" << data.numCols() << "] in data" ) ;
      SPLINE_ASSERT( _npts == data.numRows(),
                     "[SplineSet[" << _name << "]::setup] number of points [" << _npts <<
                     "] differs from the numeber of rows [" << data.numRows() << "] in data" ) ;
      for ( sizeType i = 0 ; i < _nspl ; ++i ) data.getColumn(i,Y[i]) ;
    } else if ( GC_VECTOR == gc_ydata.get_type() ) {
      vector_type const & data = gc_ydata.get_vector() ;
      SPLINE_ASSERT( _nspl == data.size(),
                     "[SplineSet[" << _name << "]::setup] field `data` expected of size " << _nspl <<
                     " found of size " << data.size()  ) ;
      for ( sizeType i = 0 ; i < _nspl ; ++i ) {
        GenericContainer const & datai = data[i] ;
        sizeType nrow = _npts ;
        if ( stype[i] == CONSTANT_TYPE ) --nrow ; // constant spline uses n-1 points
        datai.copyto_vec_real( Y[i], "SplineSet::setup reading `ydata'" ) ;
      }
    } else if ( GC_MAP == gc_ydata.get_type() ) {
      map_type const & data = gc_ydata.get_map() ;
      SPLINE_ASSERT( data.size() == _nspl,
                    "[SplineSet[" << _name << "]::setup] field `ydata` expected of size " << _nspl <<
                    " found of size " << data.size() ) ;
      map_type::const_iterator im = data.begin() ;
      for ( sizeType spl = 0 ; im != data.end() ; ++im, ++spl ) {
        headers[spl] = im->first ;
        GenericContainer const & datai = im->second ;
        sizeType nrow = _npts ;
        if ( stype[spl] == CONSTANT_TYPE ) --nrow ; // constant spline uses n-1 points
        datai.copyto_vec_real( Y[spl], "SplineSet::setup reading `ydata'" ) ;
      }
    } else {
      SPLINE_ASSERT( false,
                     "[SplineSet[" << _name << "]::setup] field `data` expected to be of type `mat_real_type`, `vector_type` or `map_type' found: ` " <<
                      gc_ydata.get_type_name() << "`" ) ;
    }
    
    if ( gc.exists("ypdata") ) { // yp puo essere solo tipo map
      GenericContainer const & gc_ypdata = gc("ypdata") ;
      SPLINE_ASSERT( GC_MAP == gc_ypdata.get_type(),
                     "[SplineSet[" << _name << "]::setup] field `ypdata` expected to be of type `map_type` found: ` " <<
                     gc_ypdata.get_type_name() << "`" ) ;
      for ( sizeType spl = 0 ; spl < _nspl ; ++spl ) header_to_position[headers[spl]] = spl ;
      map_type const & data = gc_ypdata.get_map() ;
      map_type::const_iterator im = data.begin() ;
      for ( ; im != data.end() ; ++im ) {
        sizeType spl = getPosition(im->first.c_str()) ;
        GenericContainer const & datai = im->second ;
        sizeType nrow = _npts ;
        if ( stype[spl] == CONSTANT_TYPE ) --nrow ; // constant spline uses n-1 points
        datai.copyto_vec_real( Yp[spl], "SplineSet::setup reading `ypdata'" ) ;
      }
    }
  
    vector<char const*>      __headers(_nspl) ;
    vector<valueType const*> __Y(_nspl) ;
    vector<valueType const*> __Yp(_nspl) ;
      
    for ( sizeType spl = 0 ; spl < _nspl ; ++spl ) {
      __headers[spl] = headers[spl].c_str() ;
      __Y[spl]       = &Y[spl].front() ;
      __Yp[spl]      = Yp[spl].size() > 0 ? &Yp[spl].front() : nullptr ;
    }
    
    SplineSet::build( _nspl, _npts,
                      &__headers.front(),
                      &stype.front(),
                      &X.front(),
                      &__Y.front(),
                      &__Yp.front() ) ;
  }

  //! Evaluate all the splines at `x` and fill a map of values in a GenericContainer
  void
  SplineSet::eval( valueType x, GenericContainer & gc ) const {
    map_type & vals = gc.set_map() ;
    for ( map<string,sizeType>::const_iterator s_to_pos = header_to_position.begin() ;
          s_to_pos != header_to_position.end() ; ++s_to_pos ) {
      vals[s_to_pos->first] = splines[s_to_pos->second]->eval(x) ;
    }
  }

  //! Evaluate all the splines at `x` values contained in vec and fill a map of vector in a GenericContainer
  void
  SplineSet::eval( vec_real_type const & vec, GenericContainer & gc ) const {
    sizeType npts = sizeType(vec.size()) ;
    map_type & vals = gc.set_map() ;
    for ( map<string,sizeType>::const_iterator s_to_pos = header_to_position.begin() ;
          s_to_pos != header_to_position.end() ; ++s_to_pos ) {
      vec_real_type & v = vals[s_to_pos->first].set_vec_real(npts) ;
      Spline const * p_spl = splines[s_to_pos->second] ;
      for ( sizeType i = 0 ; i < npts ; ++i ) v[i] = p_spl->eval(vec[i]) ;
    }
  }

  //! Evaluate all the splines at `x` and fill a map of values in a GenericContainer with keys in `columns`
  void
  SplineSet::eval( valueType x, vec_string_type const & columns, GenericContainer & gc ) const {
    map_type & vals = gc.set_map() ;
    for ( vec_string_type::const_iterator is = columns.begin() ;
          is != columns.end() ; ++is ) {
      Spline const * p_spl = getSpline( is->c_str() ) ;
      vals[*is] = p_spl->eval(x) ;
    }
  }

  //! Evaluate all the splines at `x` values contained in vec and fill a map of vector in a GenericContainer with keys in `columns`
  void
  SplineSet::eval( vec_real_type const & vec, vec_string_type const & columns, GenericContainer & gc ) const {
    sizeType npts = sizeType(vec.size()) ;
    map_type & vals = gc.set_map() ;
    for ( vec_string_type::const_iterator is = columns.begin() ;
          is != columns.end() ; ++is ) {
      Spline const * p_spl = getSpline( is->c_str() ) ;
      vec_real_type & v = vals[*is].set_vec_real(npts) ;
      for ( sizeType i = 0 ; i < npts ; ++i ) v[i] = p_spl->eval(vec[i]) ;
    }
  }

  //! Evaluate all the splines at `zeta` and fill a map of values in a GenericContainer and `indep` as independent spline
  void
  SplineSet::eval2( valueType zeta, sizeType indep, GenericContainer & gc ) const {
    map_type & vals = gc.set_map() ;
    valueType x ;
    intersect( indep, zeta, x ) ;
    for ( map<string,sizeType>::const_iterator s_to_pos = header_to_position.begin() ;
          s_to_pos != header_to_position.end() ; ++s_to_pos ) {
      vals[s_to_pos->first] = splines[s_to_pos->second]->eval(x) ;
    }
  }

  //! Evaluate all the splines at `zeta` values contained in vec and fill a map of vector in a GenericContainer and `indep` as independent spline
  void
  SplineSet::eval2( vec_real_type const & zetas, sizeType indep, GenericContainer & gc ) const {
    sizeType npts = sizeType(zetas.size()) ;
    map_type & vals = gc.set_map() ;

    // preallocation
    for ( map<string,sizeType>::const_iterator s_to_pos = header_to_position.begin() ;
          s_to_pos != header_to_position.end() ; ++s_to_pos )
      vals[s_to_pos->first].set_vec_real(npts) ;

    for ( sizeType i = 0 ; i < npts ; ++i ) {
      valueType x ;
      intersect( indep, zetas[i], x ) ;
      for ( map<string,sizeType>::const_iterator s_to_pos = header_to_position.begin() ;
            s_to_pos != header_to_position.end() ; ++s_to_pos ) {
        vec_real_type & v = vals[s_to_pos->first].get_vec_real() ;
        v[i] = splines[s_to_pos->second]->eval(x) ;
      }
    }
  }

  //! Evaluate all the splines at `zeta` and fill a map of values in a GenericContainer with keys in `columns` and `indep` as independent spline
  void
  SplineSet::eval2( valueType zeta, sizeType indep, vec_string_type const & columns, GenericContainer & gc ) const {
    map_type & vals = gc.set_map() ;
    valueType x ;
    intersect( indep, zeta, x ) ;
    for ( vec_string_type::const_iterator is = columns.begin() ;
          is != columns.end() ; ++is ) {
      Spline const * p_spl = getSpline( is->c_str() ) ;
      vals[*is] = p_spl->eval(x) ;
    }
  }

  //! Evaluate all the splines at `zeta` values contained in vec and fill a map of vector in a GenericContainer with keys in `columns` and `indep` as independent spline
  void
  SplineSet::eval2( vec_real_type const & zetas, sizeType indep, vec_string_type const & columns, GenericContainer & gc ) const {
    sizeType npts = sizeType(zetas.size()) ;
    map_type & vals = gc.set_map() ;

    // preallocation
    for ( vec_string_type::const_iterator is = columns.begin() ;
          is != columns.end() ; ++is )
      vals[*is].set_vec_real(npts) ;

    for ( sizeType i = 0 ; i < npts ; ++i ) {
      valueType x ;
      intersect( indep, zetas[i], x ) ;
      for ( vec_string_type::const_iterator is = columns.begin() ;
            is != columns.end() ; ++is ) {
        vec_real_type & v = vals[*is].get_vec_real() ;
        Spline const * p_spl = getSpline( is->c_str() ) ;
        v[i] = p_spl->eval(x) ;
      }
    }
  }

  /*
  //    __ _          _         _           _            _   _
  //   / _(_)_ __ ___| |_    __| | ___ _ __(_)_   ____ _| |_(_)_   _____
  //  | |_| | '__/ __| __|  / _` |/ _ \ '__| \ \ / / _` | __| \ \ / / _ \
  //  |  _| | |  \__ \ |_  | (_| |  __/ |  | |\ V / (_| | |_| |\ V /  __/
  //  |_| |_|_|  |___/\__|  \__,_|\___|_|  |_| \_/ \__,_|\__|_| \_/ \___|
  */
  void
  SplineSet::eval_D( valueType x, GenericContainer & gc ) const {
    map_type & vals = gc.set_map() ;
    for ( map<string,sizeType>::const_iterator s_to_pos = header_to_position.begin() ;
          s_to_pos != header_to_position.end() ; ++s_to_pos ) {
      vals[s_to_pos->first] = splines[s_to_pos->second]->eval_D(x) ;
    }
  }

  //! Evaluate all the splines at `x` values contained in vec and fill a map of vector in a GenericContainer
  void
  SplineSet::eval_D( vec_real_type const & vec, GenericContainer & gc ) const {
    sizeType npts = sizeType(vec.size()) ;
    map_type & vals = gc.set_map() ;
    for ( map<string,sizeType>::const_iterator s_to_pos = header_to_position.begin() ;
          s_to_pos != header_to_position.end() ; ++s_to_pos ) {
      vec_real_type & v = vals[s_to_pos->first].set_vec_real(npts) ;
      Spline const * p_spl = splines[s_to_pos->second] ;
      for ( sizeType i = 0 ; i < npts ; ++i ) v[i] = p_spl->eval_D(vec[i]) ;
    }
  }

  //! Evaluate all the splines at `x` and fill a map of values in a GenericContainer with keys in `columns`
  void
  SplineSet::eval_D( valueType x, vec_string_type const & columns, GenericContainer & gc ) const {
    map_type & vals = gc.set_map() ;
    for ( vec_string_type::const_iterator is = columns.begin() ;
          is != columns.end() ; ++is ) {
      Spline const * p_spl = getSpline( is->c_str() ) ;
      vals[*is] = p_spl->eval_D(x) ;
    }
  }

  //! Evaluate all the splines at `x` values contained in vec and fill a map of vector in a GenericContainer with keys in `columns`
  void
  SplineSet::eval_D( vec_real_type const & vec, vec_string_type const & columns, GenericContainer & gc ) const {
    sizeType npts = sizeType(vec.size()) ;
    map_type & vals = gc.set_map() ;
    for ( vec_string_type::const_iterator is = columns.begin() ;
          is != columns.end() ; ++is ) {
      Spline const * p_spl = getSpline( is->c_str() ) ;
      vec_real_type & v = vals[*is].set_vec_real(npts) ;
      for ( sizeType i = 0 ; i < npts ; ++i ) v[i] = p_spl->eval_D(vec[i]) ;
    }
  }

  //! Evaluate all the splines at `zeta` and fill a map of values in a GenericContainer and `indep` as independent spline
  void
  SplineSet::eval2_D( valueType zeta, sizeType indep, GenericContainer & gc ) const {
    map_type & vals = gc.set_map() ;
    valueType x ;
    intersect( indep, zeta, x ) ;
    for ( map<string,sizeType>::const_iterator s_to_pos = header_to_position.begin() ;
          s_to_pos != header_to_position.end() ; ++s_to_pos ) {
      vals[s_to_pos->first] = splines[s_to_pos->second]->eval_D(x) ;
    }
  }

  //! Evaluate all the splines at `zeta` values contained in vec and fill a map of vector in a GenericContainer and `indep` as independent spline
  void
  SplineSet::eval2_D( vec_real_type const & zetas, sizeType indep, GenericContainer & gc ) const {
    sizeType npts = sizeType(zetas.size()) ;
    map_type & vals = gc.set_map() ;

    // preallocation
    for ( map<string,sizeType>::const_iterator s_to_pos = header_to_position.begin() ;
          s_to_pos != header_to_position.end() ; ++s_to_pos )
      vals[s_to_pos->first].set_vec_real(npts) ;

    for ( sizeType i = 0 ; i < npts ; ++i ) {
      valueType x ;
      intersect( indep, zetas[i], x ) ;
      for ( map<string,sizeType>::const_iterator s_to_pos = header_to_position.begin() ;
            s_to_pos != header_to_position.end() ; ++s_to_pos ) {
        vec_real_type & v = vals[s_to_pos->first].get_vec_real() ;
        v[i] = splines[s_to_pos->second]->eval_D(x) ;
      }
    }
  }

  //! Evaluate all the splines at `zeta` and fill a map of values in a GenericContainer with keys in `columns` and `indep` as independent spline
  void
  SplineSet::eval2_D( valueType zeta, sizeType indep, vec_string_type const & columns, GenericContainer & gc ) const {
    map_type & vals = gc.set_map() ;
    valueType x ;
    intersect( indep, zeta, x ) ;
    for ( vec_string_type::const_iterator is = columns.begin() ;
          is != columns.end() ; ++is ) {
      Spline const * p_spl = getSpline( is->c_str() ) ;
      vals[*is] = p_spl->eval_D(x) ;
    }
  }

  //! Evaluate all the splines at `zeta` values contained in vec and fill a map of vector in a GenericContainer with keys in `columns` and `indep` as independent spline
  void
  SplineSet::eval2_D( vec_real_type const & zetas, sizeType indep, vec_string_type const & columns, GenericContainer & gc ) const {
    sizeType npts = sizeType(zetas.size()) ;
    map_type & vals = gc.set_map() ;

    // preallocation
    for ( vec_string_type::const_iterator is = columns.begin() ;
          is != columns.end() ; ++is )
      vals[*is].set_vec_real(npts) ;

    for ( sizeType i = 0 ; i < npts ; ++i ) {
      valueType x ;
      intersect( indep, zetas[i], x ) ;
      for ( vec_string_type::const_iterator is = columns.begin() ;
            is != columns.end() ; ++is ) {
        vec_real_type & v = vals[*is].get_vec_real() ;
        Spline const * p_spl = getSpline( is->c_str() ) ;
        v[i] = p_spl->eval_D(x) ;
      }
    }
  }

  /*                                _       _           _            _   _
  //   ___  ___  ___ ___  _ __   __| |   __| | ___ _ __(_)_   ____ _| |_(_)_   _____
  //  / __|/ _ \/ __/ _ \| '_ \ / _` |  / _` |/ _ \ '__| \ \ / / _` | __| \ \ / / _ \
  //  \__ \  __/ (_| (_) | | | | (_| | | (_| |  __/ |  | |\ V / (_| | |_| |\ V /  __/
  //  |___/\___|\___\___/|_| |_|\__,_|  \__,_|\___|_|  |_| \_/ \__,_|\__|_| \_/ \___|
  */
  void
  SplineSet::eval_DD( valueType x, GenericContainer & gc ) const {
    map_type & vals = gc.set_map() ;
    for ( map<string,sizeType>::const_iterator s_to_pos = header_to_position.begin() ;
          s_to_pos != header_to_position.end() ; ++s_to_pos ) {
      vals[s_to_pos->first] = splines[s_to_pos->second]->eval_DD(x) ;
    }
  }

  //! Evaluate all the splines at `x` values contained in vec and fill a map of vector in a GenericContainer
  void
  SplineSet::eval_DD( vec_real_type const & vec, GenericContainer & gc ) const {
    sizeType npts = sizeType(vec.size()) ;
    map_type & vals = gc.set_map() ;
    for ( map<string,sizeType>::const_iterator s_to_pos = header_to_position.begin() ;
          s_to_pos != header_to_position.end() ; ++s_to_pos ) {
      vec_real_type & v = vals[s_to_pos->first].set_vec_real(npts) ;
      Spline const * p_spl = splines[s_to_pos->second] ;
      for ( sizeType i = 0 ; i < npts ; ++i ) v[i] = p_spl->eval_DD(vec[i]) ;
    }
  }

  //! Evaluate all the splines at `x` and fill a map of values in a GenericContainer with keys in `columns`
  void
  SplineSet::eval_DD( valueType x, vec_string_type const & columns, GenericContainer & gc ) const {
    map_type & vals = gc.set_map() ;
    for ( vec_string_type::const_iterator is = columns.begin() ;
          is != columns.end() ; ++is ) {
      Spline const * p_spl = getSpline( is->c_str() ) ;
      vals[*is] = p_spl->eval_DD(x) ;
    }
  }

  //! Evaluate all the splines at `x` values contained in vec and fill a map of vector in a GenericContainer with keys in `columns`
  void
  SplineSet::eval_DD( vec_real_type const & vec, vec_string_type const & columns, GenericContainer & gc ) const {
    sizeType npts = sizeType(vec.size()) ;
    map_type & vals = gc.set_map() ;
    for ( vec_string_type::const_iterator is = columns.begin() ;
          is != columns.end() ; ++is ) {
      Spline const * p_spl = getSpline( is->c_str() ) ;
      vec_real_type & v = vals[*is].set_vec_real(npts) ;
      for ( sizeType i = 0 ; i < npts ; ++i ) v[i] = p_spl->eval_DD(vec[i]) ;
    }
  }

  //! Evaluate all the splines at `zeta` and fill a map of values in a GenericContainer and `indep` as independent spline
  void
  SplineSet::eval2_DD( valueType zeta, sizeType indep, GenericContainer & gc ) const {
    map_type & vals = gc.set_map() ;
    valueType x ;
    intersect( indep, zeta, x ) ;
    for ( map<string,sizeType>::const_iterator s_to_pos = header_to_position.begin() ;
          s_to_pos != header_to_position.end() ; ++s_to_pos ) {
      vals[s_to_pos->first] = splines[s_to_pos->second]->eval_DD(x) ;
    }
  }

  //! Evaluate all the splines at `zeta` values contained in vec and fill a map of vector in a GenericContainer and `indep` as independent spline
  void
  SplineSet::eval2_DD( vec_real_type const & zetas, sizeType indep, GenericContainer & gc ) const {
    sizeType npts = sizeType(zetas.size()) ;
    map_type & vals = gc.set_map() ;

    // preallocation
    for ( map<string,sizeType>::const_iterator s_to_pos = header_to_position.begin() ;
          s_to_pos != header_to_position.end() ; ++s_to_pos )
      vals[s_to_pos->first].set_vec_real(npts) ;

    for ( sizeType i = 0 ; i < npts ; ++i ) {
      valueType x ;
      intersect( indep, zetas[i], x ) ;
      for ( map<string,sizeType>::const_iterator s_to_pos = header_to_position.begin() ;
            s_to_pos != header_to_position.end() ; ++s_to_pos ) {
        vec_real_type & v = vals[s_to_pos->first].get_vec_real() ;
        v[i] = splines[s_to_pos->second]->eval_DD(x) ;
      }
    }
  }

  //! Evaluate all the splines at `zeta` and fill a map of values in a GenericContainer with keys in `columns` and `indep` as independent spline
  void
  SplineSet::eval2_DD( valueType zeta, sizeType indep, vec_string_type const & columns, GenericContainer & gc ) const {
    map_type & vals = gc.set_map() ;
    valueType x ;
    intersect( indep, zeta, x ) ;
    for ( vec_string_type::const_iterator is = columns.begin() ;
          is != columns.end() ; ++is ) {
      Spline const * p_spl = getSpline( is->c_str() ) ;
      vals[*is] = p_spl->eval_DD(x) ;
    }
  }

  //! Evaluate all the splines at `zeta` values contained in vec and fill a map of vector in a GenericContainer with keys in `columns` and `indep` as independent spline
  void
  SplineSet::eval2_DD( vec_real_type const & zetas, sizeType indep, vec_string_type const & columns, GenericContainer & gc ) const {
    sizeType npts = sizeType(zetas.size()) ;
    map_type & vals = gc.set_map() ;

    // preallocation
    for ( vec_string_type::const_iterator is = columns.begin() ;
          is != columns.end() ; ++is )
      vals[*is].set_vec_real(npts) ;

    for ( sizeType i = 0 ; i < npts ; ++i ) {
      valueType x ;
      intersect( indep, zetas[i], x ) ;
      for ( vec_string_type::const_iterator is = columns.begin() ;
            is != columns.end() ; ++is ) {
        vec_real_type & v = vals[*is].get_vec_real() ;
        Spline const * p_spl = getSpline( is->c_str() ) ;
        v[i] = p_spl->eval_DD(x) ;
      }
    }
  }

  /*
  //   _   _     _         _       _           _            _   _
  //  | |_| |__ (_)_ __ __| |   __| | ___ _ __(_)_   ____ _| |_(_)_   _____
  //  | __| '_ \| | '__/ _` |  / _` |/ _ \ '__| \ \ / / _` | __| \ \ / / _ \
  //  | |_| | | | | | | (_| | | (_| |  __/ |  | |\ V / (_| | |_| |\ V /  __/
  //   \__|_| |_|_|_|  \__,_|  \__,_|\___|_|  |_| \_/ \__,_|\__|_| \_/ \___|
  */
  void
  SplineSet::eval_DDD( valueType x, GenericContainer & gc ) const {
    map_type & vals = gc.set_map() ;
    for ( map<string,sizeType>::const_iterator s_to_pos = header_to_position.begin() ;
          s_to_pos != header_to_position.end() ; ++s_to_pos ) {
      vals[s_to_pos->first] = splines[s_to_pos->second]->eval_DDD(x) ;
    }
  }

  //! Evaluate all the splines at `x` values contained in vec and fill a map of vector in a GenericContainer
  void
  SplineSet::eval_DDD( vec_real_type const & vec, GenericContainer & gc ) const {
    sizeType npts = sizeType(vec.size()) ;
    map_type & vals = gc.set_map() ;
    for ( map<string,sizeType>::const_iterator s_to_pos = header_to_position.begin() ;
          s_to_pos != header_to_position.end() ; ++s_to_pos ) {
      vec_real_type & v = vals[s_to_pos->first].set_vec_real(npts) ;
      Spline const * p_spl = splines[s_to_pos->second] ;
      for ( sizeType i = 0 ; i < npts ; ++i ) v[i] = p_spl->eval_DDD(vec[i]) ;
    }
  }

  //! Evaluate all the splines at `x` and fill a map of values in a GenericContainer with keys in `columns`
  void
  SplineSet::eval_DDD( valueType x, vec_string_type const & columns, GenericContainer & gc ) const {
    map_type & vals = gc.set_map() ;
    for ( vec_string_type::const_iterator is = columns.begin() ;
          is != columns.end() ; ++is ) {
      Spline const * p_spl = getSpline( is->c_str() ) ;
      vals[*is] = p_spl->eval_DDD(x) ;
    }
  }

  //! Evaluate all the splines at `x` values contained in vec and fill a map of vector in a GenericContainer with keys in `columns`
  void
  SplineSet::eval_DDD( vec_real_type const & vec, vec_string_type const & columns, GenericContainer & gc ) const {
    sizeType npts = sizeType(vec.size()) ;
    map_type & vals = gc.set_map() ;
    for ( vec_string_type::const_iterator is = columns.begin() ;
          is != columns.end() ; ++is ) {
      Spline const * p_spl = getSpline( is->c_str() ) ;
      vec_real_type & v = vals[*is].set_vec_real(npts) ;
      for ( sizeType i = 0 ; i < npts ; ++i ) v[i] = p_spl->eval_DDD(vec[i]) ;
    }
  }

  //! Evaluate all the splines at `zeta` and fill a map of values in a GenericContainer and `indep` as independent spline
  void
  SplineSet::eval2_DDD( valueType zeta, sizeType indep, GenericContainer & gc ) const {
    map_type & vals = gc.set_map() ;
    valueType x ;
    intersect( indep, zeta, x ) ;
    for ( map<string,sizeType>::const_iterator s_to_pos = header_to_position.begin() ;
          s_to_pos != header_to_position.end() ; ++s_to_pos ) {
      vals[s_to_pos->first] = splines[s_to_pos->second]->eval_DDD(x) ;
    }
  }

  //! Evaluate all the splines at `zeta` values contained in vec and fill a map of vector in a GenericContainer and `indep` as independent spline
  void
  SplineSet::eval2_DDD( vec_real_type const & zetas, sizeType indep, GenericContainer & gc ) const {
    sizeType npts = sizeType(zetas.size()) ;
    map_type & vals = gc.set_map() ;

    // preallocation
    for ( map<string,sizeType>::const_iterator s_to_pos = header_to_position.begin() ;
          s_to_pos != header_to_position.end() ; ++s_to_pos )
      vals[s_to_pos->first].set_vec_real(npts) ;

    for ( sizeType i = 0 ; i < npts ; ++i ) {
      valueType x ;
      intersect( indep, zetas[i], x ) ;
      for ( map<string,sizeType>::const_iterator s_to_pos = header_to_position.begin() ;
            s_to_pos != header_to_position.end() ; ++s_to_pos ) {
        vec_real_type & v = vals[s_to_pos->first].get_vec_real() ;
        v[i] = splines[s_to_pos->second]->eval_DDD(x) ;
      }
    }
  }

  //! Evaluate all the splines at `zeta` and fill a map of values in a GenericContainer with keys in `columns` and `indep` as independent spline
  void
  SplineSet::eval2_DDD( valueType zeta, sizeType indep, vec_string_type const & columns, GenericContainer & gc ) const {
    map_type & vals = gc.set_map() ;
    valueType x ;
    intersect( indep, zeta, x ) ;
    for ( vec_string_type::const_iterator is = columns.begin() ;
          is != columns.end() ; ++is ) {
      Spline const * p_spl = getSpline( is->c_str() ) ;
      vals[*is] = p_spl->eval_DDD(x) ;
    }
  }

  //! Evaluate all the splines at `zeta` values contained in vec and fill a map of vector in a GenericContainer with keys in `columns` and `indep` as independent spline
  void
  SplineSet::eval2_DDD( vec_real_type const & zetas, sizeType indep, vec_string_type const & columns, GenericContainer & gc ) const {
    sizeType npts = sizeType(zetas.size()) ;
    map_type & vals = gc.set_map() ;

    // preallocation
    for ( vec_string_type::const_iterator is = columns.begin() ;
          is != columns.end() ; ++is )
      vals[*is].set_vec_real(npts) ;

    for ( sizeType i = 0 ; i < npts ; ++i ) {
      valueType x ;
      intersect( indep, zetas[i], x ) ;
      for ( vec_string_type::const_iterator is = columns.begin() ;
            is != columns.end() ; ++is ) {
        vec_real_type & v = vals[*is].get_vec_real() ;
        Spline const * p_spl = getSpline( is->c_str() ) ;
        v[i] = p_spl->eval_DDD(x) ;
      }
    }
  }
}

#endif
