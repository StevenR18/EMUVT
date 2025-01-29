@echo off
set compiler=%1
set flagcl=/Zi /Od
set flaggcc=-Wall -g -o
set dircfile=..\..\Vt\src\
set libwnd=user32.lib gdi32.lib
set cfile=%dircfile%main_datametter.c %dircfile%keyboard.c %dircfile%liked_list.c ^
%dircfile%font.c %dircfile%memory_traker.c %dircfile%newwvt.c %dircfile%timer.c ^
%dircfile%window64_platform.c %libwnd%
if "%compiler%" == "cl" (
if "%2" == "" (
echo variables de entorno ya estan configuradas
goto ccl
) else (
echo Configurando variables de entorno
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall" x64
goto ccl
)
) else if "%compiler%" == "gcc" (
goto cgcc
)
:cgcc
%compiler% %flaggcc% %3 %2 
goto end 
:ccl
pushd.
cd ..\Vt\Build
%compiler% %flagcl% %cfile%
popd
pause
goto end
:end
pause