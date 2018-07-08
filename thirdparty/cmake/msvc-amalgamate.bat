set ROOTDIR=%CD%
set LIBDIR=%1
set OUTDIR=%2

echo "Amalgamating target dir: %LIBDIR%"

md "%LIBDIR%"

cd %LIBDIR%
del gplay-deps.lib
md tmp
xcopy *.lib tmp

LIB.EXE /OUT:gplay-deps.lib tmp\*

move gplay-deps.lib %OUTDIR%

rmdir /s /q "tmp\"
cd %ROOTDIR%
