@echo off

set PWD=%~dp0
set MYDIR=%1
set OUTDIR=%2
set ARCHBITS=%3
set CONFIG=%4

set VC_VER=vc110

rem set BOOST_VER=1_55
rem set BOOST_LIBROOT=%BOOST_LIB64%
rem if "%ARCHBITS%"=="32" set BOOST_LIBROOT=%BOOST_LIB32%
rem 
rem set BOOST_LIBCONF=mt
rem if %CONFIG%==debug set BOOST_LIBCONF=mt-gd
rem if %CONFIG%==Debug set BOOST_LIBCONF=mt-gd
rem if %CONFIG%==DEBUG set BOOST_LIBCONF=mt-gd
rem set BOOST_LIBTAG=%VC_VER%-%BOOST_LIBCONF%-%BOOST_VER%

set VC_ROOT=%VS110COMNTOOLS%..\..\VC
set VCRARCH=x64
if "%ARCHBITS%"=="32" set VCRARCH=x86
set RTL_ROOT=%VC_ROOT%\redist\%VCRARCH%\Microsoft.VC110.CRT
if "%CONFIG%"=="debug" set RTL_ROOT=%VC_ROOT%\redist\Debug_NonRedist\%VCRARCH%\Microsoft.VC110.DebugCRT
set VCRD=
if "%CONFIG%"=="debug" set VCRD=d

echo.
echo ARCHBITS      = %ARCHBITS%
echo RTL ROOT      = `%RTL_ROOT%`
echo CONFIG        = `%CONFIG%`
echo MYDIR         = `%MYDIR%`
echo OUTPUT        = `%OUTDIR%`
rem echo BOOST         = `%BOOST_ROOT%`
rem echo BOOST LIB     = `%BOOST_LIBROOT%`
rem echo BOOST LIB TAG = `%BOOST_LIBTAG%`
echo.

echo.
echo Deploy MSVC RTL libs...
xcopy /F/Y "%RTL_ROOT%\msvcp110%VCRD%.dll" %OUTDIR%
xcopy /F/Y "%RTL_ROOT%\msvcr110%VCRD%.dll" %OUTDIR%

rem echo.
rem echo Deploy BOOST %BOOST_VER% libs...
rem xcopy /F/Y %BOOST_LIBROOT%\boost_thread-%BOOST_LIBTAG%.dll %OUTDIR%
rem xcopy /F/Y %BOOST_LIBROOT%\boost_system-%BOOST_LIBTAG%.dll %OUTDIR%
rem xcopy /F/Y %BOOST_LIBROOT%\boost_chrono-%BOOST_LIBTAG%.dll %OUTDIR%

echo.
echo Deploy done.
