@echo off

mkdir ..\build
pushd ..\build
cl -FC -Zi ..\HandMadeHero\HandMadeHero.cpp user32.lib gdi32.lib /DUNICODE /D_UNICODE
popd
