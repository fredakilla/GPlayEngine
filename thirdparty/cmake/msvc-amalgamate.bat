set ROOTDIR=%CD%
set LIBDIR=%1

echo "Amalgamating target dir: %LIBDIR%"


md "%LIBDIR%"


cd %LIBDIR%
del gplay-deps.lib
md tmp
xcopy ..\..\libtmp\*.lib tmp

LIB.EXE /OUT:gplay-deps.lib tmp\*

rmdir /s /q "tmp\"
cd %ROOTDIR%
