@echo off

setlocal

set LIBEXIF_STATICLIB=libexif.lib
set LIBEXIF_STATICLIB_PDB=libexif.pdb

set PLATFORM=%~1
set CONFIG=%~2

mkdir "..\nuget\build\native\lib\%PLATFORM%\%CONFIG%"

copy /Y "build\%CONFIG%\%LIBEXIF_STATICLIB%" "..\nuget\build\native\lib\%PLATFORM%\%CONFIG%\"
copy /Y "build\%CONFIG%\%LIBEXIF_STATICLIB_PDB%" "..\nuget\build\native\lib\%PLATFORM%\%CONFIG%\"
