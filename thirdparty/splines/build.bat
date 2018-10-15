@IF [%1] EQU [] (SET YEAR=2013) else (SET YEAR=%1)
@IF [%2] EQU [] (SET BITS=x64)  else (SET BITS=%2)

@IF %YEAR% == 2010 (
  @set STR="Visual Studio 10 2010"
) ELSE IF %YEAR% == 2012 (
  @set STR=Visual Studio 11 2012
) ELSE IF %YEAR% == 2013 (
  @set STR=Visual Studio 12 2013
) ELSE IF %YEAR% == 2015 (
  @set STR=Visual Studio 14 2015
) ELSE IF %YEAR% == 2017 (
  @set STR=Visual Studio 15 2017
) ELSE (
  @echo.
  powershell -command write-host -foreground "red" -background "yellow" -nonewline "Unsupported %YEAR%"
  @echo.
  GOTO:eof
)

@IF "%BITS%" NEQ "x86" IF "%BITS%" NEQ "x64" (
  @echo.
  powershell -command write-host -foreground "red" -background "yellow" -nonewline "Unsupported ARCH %BITS%"
  @echo.
  GOTO:eof
)

@IF "%BITS%" == "x64" (@set STR=%STR% Win64)

@IF NOT EXIST lib\Debug\Splines.lib (

  @IF NOT EXIST GC (
    @echo off
    @powershell -command write-host -foreground "red" -background "yellow" -nonewline "Download GenericContainer"
    @rmdir /S GC
    @git clone --depth 1 git@github.com:ebertolazzi/GenericContainer.git GC
  )

  @SET VSDIR=vs%YEAR%_%BITS%

  @RMDIR /S /Q %VSDIR%
  @mkdir %VSDIR%
  @cd %VSDIR%
  cmake -G "%STR%" -D%LAPACK%=1 -DYEAR=%YEAR% -DBITS=%BITS% -DCMAKE_INSTALL_PREFIX:PATH=..\lib ..
  cmake --build . --config Release --target Install
  cmake --build . --config Debug --target Install
  @cd ..
) else (
  @echo.
  powershell -command write-host -foreground "red" -background "yellow" -nonewline "Splines already compiled"
  @echo.
)


