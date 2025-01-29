@echo off
set dirsrc=.\src
set dirinclude=.\includes
set dirbuild=.\Build
pushd .
cd %dirsrc%
del *.c~ #*
popd
pushd .
cd %dirinclude%
del *.h~ #*
popd
pushd .
cd %dirbuild%
del /Q *
popd
pause