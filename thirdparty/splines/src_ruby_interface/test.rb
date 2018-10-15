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

require './Splines_ffi.rb'

xx0 = [  0,  1,  2,  3,  4,  5,    6,  7,  8,  9, 10 ] 
yy0 = [ 10, 10, 10, 10, 10, 10, 10.5, 15, 50, 60, 85 ]

spline = Spline.new

xx0.zip(yy0).each { |p| p p ; spline.push_back(p[0],p[1]) }

spline.build
puts spline.type

File.open( "out.txt", 'w') do |f|
  f.puts "x\ty\tdy\tddy"
  (0..1000).each do |i|
    x = i/100.0
    f.puts "#{x}\t#{spline.value(x)}\t#{spline.D(x)}\t#{spline.DD(x)}"
  end
end

# EOF: Splines_ffi.rb
