# get the type of OS currently running
OS=$(shell uname)
PWD=$(shell pwd)

LIB_SPLINE = libSplines.a
LIB_GC     = libGenericContainer.a

CC   = gcc
CXX  = g++
INC  = -I./src -I./include
LIBS = -L./lib -lSplines -lGenericContainer
DEFS =

# check if the OS string contains 'Linux'
ifneq (,$(findstring Linux, $(OS)))
  WARN = -Wall
  CC  = gcc
  CXX = g++
  # activate C++11 for g++ >= 4.9
  VERSION  = $(shell $(CC) -dumpversion)
ifneq (,$(findstring 4.9, $(VERSION)))
  CXX += -std=c++11 -pthread
endif
ifneq (,$(findstring 5., $(VERSION)))
  CXX += -std=c++11 -pthread
endif
ifneq (,$(findstring 6., $(VERSION)))
  CXX += -std=c++11 -pthread
endif
  CC  += $(WARN)
  CXX += $(WARN)
  LIBS     = -static -L./lib -lSplines -lGenericContainer
  CXXFLAGS = -Wall -O3 -fPIC -Wno-sign-compare
  AR       = ar rcs
endif

# check if the OS string contains 'Darwin'
ifneq (,$(findstring Darwin, $(OS)))
  WARN    = -Weverything -Wno-reserved-id-macro -Wno-padded
  CC      = clang
  CXX     = clang++
  VERSION = $(shell $(CC) --version 2>&1 | grep -o "Apple LLVM version [0-9]\.[0-9]\.[0-9]" | grep -o " [0-9]\.")
ifneq (,$(findstring 8., $(VERSION)))
  CXX += -std=c++11 -stdlib=libc++ 
endif
ifneq (,$(findstring 7., $(VERSION)))
  CXX += -std=c++11 -stdlib=libc++ 
endif
  CC  += $(WARN)
  CXX += $(WARN)
  LIBS     = -L./lib -lSplines -lGenericContainer
  CXXFLAGS = -Wall -O3 -fPIC -Wno-sign-compare
  AR       = libtool -static -o
endif

SRCS = \
src/SplineAkima.cc \
src/SplineAkima2D.cc \
src/SplineBSpline.cc \
src/SplineBessel.cc \
src/SplineBiCubic.cc \
src/SplineBiQuintic.cc \
src/SplineBilinear.cc \
src/SplineConstant.cc \
src/SplineCubic.cc \
src/SplineCubicBase.cc \
src/SplineHermite.cc \
src/SplineLinear.cc \
src/SplinePchip.cc \
src/SplineQuintic.cc \
src/SplineQuinticBase.cc \
src/SplineSet.cc \
src/SplineSetGC.cc \
src/Splines.cc \
src/SplinesBivariate.cc \
src/SplinesCinterface.cc \
src/SplinesUnivariate.cc

OBJS  = $(SRCS:.cc=.o)
DEPS  = src/Splines.hh src/SplinesCinterface.h
MKDIR = mkdir -p

# prefix for installation, use make PREFIX=/new/prefix install
# to override
PREFIX    = /usr/local
FRAMEWORK = Splines

all: gc lib
	mkdir -p bin
	$(CXX) $(INC) $(CXXFLAGS) -o bin/test1 tests/test1.cc $(LIBS)
	$(CXX) $(INC) $(CXXFLAGS) -o bin/test2 tests/test2.cc $(LIBS)
	$(CXX) $(INC) $(CXXFLAGS) -o bin/test3 tests/test3.cc $(LIBS)
	$(CXX) $(INC) $(CXXFLAGS) -o bin/test4 tests/test4.cc $(LIBS)
	$(CXX) $(INC) $(CXXFLAGS) -o bin/test5 tests/test5.cc $(LIBS)
	$(CXX) $(INC) $(CXXFLAGS) -o bin/test6 tests/test6.cc $(LIBS)
	$(CXX) $(INC) $(CXXFLAGS) -o bin/test7 tests/test7.cc $(LIBS)
	$(CXX) $(INC) $(CXXFLAGS) -o bin/test8 tests/test8.cc $(LIBS)
	$(CXX) $(INC) $(CXXFLAGS) -o bin/test9 tests/test9.cc $(LIBS)
	$(CXX) $(INC) $(CXXFLAGS) -o bin/test10 tests/test10.cc $(LIBS)

gc: lib/$(LIB_GC)

lib: gc lib/$(LIB_SPLINE)

include_local:
	@rm -rf lib/include
	$(MKDIR) lib
	$(MKDIR) lib/include
	@cp -f src/*.h* lib/include

src/%.o: src/%.cc $(DEPS)
	$(CXX) $(INC) $(CXXFLAGS) $(DEFS) -c $< -o $@ 

src/%.o: src/%.c $(DEPS)
	$(CC) $(INC) $(CFLAGS) $(DEFS) -c -o $@ $<

lib/libSplines.a: $(OBJS) include_local
	$(AR) lib/libSplines.a $(OBJS) 

lib/libSplines.dylib: $(OBJS) include_local
	$(CXX) -shared -o lib/libSplines.dylib $(OBJS) 

lib/libSplines.so: $(OBJS) include_local
	$(CXX) -shared -o lib/libSplines.so $(OBJS) 

lib/$(LIB_GC):
	#rm -rf GC ; git clone --depth 1 git@github.com:ebertolazzi/GenericContainer.git GC
	rm -rf GC ; git clone --depth 1 https://github.com/ebertolazzi/GenericContainer.git GC
ifneq (,$(findstring Linux, $(OS)))
	cd GC ; ruby gcc_workaround.rb ; cd ..
endif
	$(MKDIR) include ; cd GC ; make CXXFLAGS="$(CXXFLAGS)" CC="$(CC)" CXX=-"$(CXX)" lib ; make PREFIX="$(PWD)" install 

install_local: lib
	$(MKDIR) ./lib/include
	cp GC/lib/include/*        ./lib/include
	cp src/Splines.hh          ./lib/include
	cp src/SplinesCinterface.h ./lib/include

install: lib
	cp src/Splines.hh          $(PREFIX)/include
	cp src/SplinesCinterface.h $(PREFIX)/include
	cp lib/$(LIB_SPLINE)       $(PREFIX)/lib

install_as_framework: lib
	$(MKDIR) $(PREFIX)/include/$(FRAMEWORK)
	cp src/Splines.hh          $(PREFIX)/include/$(FRAMEWORK)
	cp src/SplinesCinterface.h $(PREFIX)/include/$(FRAMEWORK)
	cp lib/$(LIB_SPLINE)       $(PREFIX)/lib

run:
	./bin/test1
	./bin/test2
	./bin/test3
	./bin/test4
	./bin/test5
	./bin/test6
	./bin/test7
	./bin/test8
	./bin/test9
	./bin/test10

doc:
	doxygen
	
clean:
	rm -rf lib/libSplines.* lib/libGenericContainer.* lib/include src/*.o

	rm -rf bin
	