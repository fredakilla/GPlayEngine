set ROOTDIR=%CD%
set LIBDIR=%1

echo "Amalgamating target dir: %LIBDIR%"

cd %LIBDIR%
mkdir tmp
del gplay3d-deps.lib
move *.lib tmp

LIB.EXE /OUT:gplay3d-deps.lib tmp\*

rmdir /s /q "tmp\"
cd %ROOTDIR%
