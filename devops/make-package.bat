@echo off

setlocal

if "%~1" == "" (
  echo Package revision must be provided as the first argument
  goto :EOF
)

set PKG_VER=0.6.24
set PKG_REV=%~1

set LIBEXIF_FNAME=libexif-%PKG_VER%.zip 
set LIBEXIF_DNAME=libexif-%PKG_VER%
set LIBEXIF_SHA256=12fa9fc73d3610f752f9a0ef5da1269e76b1caab7aca83f5174ca0c9565ca802

set PATCH=c:\Program Files\Git\usr\bin\patch.exe
set SEVENZIP_EXE=c:\Program Files\7-Zip\7z.exe
set VCVARSALL=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall

curl --location --output %LIBEXIF_FNAME% https://github.com/libexif/libexif/releases/download/v%PKG_VER%/%LIBEXIF_FNAME%

"%SEVENZIP_EXE%" h -scrcSHA256 %LIBEXIF_FNAME% | findstr /C:"SHA256 for data" | call devops\check-sha256 "%LIBEXIF_SHA256%"

if ERRORLEVEL 1 (
  echo SHA-256 signature for %LIBEXIF_FNAME% does not match
  goto :EOF
)

"%SEVENZIP_EXE%" x %LIBEXIF_FNAME%

cd %LIBEXIF_DNAME%

copy /y config.h.in config.h

rem
rem Patch the source to work around build problems
rem
"%PATCH%" --unified --input ..\patches\config.h.patch config.h

mkdir ..\nuget\licenses
copy COPYING ..\nuget\licenses\

rem
rem Header files
rem
mkdir ..\nuget\build\native\include\libexif
copy config.h ..\nuget\build\native\include\
xcopy /Y /S libexif\*.h ..\nuget\build\native\include\libexif\

rem
rem x64 Debug
rem 

call "%VCVARSALL%" x64

nmake -f ..\devops\Makefile.msc DEBUG=1

call ..\devops\copy-build x64 Debug

nmake -f ..\devops\Makefile.msc DEBUG=1 clean

rem
rem x64 Release
rem 

nmake -f ..\devops\Makefile.msc

call ..\devops\copy-build x64 Release

nmake -f ..\devops\Makefile.msc clean

cd ..

rem
rem Create a package
rem

nuget pack nuget\StoneSteps.libexif.Static.nuspec -Version %PKG_VER%.%PKG_REV%
