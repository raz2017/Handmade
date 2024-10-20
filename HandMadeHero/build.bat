@echo off

mkdir ..\build
pushd ..\build
cl -Zi ..\HandMadeHero\HandMadeHero.cpp user32.lib
popd
