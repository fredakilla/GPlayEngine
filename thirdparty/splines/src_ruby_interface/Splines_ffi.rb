#--------------------------------------------------------------------------#
#  file: Splines_ffi.rb                                                    #
#                                                                          #
#  version: 1.0   date 23/9/2013                                           #
#                                                                          #
#  Copyright (C) 2013                                                      #
#                                                                          #
#      Enrico Bertolazzi                                                   #
#      Dipartimento di Ingegneria Industriale                              #
#      Universita` degli Studi di Trento                                   #
#      Via Mesiano 77, I-38050 Trento, Italy                               #
#      email: enrico.bertolazzi@unitn.it                                   #
#--------------------------------------------------------------------------#

require 'ffi'

module Splines

  extend FFI::Library

  base_path = File.expand_path('../', __FILE__)

  if @libSplines then
    ffi_lib @libSplines
  else
    HOST_OS     = RbConfig::CONFIG['host_os']
    @libSplines = "Splines"
    @ext        = ".noextension"
    case HOST_OS
    when /mac|darwin/
      @ext = ".dylib" ;
    when /linux|cygwin|bsd/
      @ext = ".so" ;
    when /mswin|win|mingw/
      @ext = ".dll" ;
    else
      raise RuntimeError, "Unsupported platform: #{HOST_OS}"
    end
    ffi_lib [ @libSplines+@ext,
              "./lib/"+@libSplines+@ext,
              "../lib/"+@libSplines+@ext,
              "./libs/"+@libSplines+@ext,
              "../libs/"+@libSplines+@ext,
              "lib"+@libSplines+@ext,
              "./lib/lib"+@libSplines+@ext,
              "../lib/lib"+@libSplines+@ext,
              "./libs/lib"+@libSplines+@ext,
              "../libs/lib"+@libSplines+@ext ]
  end

  attach_function :SPLINE_new,           [ :string, :string ], :int
  attach_function :SPLINE_select,        [ :string ], :int
  attach_function :SPLINE_delete,        [ :string ], :int
  attach_function :SPLINE_print,         [], :int
  attach_function :SPLINE_get_type_name, [], :string
  attach_function :SPLINE_mem_ptr,       [ :string ], :pointer
  attach_function :SPLINE_init,          [], :int
  attach_function :SPLINE_push,          [ :double, :double ], :int
  attach_function :SPLINE_build,         [], :int
  attach_function :SPLINE_build2,        [ :buffer_in, :buffer_in, :int ], :int

  attach_function :SPLINE_eval,          [ :double ], :double
  attach_function :SPLINE_eval_D,        [ :double ], :double
  attach_function :SPLINE_eval_DD,       [ :double ], :double
  attach_function :SPLINE_eval_DDD,      [ :double ], :double
end

class Spline
  attr_reader :id

  def initialize(type="pchip")
    @id = self.__id__.to_s
    ok = ::Splines.SPLINE_new @id, type
    if ok != 0 then
      puts "initialize error, check type = #{type}\n"
    end
    return @id
  end

  def clear
    ::Splines.SPLINE_select @id
    ::Splines.SPLINE_init
  end

  def build
    ::Splines.SPLINE_select @id
    ::Splines.SPLINE_build
  end

  def type
    ::Splines.SPLINE_select @id
    ::Splines.SPLINE_get_type_name
  end

  def push_back( x, y )
    ::Splines.SPLINE_select @id
    ::Splines.SPLINE_push x, y
  end

  def value( x )
    ::Splines.SPLINE_select @id
    return ::Splines.SPLINE_eval(x)
  end

  def D( x )
    ::Splines.SPLINE_select @id
    return ::Splines.SPLINE_eval_D(x)
  end

  def DD( x )
    ::Splines.SPLINE_select @id
    return ::Splines.SPLINE_eval_DD(x)
  end

  def DDD( x )
    ::Splines.SPLINE_select @id
    return ::Splines.SPLINE_eval_DDD(x)
  end

end

# EOF: Splines_ffi.rb
