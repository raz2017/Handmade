@echo off

mkdir ..\build
pushd ..\build
cl -FC -Zi ..\HandMadeHero\HandMadeHero.cpp user32.lib
popd
