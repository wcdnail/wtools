@echo off

set PWD=%~dp0
set MYDIR=%1
set OUTDIR=%2
set ARCHBITS=%3
set CONFIG=%4
set VC_VER=%5
set VC_REFVER=%6
set VC_INSTDIR=%7

rem set BOOST_VER=1_55
rem set BOOST_LIBROOT=%BOOST_LIB64%
rem if "%ARCHBITS%"=="32" set BOOST_LIBROOT=%BOOST_LIB32%
rem 
rem set BOOST_LIBCONF=mt
rem if %CONFIG%==debug set BOOST_LIBCONF=mt-gd
rem if %CONFIG%==Debug set BOOST_LIBCONF=mt-gd
rem if %CONFIG%==DEBUG set BOOST_LIBCONF=mt-gd
rem set BOOST_LIBTAG=%VC_VER%-%BOOST_LIBCONF%-%BOOST_VER%

set RTL_ROOT=%VC_INSTDIR%

echo.
echo VC_VER        = '%VC_VER%'
echo VC_REFVER     = '%VC_REFVER%'
echo ARCHBITS      = '%ARCHBITS%'
echo RTL ROOT      = '%RTL_ROOT%'
echo CONFIG        = '%CONFIG%'
echo MYDIR         = '%MYDIR%'
echo OUTPUT        = '%OUTDIR%'
echo BOOST         = '%VCPKG_ROOT%'
echo.

rem echo.
rem echo Deploy MSVC RTL libs...
rem xcopy /F/Y "%RTL_ROOT%\msvcp110%VCRD%.dll" %OUTDIR%
rem xcopy /F/Y "%RTL_ROOT%\msvcr110%VCRD%.dll" %OUTDIR%

rem echo.
rem echo Deploy BOOST %BOOST_VER% libs...
rem xcopy /F/Y %BOOST_LIBROOT%\boost_thread-%BOOST_LIBTAG%.dll %OUTDIR%
rem xcopy /F/Y %BOOST_LIBROOT%\boost_system-%BOOST_LIBTAG%.dll %OUTDIR%
rem xcopy /F/Y %BOOST_LIBROOT%\boost_chrono-%BOOST_LIBTAG%.dll %OUTDIR%

rem echo.
rem echo Deploy done.
